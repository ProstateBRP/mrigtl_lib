/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#pragma once

#include <QWidget>
#include <QString>
#include <memory>

namespace mrigtlbridge {

class WidgetBase;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setTitle(const QString& title);
    void setLeftWidget(WidgetBase* widget);
    void setRightWidget(WidgetBase* widget);
    void setup();

private:
    WidgetBase* leftWidget;
    WidgetBase* rightWidget;
    QString title;
};

} // namespace mrigtlbridge