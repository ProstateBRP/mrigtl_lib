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
#include <QWidget>
#include <QString>
#include <memory>

namespace mrigtlbridge {

class WidgetBase;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MRIGTL_LIB_EXPORT explicit MainWindow(QWidget* parent = nullptr);
    MRIGTL_LIB_EXPORT ~MainWindow();

    MRIGTL_LIB_EXPORT void setTitle(const QString& title);
    MRIGTL_LIB_EXPORT void setLeftWidget(WidgetBase* widget);
    MRIGTL_LIB_EXPORT void setRightWidget(WidgetBase* widget);
    MRIGTL_LIB_EXPORT void setup();

private:
    WidgetBase* leftWidget;
    WidgetBase* rightWidget;
    QString title;
};

} // namespace mrigtlbridge