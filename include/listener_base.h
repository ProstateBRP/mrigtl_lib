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

#include <QThread>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <memory>
#include <atomic>

namespace mrigtlbridge {

class SignalManager;

class ListenerBase : public QThread {
    Q_OBJECT

public:
    explicit ListenerBase(QObject* parent = nullptr);
    virtual ~ListenerBase();

    // Configure listener parameters
    virtual void configure(const QVariantMap& params);

    // Connect listener to signal manager
    virtual void connectSlots(SignalManager* signalManager);

    // Disconnect listener from signal manager
    virtual void disconnectSlots();

    // Stop the listener thread
    virtual void stop();

    // Map of custom signals that will be registered with signal manager
    QMap<QString, QString> customSignalList;

    void setProcessTimeout(time_t timeout) {
        processTimeout = timeout;
        if (processTimer) {
            processTimer->setInterval(processTimeout);
        }
    }

    // Get current parameters
    QVariantMap getParameters() const { return parameter; }

protected slots:
    // Main processing function driven by a timer (to be implemented by subclasses)
    virtual void process();


protected:
    // Main thread function (override from QThread)
    void run() override;

    // Initialize the listener (to be implemented by subclasses)
    virtual bool initialize();

    // Finalize when thread stops (to be implemented by subclasses)
    virtual void finalize();

    std::atomic<bool> threadActive;
    SignalManager* signalManager;
    QVariantMap parameter;
    QTimer* processTimer = nullptr;
    time_t processTimeout = 50; // Default processing interval in milliseconds

};

} // namespace mrigtlbridge
