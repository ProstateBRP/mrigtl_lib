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
#include <QDateTime>

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
        
        try {
            // Create a small simple test image first to debug IGTL sending
            QVariantMap imageParam;
            
            // Set basic image parameters for a small test image
            imageParam["dtype"] = "uint16";
            
            // Create small 256x256 image for testing
            QVariantList dimensions;
            dimensions.append(256);
            dimensions.append(256);
            dimensions.append(1);
            imageParam["dimension"] = dimensions;
            
            QVariantList spacings;
            spacings.append(1.0);
            spacings.append(1.0);
            spacings.append(1.0);
            imageParam["spacing"] = spacings;
            
            imageParam["name"] = "TestImage";
            imageParam["numberOfComponents"] = 1;
            int n = 1; // for checking endianness:

            imageParam["endian"] = (*(char *)&n == 1)? 2 : 1; // 1 if little endian

            // Default identity matrix
            QVariantList matrix;
            QVariantList row1, row2, row3, row4;
            row1 << 1.0 << 0.0 << 0.0 << 0.0;
            row2 << 0.0 << 1.0 << 0.0 << 0.0;
            row3 << 0.0 << 0.0 << 1.0 << 0.0;
            row4 << 0.0 << 0.0 << 0.0 << 1.0;
            matrix << row1 << row2 << row3 << row4;
            imageParam["matrix"] = matrix;
            
            // Debug matrix structure
            signalManager->emitSignal("consoleTextMR", QString("Matrix size: %1").arg(matrix.size()));
            
            // Create a simple binary image
            int width = 256;
            int height = 256;
            int size = width * height * 2;
            QByteArray imgData;
            imgData.resize(size);
            
            // Fill with a simple pattern (alternating values)
            quint16* pixelData = reinterpret_cast<quint16*>(imgData.data());
            for (int i = 0; i < width * height; i++) {
                pixelData[i] = (i % 2 == 0) ? 1000 : 200;
            }
            
            // Set the binary data
            //QVariantList binary;
            //binary.append(imgData);
            imageParam["binary"] = imgData;

            // Set binary offset
            imageParam["binaryOffset"] = 0;
            
            // Add timestamp
            imageParam["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            
            // Debug output
            signalManager->emitSignal("consoleTextMR", "Sending test image...");
            
            // Send the image via OpenIGTLink
            signalManager->emitSignal("sendImageIGTL", imageParam);
        } 
        catch (const std::exception& e) {
            signalManager->emitSignal("consoleTextMR", QString("Error: %1").arg(e.what()));
        }
        catch (...) {
            signalManager->emitSignal("consoleTextMR", "Unknown error occurred");
        }
        
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
