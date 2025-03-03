/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mrsim_listener.h"
#include "signal_manager.h"
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <QMutexLocker>

namespace mrigtlbridge {

MRSimListener::MRSimListener(QObject* parent)
    : ListenerBase(parent),
      running(false) {
    
    // Initialize scan planes
    scanPlanes.resize(3);
}

MRSimListener::~MRSimListener() {
}

void MRSimListener::connectSlots(SignalManager* sm) {
    ListenerBase::connectSlots(sm);
    sm->connectSlot("startSequence", this, SLOT(onStartSequence()));
    sm->connectSlot("stopSequence", this, SLOT(onStopSequence()));
    sm->connectSlot("updateScanPlane", this, SLOT(onUpdateScanPlane(QVariantMap)));
}

void MRSimListener::disconnectSlots() {
    ListenerBase::disconnectSlots();
    if (signalManager) {
        signalManager->disconnectSlot("startSequence", this, SLOT(onStartSequence()));
        signalManager->disconnectSlot("stopSequence", this, SLOT(onStopSequence()));
        signalManager->disconnectSlot("updateScanPlane", this, SLOT(onUpdateScanPlane(QVariantMap)));
    }
}

bool MRSimListener::initialize() {
    signalManager->emitSignal("consoleTextMR", "Initializing MR Simulator...");
    return true;
}

void MRSimListener::process() {
    // If the simulation is running, generate and send image data
    if (running) {
        QMutexLocker locker(&mutex);
        
        // Send console message
        signalManager->emitSignal("consoleTextMR", "Simulating MR acquisition...");
        
        // Delay to simulate MR acquisition
        QThread::msleep(500);
    } else {
        // Sleep to avoid busy waiting
        QThread::msleep(100);
    }
}

void MRSimListener::finalize() {
    ListenerBase::finalize();
}

void MRSimListener::onStartSequence() {
    QMutexLocker locker(&mutex);
    running = true;
    signalManager->emitSignal("consoleTextMR", "Sequence started");
}

void MRSimListener::onStopSequence() {
    QMutexLocker locker(&mutex);
    running = false;
    signalManager->emitSignal("consoleTextMR", "Sequence stopped");
    
    // If this was triggered by IGTL disconnect, we should also show it
    signalManager->emitSignal("consoleTextMR", "IGTL connection closed");
}

void MRSimListener::onUpdateScanPlane(const QVariantMap& param) {
    QMutexLocker locker(&mutex);
    
    int planeId = param["plane_id"].toInt();
    if (planeId >= 0 && planeId < scanPlanes.size()) {
        scanPlanes[planeId] = param;
        signalManager->emitSignal("consoleTextMR", QString("Scan plane %1 updated").arg(planeId));
    }
}

} // namespace mrigtlbridge