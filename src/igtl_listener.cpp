#include "igtl_listener.h"
#include "signal_manager.h"
#include "common.h"
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <ctime>
#include <igtlTrackingDataMessage.h>

namespace mrigtlbridge {

IGTLListener::IGTLListener(QObject* parent)
    : ListenerBase(parent),
      imgIntvQueueIndex(0),
      imgIntv(1.0),
      prevImgTime(0.0),
      state("INIT"),
      prevTransMsgTime(0.0),
      minTransMsgInterval(0.1),
      pendingTransMsg(false) {
    
    // Initialize parameters
    parameter["ip"] = "localhost";
    parameter["port"] = "18944";
    parameter["sendTimestamp"] = 1;
    
    // Initialize image interval queue
    imgIntvQueue.resize(5);
    imgIntvQueue.fill(0.0);
}

IGTLListener::~IGTLListener() {
}

void IGTLListener::connectSlots(SignalManager* sm) {
    ListenerBase::connectSlots(sm);
    sm->connectSlot("disconnectIGTL", this, SLOT(disconnectOpenIGTEvent()));
    sm->connectSlot("sendImageIGTL", this, SLOT(sendImageIGTL(QVariantMap)));
    sm->connectSlot("sendTrackingDataIGTL", this, SLOT(sendTrackingDataIGTL(QVariantMap)));
}

void IGTLListener::disconnectSlots() {
    ListenerBase::disconnectSlots();
    if (signalManager) {
        signalManager->disconnectSlot("disconnectIGTL", this, SLOT(disconnectOpenIGTEvent()));
        signalManager->disconnectSlot("sendImageIGTL", this, SLOT(sendImageIGTL(QVariantMap)));
        signalManager->disconnectSlot("sendTrackingDataIGTL", this, SLOT(sendTrackingDataIGTL(QVariantMap)));
    }
}

bool IGTLListener::initialize() {
    signalManager->emitSignal("consoleTextIGTL", "Initializing IGTL Listener...");
    
    // Reset timing variables
    prevImgTime = 0.0;
    prevTransMsgTime = 0.0;
    minTransMsgInterval = 0.1; // 10 Hz
    pendingTransMsg = false;

    QString socketIP = parameter["ip"].toString();
    int socketPort = parameter["port"].toString().toInt();

    return connect(socketIP, socketPort);
}

void IGTLListener::process() {
    minTransMsgInterval = 0.1; // 100ms

    // Initialize receive buffer
    igtl::MessageBase::Pointer headerMsg = igtl::MessageBase::New();
    headerMsg->InitPack();

    clientServer->SetReceiveTimeout(10); // Milliseconds
    bool timeout = true;
    
    // Call Receive and get the result (don't use std::tie)
    int result = clientServer->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
    
    double msgTime = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    
    if (result == 0 && timeout) {
        // Time out
        if (pendingTransMsg) {
            if (msgTime - prevTransMsgTime > minTransMsgInterval) {
                signalManager->emitSignal("consoleTextIGTL", "Sending out pending transform.");
                transMsg->Unpack();
                onReceiveTransform(transMsg);
                prevTransMsgTime = msgTime;
                pendingTransMsg = false;
            }
        }
        return;
    }
    
    if (result != headerMsg->GetPackSize()) {
        signalManager->emitSignal("consoleTextIGTL", "Incorrect pack size!");
        return;
    }
    
    // Deserialize the header
    headerMsg->Unpack();

    // Check data type and respond accordingly
    std::string msgType = headerMsg->GetDeviceType();
    if (!msgType.empty()) {
        signalManager->emitSignal("consoleTextIGTL", QString("Received: %1").arg(msgType.c_str()));
    }
    
    // ---------------------- TRANSFORM ----------------------------
    if (msgType == "TRANSFORM") {
        transMsg = igtl::TransformMessage::New();
        transMsg->Copy(headerMsg); // Copy header 
        transMsg->AllocatePack();

        // Receive transform data from the socket
        timeout = false;
        result = clientServer->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize(), timeout);

        // Check the time interval. Send the transform to MRI only if there was enough interval.
        if (msgTime - prevTransMsgTime > minTransMsgInterval) {
            transMsg->Unpack();
            onReceiveTransform(transMsg);
            prevTransMsgTime = msgTime;
            pendingTransMsg = false;
        } else {
            pendingTransMsg = true;
        }
    }
    // ---------------------- STRING ----------------------------
    else if (msgType == "STRING") {
        // Create a message buffer to receive string data
        igtl::StringMessage::Pointer stringMsg = igtl::StringMessage::New();
        stringMsg->Copy(headerMsg); // Copy header
        stringMsg->AllocatePack();

        // Receive string data from the socket
        timeout = false;
        result = clientServer->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize(), timeout);
        stringMsg->Unpack();
        
        onReceiveString(stringMsg);
    }
    else if (msgType == "POINT") {
        // Handle POINT messages if needed
    }

    double endTime = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    double sleepTime = minTransMsgInterval - (endTime - msgTime);
    qDebug() << "sleep time =" << sleepTime;
    if (sleepTime < 0) {
        sleepTime = 0;
    }

    QThread::msleep(static_cast<unsigned long>(1000.0 * sleepTime));
}

void IGTLListener::finalize() {
    // Send explicit disconnection message to the server if not already done
    if (clientServer && clientServer->GetConnected()) {
        try {
            // Create a string message to indicate disconnection
            igtl::StringMessage::Pointer disconnectMsg = igtl::StringMessage::New();
            disconnectMsg->SetDeviceName("DISCONNECT");
            disconnectMsg->SetString("CLIENT_FINALIZING");
            disconnectMsg->Pack();
            
            // Send the message
            clientServer->Send(disconnectMsg->GetPackPointer(), disconnectMsg->GetPackSize());
            
            signalManager->emitSignal("consoleTextIGTL", "Sent finalization notification to server");
            
            // Close the socket explicitly
            clientServer->CloseSocket();
        } catch (const std::exception& e) {
            signalManager->emitSignal("consoleTextIGTL", QString("Error sending finalize message: %1").arg(e.what()));
        }
    }
    
    // Call parent class finalize
    ListenerBase::finalize();
}

bool IGTLListener::connect(const QString& ip, int port) {
    clientServer = igtl::ClientSocket::New();
    clientServer->SetReceiveTimeout(1); // Milliseconds
    
    int ret = clientServer->ConnectToServer(ip.toStdString().c_str(), port);
    if (ret == 0) {
        signalManager->emitSignal("consoleTextIGTL", "Connection successful");
        return true;
    } else {
        signalManager->emitSignal("consoleTextIGTL", "Connection failed");
        return false;
    }
}

int IGTLListener::onReceiveTransform(igtl::TransformMessage::Pointer transMsg) {
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    
    QVariantMap param;
    std::string deviceName = transMsg->GetDeviceName();
    if (deviceName == "PLANE_0" || deviceName == "PLANE") {
        param["plane_id"] = 0;
    } else if (deviceName == "PLANE_1") {
        param["plane_id"] = 1;
    } else if (deviceName == "PLANE_2") {
        param["plane_id"] = 2;
    }
    
    // Convert matrix to QVariant for signal emission
    QVariantList matrixList;
    for (int i = 0; i < 4; i++) {
        QVariantList row;
        for (int j = 0; j < 4; j++) {
            row.append(matrix[i][j]);
        }
        matrixList.append(row);
    }
    param["matrix"] = matrixList;
    
    signalManager->emitSignal("consoleTextIGTL", QString::fromStdString(std::to_string(matrix[0][0]) + " " +
                                                              std::to_string(matrix[0][1]) + " " +
                                                              std::to_string(matrix[0][2]) + " " +
                                                              std::to_string(matrix[0][3])));
    signalManager->emitSignal("updateScanPlane", param);
    
    return 1;
}

void IGTLListener::onReceiveString(igtl::StringMessage::Pointer stringMsg) {
    std::string str = stringMsg->GetString();
    std::string deviceName = stringMsg->GetDeviceName();
    
    if (str == "START_SEQUENCE") {
        state = "SCAN";
        signalManager->emitSignal("startSequence");
    } else if (str == "STOP_SEQUENCE") {
        state = "IDLE";
        signalManager->emitSignal("stopSequence");
    } else if (str == "START_UP") {   // Initialize
        state = "IDLE";
    } else if (deviceName == "DISCONNECT") {
        // Server is notifying us that it's disconnecting
        signalManager->emitSignal("consoleTextIGTL", QString("Server requested disconnect: %1").arg(str.c_str()));
        
        // Initiate a clean shutdown
        signalManager->emitSignal("disconnectIGTL");
    }
}

void IGTLListener::disconnectOpenIGTEvent() {
    // This method is called when disconnectIGTL signal is emitted
    signalManager->emitSignal("consoleTextIGTL", "Received disconnection request");
    
    // Send explicit disconnection message to the server
    if (clientServer && clientServer->GetConnected()) {
        try {
            // Create a string message to indicate disconnection
            igtl::StringMessage::Pointer disconnectMsg = igtl::StringMessage::New();
            disconnectMsg->SetDeviceName("DISCONNECT");
            disconnectMsg->SetString("CLIENT_DISCONNECTING");
            disconnectMsg->Pack();
            
            // Send the message
            clientServer->Send(disconnectMsg->GetPackPointer(), disconnectMsg->GetPackSize());
            
            signalManager->emitSignal("consoleTextIGTL", "Sent disconnection notification to server");
            
            // Close the socket explicitly
            clientServer->CloseSocket();
        } catch (const std::exception& e) {
            signalManager->emitSignal("consoleTextIGTL", QString("Error sending disconnect message: %1").arg(e.what()));
        }
    }
    
    // Stop this listener
    stop();
}

void IGTLListener::sendImageIGTL(const QVariantMap& param) {
    signalManager->emitSignal("consoleTextIGTL", "Sending image...");
    /*
     * 'param' dictionary must contain the following members:
     *
     *  param['dtype']       : Data type in str. See mrigtlbridge/common.h
     *  param['dimension']   : Matrix size in each dimension e.g., [256, 256, 128]
     *  param['spacing']     : Pixel spacing in each dimension e.g., [1.0, 1.0, 5.0]
     *  param['name']        : Name of the image in the string type.
     *  param['numberOfComponents'] : Number of components per voxel.
     *  param['endian']      : Endian used in the binary data. 1: big; 2: little
     *  param['matrix']      : 4x4 transformation matrix to map the pixel to the physical space.
     *  param['attribute']   : Dictionary to pass miscellaneous attributes (OPTIONAL)
     *  param['binary']      : List of binary arrays.
     *  param['binaryOffset']: Offset to each binary array.
     *  param['timestamp']   : Timestamp (OPTIONAL)
     */

    try {
        QString dtype = param["dtype"].toString();
        QVariantList dimensionVar = param["dimension"].toList();
        QVariantList spacingVar = param["spacing"].toList();
        QString name = param["name"].toString();
        int numberOfComponents = param["numberOfComponents"].toInt();
        int endian = param["endian"].toInt();
        QVariantList matrixVar = param["matrix"].toList();
        QVariantList binaryVar = param["binary"].toList();
        QVariantList binaryOffsetVar = param["binaryOffset"].toList();
        
        // Convert variant lists to native types
        std::vector<int> dimension = {
            dimensionVar[0].toInt(),
            dimensionVar[1].toInt(),
            dimensionVar[2].toInt()
        };
        
        std::vector<float> spacing = {
            spacingVar[0].toFloat(),
            spacingVar[1].toFloat(),
            spacingVar[2].toFloat()
        };
        
        igtl::Matrix4x4 matrix;
        for (int i = 0; i < 4; i++) {
            QVariantList row = matrixVar[i].toList();
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = row[j].toFloat();
            }
        }
        
        std::vector<QByteArray> binary;
        for (const QVariant& var : binaryVar) {
            binary.push_back(var.toByteArray());
        }
        
        std::vector<int> binaryOffset;
        for (const QVariant& var : binaryOffsetVar) {
            binaryOffset.push_back(var.toInt());
        }
        
        // Optional parameters
        QVariantMap attribute;
        if (param.contains("attribute")) {
            attribute = param["attribute"].toMap();
        }
        
        QVariant timestamp;
        if (param.contains("timestamp")) {
            timestamp = param["timestamp"];
        }
        
        // Create and send the image message
        igtl::ImageMessage::Pointer imageMsg = igtl::ImageMessage::New();
        imageMsg->SetDimensions(dimension[0], dimension[1], dimension[2]);
        
        if (DataTypeTable.find(dtype.toStdString()) != DataTypeTable.end()) {
            const auto& typeInfo = DataTypeTable[dtype.toStdString()];
            imageMsg->SetScalarType(typeInfo[0]);
        }
        
        imageMsg->SetDeviceName(name.toStdString());
        imageMsg->SetNumComponents(numberOfComponents);
        imageMsg->SetEndian(endian); // little is 2, big is 1
        imageMsg->AllocateScalars();
        
        // Copy the binary data
        for (size_t i = 0; i < binary.size(); i++) {
            void* dest = static_cast<void*>(static_cast<char*>(imageMsg->GetScalarPointer()) + binaryOffset[i]);
            std::memcpy(dest, binary[i].constData(), binary[i].size());
        }
        
        imageMsg->SetSpacing(spacing[0], spacing[1], spacing[2]);
        imageMsg->SetMatrix(matrix);
        imageMsg->Pack();
        
        clientServer->Send(imageMsg->GetPackPointer(), imageMsg->GetPackSize());
        
        // Send a separate timestamp message if needed
        if (parameter["sendTimestamp"].toInt() == 1 && param.contains("timestamp")) {
            igtl::StringMessage::Pointer textMsg = igtl::StringMessage::New();
            textMsg->SetDeviceName("IMAGE_TIMESTAMP");
            textMsg->SetString(timestamp.toString().toStdString());
            textMsg->Pack();
            clientServer->Send(textMsg->GetPackPointer(), textMsg->GetPackSize());
        }
    } catch (const std::exception& e) {
        signalManager->emitSignal("consoleTextIGTL", QString("ERROR: %1").arg(e.what()));
    }
}

void IGTLListener::sendTrackingDataIGTL(const QVariantMap& param) {
    signalManager->emitSignal("consoleTextIGTL", "Sending tracking data...");
    /*
     * 'param' is a map of coil data, which consists of the following fields:
     *
     *  coil['name']         : Coil name
     *  coil['position_pcs'] : Coil position in the patient coordinate system
     *  coil['position_dcs'] : Coil position in the device coordinate system
     */
    
    if (param.isEmpty()) {
        signalManager->emitSignal("consoleTextIGTL", "ERROR: No tracking data.");
        return;
    }
    
    igtl::TrackingDataMessage::Pointer trackingDataMsg = igtl::TrackingDataMessage::New();
    trackingDataMsg->SetDeviceName("MRTracking");
    
    for (auto it = param.constBegin(); it != param.constEnd(); ++it) {
        QVariantMap coil = it.value().toMap();
        QVariantList posVar = coil["position_pcs"].toList();
        
        igtl::TrackingDataElement::Pointer trackElement = igtl::TrackingDataElement::New();
        trackElement->SetName(it.key().toStdString().c_str());
        trackElement->SetType(igtl::TrackingDataElement::TYPE_TRACKER);
        trackElement->SetPosition(posVar[0].toFloat(), posVar[1].toFloat(), posVar[2].toFloat());
        trackingDataMsg->AddTrackingDataElement(trackElement);
    }
    
    trackingDataMsg->Pack();
    clientServer->Send(trackingDataMsg->GetPackPointer(), trackingDataMsg->GetPackSize());
}

} // namespace mrigtlbridge
