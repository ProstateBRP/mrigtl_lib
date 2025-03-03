#pragma once

#include <QThread>
#include <QMap>
#include <QString>
#include <QVariant>
#include <memory>
#include <atomic>

namespace mrigtlbridge {

class SignalManager;

class ListenerBase : public QThread {
    Q_OBJECT

public:
    explicit ListenerBase(QObject* parent = nullptr);
    virtual ~ListenerBase();

    // Configure listener parameters
    virtual void configure(const QVariantMap& params);

    // Connect listener to signal manager
    virtual void connectSlots(SignalManager* signalManager);

    // Disconnect listener from signal manager
    virtual void disconnectSlots();

    // Stop the listener thread
    virtual void stop();

    // Map of custom signals that will be registered with signal manager
    QMap<QString, QString> customSignalList;

protected:
    // Main thread function (override from QThread)
    void run() override;

    // Initialize the listener (to be implemented by subclasses)
    virtual bool initialize();

    // Main processing function (to be implemented by subclasses)
    virtual void process();

    // Finalize when thread stops (to be implemented by subclasses)
    virtual void finalize();

    std::atomic<bool> threadActive;
    SignalManager* signalManager;
    QVariantMap parameter;
};

} // namespace mrigtlbridge