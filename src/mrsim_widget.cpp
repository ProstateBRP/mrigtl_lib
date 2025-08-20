/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mrsim_widget.h"
#include "mrsim_listener.h"
#include "signal_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>
#include <QDateTime>

namespace mrigtlbridge {

MRSimWidget::MRSimWidget(QObject* parent)
    : WidgetBase(parent),
      mrSimConnectButton(nullptr),
      mrSimDisconnectButton(nullptr),
      startSequenceButton(nullptr),
      stopSequenceButton(nullptr),
      mrSimConsole(nullptr),
      mrSimStatus(nullptr) {
    
    // Set the listener class for this widget
    listener_class << "MRSimListener";
}

MRSimWidget::~MRSimWidget() {
}

void MRSimWidget::buildGUI(QWidget* parent) {
    QVBoxLayout* layout = new QVBoxLayout(parent);
    parent->setLayout(layout);
    
    // Title
    QLabel* titleLabel = new QLabel("MR Simulator", parent);
    QFont font = titleLabel->font();
    font.setBold(true);
    font.setPointSize(14);
    titleLabel->setFont(font);
    layout->addWidget(titleLabel);
    
    // Connection Group
    QGroupBox* connectionGroupBox = new QGroupBox("Connection", parent);
    layout->addWidget(connectionGroupBox);
    
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionGroupBox);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    mrSimConnectButton = new QPushButton("Connect", parent);
    mrSimDisconnectButton = new QPushButton("Disconnect", parent);
    mrSimDisconnectButton->setEnabled(false);
    buttonLayout->addWidget(mrSimConnectButton);
    buttonLayout->addWidget(mrSimDisconnectButton);
    connectionLayout->addLayout(buttonLayout);
    
    // Status
    mrSimStatus = new QLabel("Disconnected", parent);
    connectionLayout->addWidget(mrSimStatus);
    
    // Sequence Control Group
    QGroupBox* sequenceGroupBox = new QGroupBox("Sequence Control", parent);
    layout->addWidget(sequenceGroupBox);
    
    QVBoxLayout* sequenceLayout = new QVBoxLayout(sequenceGroupBox);
    
    QHBoxLayout* sequenceButtonLayout = new QHBoxLayout();
    startSequenceButton = new QPushButton("Start Sequence", parent);
    stopSequenceButton = new QPushButton("Stop Sequence", parent);
    startSequenceButton->setEnabled(false);
    stopSequenceButton->setEnabled(false);
    sequenceButtonLayout->addWidget(startSequenceButton);
    sequenceButtonLayout->addWidget(stopSequenceButton);
    sequenceLayout->addLayout(sequenceButtonLayout);
    
    // Console
    QGroupBox* consoleGroupBox = new QGroupBox("Console", parent);
    layout->addWidget(consoleGroupBox);
    
    QVBoxLayout* consoleLayout = new QVBoxLayout(consoleGroupBox);
    mrSimConsole = new QTextEdit(parent);
    mrSimConsole->setReadOnly(true);
    consoleLayout->addWidget(mrSimConsole);
    
    // Connect signals and slots
    connect(mrSimConnectButton, &QPushButton::clicked, this, &MRSimWidget::onConnectButtonClicked);
    connect(mrSimDisconnectButton, &QPushButton::clicked, this, &MRSimWidget::onDisconnectButtonClicked);
    connect(startSequenceButton, &QPushButton::clicked, this, &MRSimWidget::onStartSequenceClicked);
    connect(stopSequenceButton, &QPushButton::clicked, this, &MRSimWidget::onStopSequenceClicked);
}

void MRSimWidget::updateGUI(const QString& state) {
    if (state == "listenerConnected") {
        mrSimConnectButton->setEnabled(false);
        mrSimDisconnectButton->setEnabled(true);
        startSequenceButton->setEnabled(true);
        stopSequenceButton->setEnabled(false);
        mrSimStatus->setText("Connected");
    } else if (state == "listenerDisconnected") {
        mrSimConnectButton->setEnabled(true);
        mrSimDisconnectButton->setEnabled(false);
        startSequenceButton->setEnabled(false);
        stopSequenceButton->setEnabled(false);
        mrSimStatus->setText("Disconnected");
    }
}

void MRSimWidget::setSignalManager(SignalManager* sm) {
    WidgetBase::setSignalManager(sm);
    sm->connectSlot("consoleTextMR", this, SLOT(onConsoleTextReceived(QString)));
    
    // Connect to handle disconnection initiated from IGTL side
    sm->connectSlot("stopSequence", this, SLOT(onStopSequenceClicked()));
}

void MRSimWidget::onConnectButtonClicked() {
    // Create and start the listener
    try {
        if (!listener) {
            listener = new MRSimListener();
            listener->connectSlots(signalManager);
            listener->configure(listenerParameter);
            listener->start();
        }
    } catch (const std::exception& e) {
        qCritical() << "Failed to start MR Simulator: " << e.what();
        if (listener) {
            listener->stop();
            delete listener;
            listener = nullptr;
        }
    }
}

void MRSimWidget::onDisconnectButtonClicked() {
    // Guard against recursive calls
    static bool inProgress = false;
    if (inProgress) return;
    
    inProgress = true;
    
    if (listener) {
        // Emit a signal to notify the IGTL side that we're disconnecting
        signalManager->emitSignal("disconnectIGTL");
        
        // Then stop and clean up our listener
        listener->stop();
        delete listener;
        listener = nullptr;
        updateGUI("listenerDisconnected");
    }
    
    inProgress = false;
}

void MRSimWidget::onStartSequenceClicked() {
    signalManager->emitSignal("startSequence");
    startSequenceButton->setEnabled(false);
    stopSequenceButton->setEnabled(true);
}

void MRSimWidget::onStopSequenceClicked() {
    // Guard against recursive calls
    static bool inProgress = false;
    if (inProgress) return;
    
    inProgress = true;
    signalManager->emitSignal("stopSequence");
    startSequenceButton->setEnabled(true);
    stopSequenceButton->setEnabled(false);
    inProgress = false;
}

void MRSimWidget::onConsoleTextReceived(const QString& text) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    mrSimConsole->append(QString("[%1] %2").arg(timestamp, text));
}

} // namespace mrigtlbridge
