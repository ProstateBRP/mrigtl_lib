#pragma once

#include "widget_base.h"
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <memory>

namespace mrigtlbridge {

class MRSimWidget : public WidgetBase {
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