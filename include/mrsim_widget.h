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

class MRIGTL_LIB_EXPORT MRSimWidget : public WidgetBase {
    Q_OBJECT

public:
    explicit MRSimWidget(QObject* parent = nullptr);
    ~MRSimWidget() override;

    void buildGUI(QWidget* parent) override;
    void updateGUI(const QString& state) override;
    void setSignalManager(SignalManager* sm) override;

private slots:
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onStartSequenceClicked();
    void onStopSequenceClicked();
    void onConsoleTextReceived(const QString& text);

private:
    // GUI elements
    QPushButton* mrSimConnectButton;
    QPushButton* mrSimDisconnectButton;
    QPushButton* startSequenceButton;
    QPushButton* stopSequenceButton;
    QTextEdit* mrSimConsole;
    QLabel* mrSimStatus;
};

} // namespace mrigtlbridge