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
#include "widget_base.h"
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <memory>

namespace mrigtlbridge {

class IGTLWidget : public WidgetBase {
    Q_OBJECT

public:
    MRIGTL_LIB_EXPORT explicit IGTLWidget(QObject* parent = nullptr);
    MRIGTL_LIB_EXPORT ~IGTLWidget() override;

    MRIGTL_LIB_EXPORT void buildGUI(QWidget* parent) override;
    MRIGTL_LIB_EXPORT void updateGUI(const QString& state) override;
    MRIGTL_LIB_EXPORT void setSignalManager(SignalManager* sm) override;

private slots:
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onConsoleTextReceived(const QString& text);

private:
    // GUI elements
    QLineEdit* openIGT_IpEdit;
    QLineEdit* openIGT_PortEdit;
    QPushButton* openIGTConnectButton;
    QPushButton* openIGTDisconnectButton;
    QTextEdit* openIGTConsole;
    QLabel* openIGTStatus;
};

} // namespace mrigtlbridge