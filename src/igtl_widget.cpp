/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtl_widget.h"
#include "igtl_listener.h"
#include "signal_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>
#include <QDateTime>
#include <QThread>

namespace mrigtlbridge {

IGTLWidget::IGTLWidget(QObject* parent)
    : WidgetBase(parent),
      openIGT_IpEdit(nullptr),
      openIGT_PortEdit(nullptr),
      openIGTConnectButton(nullptr),
      openIGTDisconnectButton(nullptr),
      openIGTConsole(nullptr),
      openIGTStatus(nullptr) {
    
    // Set the listener class for this widget
    listener_class << "IGTLListener";
}

IGTLWidget::~IGTLWidget() {
}

void IGTLWidget::buildGUI(QWidget* parent) {
    QVBoxLayout* layout = new QVBoxLayout(parent);
    parent->setLayout(layout);
    
    // Title
    QLabel* titleLabel = new QLabel("OpenIGTLink", parent);
    QFont font = titleLabel->font();
    font.setBold(true);
    font.setPointSize(14);
    titleLabel->setFont(font);
    layout->addWidget(titleLabel);
    
    // Socket Connection Group
    QGroupBox* socketGroupBox = new QGroupBox("Socket Connection", parent);
    layout->addWidget(socketGroupBox);
    
    QVBoxLayout* socketLayout = new QVBoxLayout(socketGroupBox);
    
    QHBoxLayout* ipLayout = new QHBoxLayout();
    QLabel* ipLabel = new QLabel("IP:", parent);
    openIGT_IpEdit = new QLineEdit("localhost", parent);
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(openIGT_IpEdit);
    socketLayout->addLayout(ipLayout);
    
    QHBoxLayout* portLayout = new QHBoxLayout();
    QLabel* portLabel = new QLabel("Port:", parent);
    openIGT_PortEdit = new QLineEdit("18944", parent);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(openIGT_PortEdit);
    socketLayout->addLayout(portLayout);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    openIGTConnectButton = new QPushButton("Connect", parent);
    openIGTDisconnectButton = new QPushButton("Disconnect", parent);
    openIGTDisconnectButton->setEnabled(false);
    buttonLayout->addWidget(openIGTConnectButton);
    buttonLayout->addWidget(openIGTDisconnectButton);
    socketLayout->addLayout(buttonLayout);
    
    // Status
    openIGTStatus = new QLabel("Disconnected", parent);
    socketLayout->addWidget(openIGTStatus);
    
    // Console
    QGroupBox* consoleGroupBox = new QGroupBox("Console", parent);
    layout->addWidget(consoleGroupBox);
    
    QVBoxLayout* consoleLayout = new QVBoxLayout(consoleGroupBox);
    openIGTConsole = new QTextEdit(parent);
    openIGTConsole->setReadOnly(true);
    consoleLayout->addWidget(openIGTConsole);
    
    // Connect signals and slots
    connect(openIGTConnectButton, &QPushButton::clicked, this, &IGTLWidget::onConnectButtonClicked);
    connect(openIGTDisconnectButton, &QPushButton::clicked, this, &IGTLWidget::onDisconnectButtonClicked);
}

void IGTLWidget::updateGUI(const QString& state) {
    if (state == "listenerConnected") {
        openIGTConnectButton->setEnabled(false);
        openIGTDisconnectButton->setEnabled(true);
        openIGT_IpEdit->setEnabled(false);
        openIGT_PortEdit->setEnabled(false);
        openIGTStatus->setText("Connected");
    } else if (state == "listenerDisconnected") {
        openIGTConnectButton->setEnabled(true);
        openIGTDisconnectButton->setEnabled(false);
        openIGT_IpEdit->setEnabled(true);
        openIGT_PortEdit->setEnabled(true);
        openIGTStatus->setText("Disconnected");
    }
}

void IGTLWidget::onConnectButtonClicked() {
    listenerParameter["ip"] = openIGT_IpEdit->text();
    listenerParameter["port"] = openIGT_PortEdit->text();
    
    // Create and start the listener
    try {
        if (!listener) {
            listener = new IGTLListener();
            listener->connectSlots(signalManager);
            listener->configure(listenerParameter);
            listener->start();
        }
    } catch (const std::exception& e) {
        qCritical() << "Failed to start IGTL Listener: " << e.what();
        if (listener) {
            listener->stop();
            delete listener;
            listener = nullptr;
        }
    }
}

void IGTLWidget::onDisconnectButtonClicked() {
    // Guard against recursive calls
    static bool inProgress = false;
    if (inProgress) return;
    
    inProgress = true;
    
    if (listener) {
        // First, emit signal to notify the MR side we're disconnecting
        signalManager->emitSignal("stopSequence");
        
        // Next, explicitly tell the IGTL server we're disconnecting
        signalManager->emitSignal("disconnectIGTL");
        
        // Add a small delay to allow disconnection message to be sent
        QThread::msleep(100);
        
        // Then stop and clean up our listener
        listener->stop();
        delete listener;
        listener = nullptr;
        updateGUI("listenerDisconnected");
    }
    
    inProgress = false;
}

void IGTLWidget::onConsoleTextReceived(const QString& text) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    openIGTConsole->append(QString("[%1] %2").arg(timestamp, text));
}

void IGTLWidget::setSignalManager(SignalManager* sm) {
    WidgetBase::setSignalManager(sm);
    sm->connectSlot("consoleTextIGTL", this, SLOT(onConsoleTextReceived(QString)));
    
    // Connect to handle disconnection from MR side
    sm->connectSlot("disconnectIGTL", this, SLOT(onDisconnectButtonClicked()));
}

} // namespace mrigtlbridge