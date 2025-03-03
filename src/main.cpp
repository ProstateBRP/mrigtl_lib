/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <QApplication>
#include <QLoggingCategory>
#include <memory>

// Forward declarations to make MOC happy
QT_FORWARD_DECLARE_CLASS(QApplication)
QT_FORWARD_DECLARE_CLASS(QWidget)

#include "signal_manager.h"
#include "igtl_widget.h"
#include "mrsim_widget.h"
#include "mr_igtl_bridge_window.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    // Set up logging
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    
    // Create signal manager
    auto signalManager = std::make_shared<mrigtlbridge::SignalManager>();
    
    // Create widgets
    auto igtlWidget = std::make_shared<mrigtlbridge::IGTLWidget>();
    igtlWidget->setSignalManager(signalManager.get());
    
    auto mrSimWidget = std::make_shared<mrigtlbridge::MRSimWidget>();
    mrSimWidget->setSignalManager(signalManager.get());
    
    // Create main window
    mrigtlbridge::MainWindow mainWindow;
    mainWindow.setTitle("MR-OpenIGTLink Bridge");
    mainWindow.setLeftWidget(igtlWidget.get());
    mainWindow.setRightWidget(mrSimWidget.get());
    mainWindow.setup();
    mainWindow.resize(1000, 800);
    mainWindow.show();
    
    return app.exec();
}