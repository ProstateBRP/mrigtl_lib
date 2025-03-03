/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mr_igtl_bridge_window.h"
#include "widget_base.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QThread>
#include <QApplication>

namespace mrigtlbridge {

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent),
      leftWidget(nullptr),
      rightWidget(nullptr),
      title("MRI OpenIGTLink Bridge") {
}

MainWindow::~MainWindow() {
    // Make sure listener threads are stopped
    if (leftWidget) {
        if (QThread::currentThread() != QApplication::instance()->thread()) {
            // We're in a different thread, move back to main thread to avoid issues
            QMetaObject::invokeMethod(leftWidget, "stopListener", Qt::BlockingQueuedConnection);
        } else {
            leftWidget->stopListener();
        }
    }
    
    if (rightWidget) {
        if (QThread::currentThread() != QApplication::instance()->thread()) {
            // We're in a different thread, move back to main thread to avoid issues
            QMetaObject::invokeMethod(rightWidget, "stopListener", Qt::BlockingQueuedConnection);
        } else {
            rightWidget->stopListener();
        }
    }
}

void MainWindow::setTitle(const QString& windowTitle) {
    title = windowTitle;
}

void MainWindow::setLeftWidget(WidgetBase* widget) {
    leftWidget = widget;
}

void MainWindow::setRightWidget(WidgetBase* widget) {
    rightWidget = widget;
}

void MainWindow::setup() {
    setWindowTitle(title);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    setLayout(topLayout);

    // Left Layout (OpenIGTLink)
    QWidget* leftContainer = new QWidget(this);
    topLayout->addWidget(leftContainer);
    if (leftWidget) {
        leftWidget->buildGUI(leftContainer);
    }

    // Separator
    QFrame* vline = new QFrame(this);
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    topLayout->addWidget(vline);

    // Right Layout (Scanner)
    QWidget* rightContainer = new QWidget(this);
    topLayout->addWidget(rightContainer);
    if (rightWidget) {
        rightWidget->buildGUI(rightContainer);
    }
}

} // namespace mrigtlbridge