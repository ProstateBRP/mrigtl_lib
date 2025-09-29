/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtl_lib

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#pragma once

#include "mrigtl_lib_export.h"
#include <QObject>
#include <QThread>
#include <QMap>
#include <QString>
#include <QVariant>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

namespace mrigtlbridge {

// Forward declarations
class SignalManagerProxy;
class SignalWrap;
class SignalWrapVoid;
class SignalWrapStr;
class SignalWrapDict;

class MRIGTL_LIB_EXPORT SignalManager : public QObject {
    Q_OBJECT

public:
    explicit SignalManager(QObject* parent = nullptr);
    ~SignalManager();

    bool addSlot(const QString& name, const QString& paramType);
    bool addCustomSignal(const QString& name, const QString& paramType);
    bool addCustomSlot(const QString& name, const QString& paramType, QObject* receiver, const char* slot);
    bool connectSlot(const QString& name, QObject* receiver, const char* slot);
    bool disconnectSlot(const QString& name, QObject* receiver = nullptr, const char* slot = nullptr);
    bool emitSignal(const QString& name, const QVariant& param = QVariant());

    SignalManagerProxy* getSignalManagerProxy();

private:
    friend class SignalManagerProxy;
    friend class SignalWrap;
    friend class SignalWrapVoid;
    friend class SignalWrapStr;
    friend class SignalWrapDict;

    QMap<QString, SignalWrap*> signalMap; // renamed from signals to avoid conflict with Qt's signals keyword
    std::unique_ptr<SignalManagerProxy> signalManagerProxy;
};

class MRIGTL_LIB_EXPORT SignalManagerProxy : public QThread {
    Q_OBJECT

public:
    SignalManagerProxy(QObject* parent = nullptr);
    ~SignalManagerProxy();

    void setSignalManager(SignalManager* signalManager);
    void emitSignal(const QString& name, const QVariant& param = QVariant());
    void run() override;

private:
    struct SignalData {
        QString name;
        QVariant param;
    };

    SignalManager* signalManager;
    std::queue<SignalData> signalQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> stopRequested;
};

} // namespace mrigtlbridge