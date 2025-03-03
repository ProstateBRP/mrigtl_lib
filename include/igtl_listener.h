#pragma once

#include "listener_base.h"
#include <QMutex>
#include <QVector>
#include <QString>
#include <igtlClientSocket.h>
#include <igtlTransformMessage.h>
#include <igtlImageMessage.h>
#include <igtlStringMessage.h>
#include <igtlMessageBase.h>
#include <array>
#include <vector>
#include <cstdint>

namespace mrigtlbridge {

class IGTLListener : public ListenerBase {
    Q_OBJECT

public:
    explicit IGTLListener(QObject* parent = nullptr);
    ~IGTLListener() override;

    void connectSlots(SignalManager* signalManager) override;
    void disconnectSlots() override;

signals:
    void closeSocketSignal();
    void transformReceivedSignal(const QVariantMap& matrix, const QVariantMap& param);
    void startSequenceSignal();
    void stopSequenceSignal();

private slots:
    void disconnectOpenIGTEvent();
    void sendImageIGTL(const QVariantMap& param);
    void sendTrackingDataIGTL(const QVariantMap& param);

protected:
    bool initialize() override;
    void process() override;
    void finalize() override;

private:
    bool connect(const QString& ip, int port);
    int onReceiveTransform(igtl::TransformMessage::Pointer transMsg);
    void onReceiveString(igtl::StringMessage::Pointer stringMsg);

    igtl::ClientSocket::Pointer clientServer;
    
    QVector<QByteArray> imageQueue;
    QVector<double> imgIntvQueue;
    int imgIntvQueueIndex;
    double imgIntv;     // Frequency of incoming images (second)
    double prevImgTime; // Previous image arrival time (to estimate imgIntv) (second)
    
    QString state; // Either 'INIT', 'IDLE', or 'SCAN'

    // For tracking message throttling
    double prevTransMsgTime;
    double minTransMsgInterval;
    bool pendingTransMsg;
    igtl::TransformMessage::Pointer transMsg;
};

} // namespace mrigtlbridge