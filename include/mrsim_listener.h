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