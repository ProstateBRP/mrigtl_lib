/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtl_lib

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "listener_base.h"
#include "signal_manager.h"
#include <QDebug>

namespace mrigtlbridge {

ListenerBase::ListenerBase(QObject* parent) 
    : QThread(parent),
      threadActive(false),
      signalManager(nullptr) {
}

ListenerBase::~ListenerBase() {
    qDebug() << "ListenerBase::~ListenerBase()";
    if (signalManager) {
        signalManager->emitSignal("listenerTerminated", metaObject()->className());
    }
}

void ListenerBase::configure(const QVariantMap& params) {
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        if (parameter.contains(it.key())) {
            parameter[it.key()] = it.value();
        }
    }
}

void ListenerBase::connectSlots(SignalManager* sm) {
    signalManager = sm;
}

void ListenerBase::disconnectSlots() {
    // To be overridden by subclasses if needed
}

void ListenerBase::stop() {
    qDebug() << "ListenerBase::stop() - Stopping thread" << metaObject()->className();
    threadActive = false;
    
    // First, quit the event loop gracefully
    if (isRunning()) {
        quit();
        
        // Wait for the thread to finish (with timeout)
        if (!wait(2000)) { // Wait for 2 seconds
            qDebug() << "ListenerBase::stop() - Thread did not finish gracefully, terminating";
            // If the thread doesn't finish within the timeout, terminate it
            terminate();
            wait();
        } else {
            qDebug() << "ListenerBase::stop() - Thread finished gracefully";
        }
    }
}

void ListenerBase::run() {
    // Note: metaObject()->className() gives the name of the subclass, not this base class
    // Create timer without parent to avoid Qt threading violations
    processTimer = new QTimer(nullptr);

    if (initialize()) {
        // Initialization was successful. Start the main loop
        signalManager->emitSignal("listenerConnected", metaObject()->className());
        threadActive = true;

        connect(processTimer, SIGNAL(timeout()), this, SLOT(process()));
        processTimer->start(processTimeout); // Process every 100 ms
        exec();

        //while (threadActive) {
        //    process();
        //}
    } else {
        signalManager->emitSignal("listenerDisconnected", metaObject()->className());
    }

    finalize();
    
    // Clean up timer manually since it doesn't have a parent
    if (processTimer) {
        processTimer->stop();
        delete processTimer;
        processTimer = nullptr;
    }
    
    // Ensure event loop stops completely
    quit();
    
    if (signalManager) {
        signalManager->emitSignal("listenerDisconnected", metaObject()->className());
    }
}

bool ListenerBase::initialize() {
    // Return true if success
    return true;
}

void ListenerBase::process() {
    // This method is implemented in a child class and called from run()
}

void ListenerBase::finalize() {
    if (signalManager) {
        signalManager->emitSignal("listenerTerminated", metaObject()->className());
        qDebug() << "disconnecting slots......";
        disconnectSlots();
    }
}

} // namespace mrigtlbridge
