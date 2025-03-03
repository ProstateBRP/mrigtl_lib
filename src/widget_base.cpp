/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "widget_base.h"
#include "signal_manager.h"
#include "listener_base.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>

namespace mrigtlbridge {

WidgetBase::WidgetBase(QObject* parent)
    : QObject(parent),
      signalManager(nullptr),
      listener(nullptr) {
}

WidgetBase::~WidgetBase() {
    // Ensure the listener is properly stopped and deleted
    if (listener) {
        listener->stop(); // This will wait for the thread to finish
        
        // Safe deletion - make sure we're not in the thread's context
        if (listener->thread() != QThread::currentThread()) {
            listener->moveToThread(QThread::currentThread());
        }
        
        delete listener;
        listener = nullptr;
    }
}

void WidgetBase::buildGUI(QWidget* parent) {
    // To be implemented by derived classes
}

void WidgetBase::updateGUI(const QString& state) {
    // To be implemented by derived classes
    Q_UNUSED(state);
}

void WidgetBase::setSignalManager(SignalManager* sm) {
    signalManager = sm;
    signalManager->connectSlot("listenerConnected", this, SLOT(onListenerConnected(QString)));
    signalManager->connectSlot("listenerDisconnected", this, SLOT(onListenerDisconnected(QString)));
    signalManager->connectSlot("listenerTerminated", this, SLOT(onListenerTerminated(QString)));
}

void WidgetBase::startListener() {
    if (!signalManager) {
        qCritical() << "SignalManager is not set!";
        return;
    }

    if (!listener) {
        try {
            // Create listener instance based on listener_class
            // Note: In C++, we'll need to use a factory pattern or similar to create the right type
            // For simplicity, this should be implemented in derived classes
            // that know the specific listener type they need
            
            /* Example for a specific class:
            listener = new IGTLListener();
            listener->connectSlots(signalManager);
            listener->configure(listenerParameter);
            listener->start();
            */
            
            // This method should be overridden by derived classes
            qDebug() << "WidgetBase::startListener() - This method should be overridden by derived classes";
        } catch (const std::exception& e) {
            qCritical() << "Failed to start Listener: " << e.what();
            if (listener) {
                listener->stop();
                listener->disconnectSlots();
                delete listener;
                listener = nullptr;
            }
        }
    } else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("A listener is already running. Kill it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);
        int button = msgBox.exec();
        
        if (button == QMessageBox::Yes) {
            listener->stop();
            delete listener;
            listener = nullptr;
        }
    }
}

void WidgetBase::stopListener() {
    qDebug() << "stopListener() is called.";
    if (listener) {
        // First signal that we're disconnecting for UI updates
        updateGUI("listenerDisconnected");
        
        // Stop the listener thread safely
        listener->stop();
        
        // Make sure we're on the right thread before deleting
        if (listener->thread() != QThread::currentThread()) {
            listener->moveToThread(QThread::currentThread());
        }
        
        delete listener;
        listener = nullptr;
    }
}

void WidgetBase::onListenerConnected(const QString& className) {
    if (listener && listener->metaObject()->className() == className) {
        updateGUI("listenerConnected");
    }
}

void WidgetBase::onListenerDisconnected(const QString& className) {
    if (listener && listener->metaObject()->className() == className) {
        delete listener;
        listener = nullptr;
        updateGUI("listenerDisconnected");
    }
}

void WidgetBase::onListenerTerminated(const QString& className) {
    if (listener && listener->metaObject()->className() == className) {
        listener = nullptr;
        updateGUI("listenerDisconnected");
    }
}

} // namespace mrigtlbridge