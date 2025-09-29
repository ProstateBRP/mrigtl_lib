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

class SignalManager : public QObject {
    Q_OBJECT

public:
    MRIGTL_LIB_EXPORT explicit SignalManager(QObject* parent = nullptr);
    MRIGTL_LIB_EXPORT ~SignalManager();

    MRIGTL_LIB_EXPORT bool addSlot(const QString& name, const QString& paramType);
    MRIGTL_LIB_EXPORT bool addCustomSignal(const QString& name, const QString& paramType);
    MRIGTL_LIB_EXPORT bool addCustomSlot(const QString& name, const QString& paramType, QObject* receiver, const char* slot);
    MRIGTL_LIB_EXPORT bool connectSlot(const QString& name, QObject* receiver, const char* slot);
    MRIGTL_LIB_EXPORT bool disconnectSlot(const QString& name, QObject* receiver = nullptr, const char* slot = nullptr);
    MRIGTL_LIB_EXPORT bool emitSignal(const QString& name, const QVariant& param = QVariant());

    MRIGTL_LIB_EXPORT SignalManagerProxy* getSignalManagerProxy();

private:
    friend class SignalManagerProxy;
    friend class SignalWrap;
    friend class SignalWrapVoid;
    friend class SignalWrapStr;
    friend class SignalWrapDict;

    QMap<QString, SignalWrap*> signalMap; // renamed from signals to avoid conflict with Qt's signals keyword
    std::unique_ptr<SignalManagerProxy> signalManagerProxy;
};

class SignalManagerProxy : public QThread {
    Q_OBJECT

public:
    MRIGTL_LIB_EXPORT SignalManagerProxy(QObject* parent = nullptr);
    MRIGTL_LIB_EXPORT ~SignalManagerProxy();

    MRIGTL_LIB_EXPORT void setSignalManager(SignalManager* signalManager);
    MRIGTL_LIB_EXPORT void emitSignal(const QString& name, const QVariant& param = QVariant());
    MRIGTL_LIB_EXPORT void run() override;

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