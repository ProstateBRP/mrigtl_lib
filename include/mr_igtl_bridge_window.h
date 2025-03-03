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