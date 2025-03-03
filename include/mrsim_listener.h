#pragma once

#include "listener_base.h"
#include <QVariant>
#include <QMutex>
#include <QVector>
#include <QString>

namespace mrigtlbridge {

class MRSimListener : public ListenerBase {
    Q_OBJECT

public:
    explicit MRSimListener(QObject* parent = nullptr);
    ~MRSimListener() override;

    void connectSlots(SignalManager* signalManager) override;
    void disconnectSlots() override;

private slots:
    void onStartSequence();
    void onStopSequence();
    void onUpdateScanPlane(const QVariantMap& param);

protected:
    bool initialize() override;
    void process() override;
    void finalize() override;

private:
    bool running;
    QMutex mutex;
    
    // Scan plane parameters
    QVector<QVariantMap> scanPlanes;
};

} // namespace mrigtlbridge