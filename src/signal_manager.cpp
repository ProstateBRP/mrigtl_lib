/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtl_lib

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "signal_manager.h"
#include "signal_wrap.h"
#include "common.h"
#include <QDebug>
#include <QMetaObject>
#include <QThread>

namespace mrigtlbridge {

SignalManager::SignalManager(QObject* parent) : QObject(parent) {
    // Initialize signals from common.h
    for (const auto& [name, type] : SignalNames) {
        addSlot(QString::fromStdString(name), QString::fromStdString(type));
    }

    // Create and start signal manager proxy
    signalManagerProxy = std::make_unique<SignalManagerProxy>();
    signalManagerProxy->setSignalManager(this);
    signalManagerProxy->start();
}

SignalManager::~SignalManager() {
    // Stop the proxy thread before deleting signals
    if (signalManagerProxy) {
        signalManagerProxy->terminate();
        signalManagerProxy->wait(1000); // Wait for the thread to finish, timeout after 1 second
    }
    
    // Clean up signals
    for (auto signal : signalMap.values()) {
        delete signal;
    }
}

bool SignalManager::addSlot(const QString& name, const QString& paramType) {
    qDebug() << "SignalManager::addSlot(" << name << ", " << paramType << ")";
    
    if (signalMap.contains(name)) {
        if (paramType == signalMap[name]->paramType) {
            qDebug() << "SignalManager::addSlot(): Slot already exists.";
        } else {
            qDebug() << "SignalManager::addSlot(): The parameter type conflicts with the existing slot";
        }
        return false;
    }
    
    if (paramType.isEmpty()) {
        signalMap[name] = new SignalWrapVoid();
    } else if (paramType == "str") {
        signalMap[name] = new SignalWrapStr();
    } else if (paramType == "dict") {
        signalMap[name] = new SignalWrapDict();
    } else {
        qDebug() << "SignalManager::addSlot(): Illegal parameter type.";
        return false;
    }
    
    return true;
}

bool SignalManager::addCustomSignal(const QString& name, const QString& paramType) {
    return addSlot(name, paramType);
}

bool SignalManager::addCustomSlot(const QString& name, const QString& paramType, QObject* receiver, const char* slot) {
    qDebug() << "SignalManager::addCustomSlot(" << name << ")";
    if (addSlot(name, paramType)) {
        return connectSlot(name, receiver, slot);
    }
    return false;
}

bool SignalManager::connectSlot(const QString& name, QObject* receiver, const char* slot) {
    qDebug() << "SignalManager::connectSlot(" << name << ")";
    if (signalMap.contains(name)) {
        if (signalMap[name]->paramType.isEmpty()) {
            return QObject::connect(qobject_cast<SignalWrapVoid*>(signalMap[name]), SIGNAL(signal()), receiver, slot);
        } else if (signalMap[name]->paramType == "str") {
            return QObject::connect(qobject_cast<SignalWrapStr*>(signalMap[name]), SIGNAL(signal(QString)), receiver, slot);
        } else if (signalMap[name]->paramType == "dict") {
            return QObject::connect(qobject_cast<SignalWrapDict*>(signalMap[name]), SIGNAL(signal(QVariantMap)), receiver, slot);
        }
    }
    return false;
}

bool SignalManager::disconnectSlot(const QString& name, QObject* receiver, const char* slot) {
    qDebug() << "SignalManager::disconnectSlot(" << name << ")";
    if (signalMap.contains(name)) {
        if (receiver) {
            if (signalMap[name]->paramType.isEmpty()) {
                return QObject::disconnect(qobject_cast<SignalWrapVoid*>(signalMap[name]), SIGNAL(signal()), receiver, slot);
            } else if (signalMap[name]->paramType == "str") {
                return QObject::disconnect(qobject_cast<SignalWrapStr*>(signalMap[name]), SIGNAL(signal(QString)), receiver, slot);
            } else if (signalMap[name]->paramType == "dict") {
                return QObject::disconnect(qobject_cast<SignalWrapDict*>(signalMap[name]), SIGNAL(signal(QVariantMap)), receiver, slot);
            }
        } else {
            if (signalMap[name]->paramType.isEmpty()) {
                return QObject::disconnect(qobject_cast<SignalWrapVoid*>(signalMap[name]), SIGNAL(signal()), nullptr, nullptr);
            } else if (signalMap[name]->paramType == "str") {
                return QObject::disconnect(qobject_cast<SignalWrapStr*>(signalMap[name]), SIGNAL(signal(QString)), nullptr, nullptr);
            } else if (signalMap[name]->paramType == "dict") {
                return QObject::disconnect(qobject_cast<SignalWrapDict*>(signalMap[name]), SIGNAL(signal(QVariantMap)), nullptr, nullptr);
            }
        }
    }
    return false;
}

bool SignalManager::emitSignal(const QString& name, const QVariant& param) {
    // Avoid unnecessary copying that can cause heap corruption with large data
    // Use references where possible to prevent memory issues
    
    if (signalMap.contains(name)) {
        return signalMap[name]->emitSignal(param);
    } else {
        return false;
    }
}

SignalManagerProxy* SignalManager::getSignalManagerProxy() {
    return signalManagerProxy.get();
}

// SignalManagerProxy implementation
SignalManagerProxy::SignalManagerProxy(QObject* parent) : QThread(parent), signalManager(nullptr), stopRequested(false) {
}

SignalManagerProxy::~SignalManagerProxy() {
    stopRequested = true;
    queueCondition.notify_all();
    wait();
}

void SignalManagerProxy::setSignalManager(SignalManager* manager) {
    signalManager = manager;
}

void SignalManagerProxy::emitSignal(const QString& name, const QVariant& param) {
    SignalData data;
    data.name = name;
    data.param = param;
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        signalQueue.push(data);
    }
    queueCondition.notify_one();
}

void SignalManagerProxy::run() {
    while (!stopRequested) {
        SignalData data;
        bool hasData = false;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (signalQueue.empty()) {
                queueCondition.wait_for(lock, std::chrono::milliseconds(100));
            }
            
            if (!signalQueue.empty()) {
                data = signalQueue.front();
                signalQueue.pop();
                hasData = true;
            }
        }
        
        if (hasData && signalManager) {
            // Use QMetaObject::invokeMethod to safely emit signals from this thread
            QMetaObject::invokeMethod(signalManager, [this, data]() {
                signalManager->emitSignal(data.name, data.param);
            }, Qt::QueuedConnection);
        }
        
        QThread::msleep(5);
    }
}

} // namespace mrigtlbridge