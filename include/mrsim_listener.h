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
#include "listener_base.h"
#include <QVariant>
#include <QMutex>
#include <QVector>
#include <QMap>
#include <QString>

namespace mrigtlbridge {

class MRSimListener : public ListenerBase {
    Q_OBJECT

public:
    MRIGTL_LIB_EXPORT explicit MRSimListener(QObject* parent = nullptr);
    MRIGTL_LIB_EXPORT ~MRSimListener() override;

    MRIGTL_LIB_EXPORT void connectSlots(SignalManager* signalManager) override;
    MRIGTL_LIB_EXPORT void disconnectSlots() override;

private slots:
    void onStartSequence();
    void onStopSequence();
    void onUpdateScanPlane(const QVariantMap& param);
    void onSetTrackingEnabled(const QString& enabled);
    void onSetTrackingChannels(const QString& channels);
    void onSetTrackingSource(const QString& source);
    void onSetTrackingFile(const QString& filePath);

protected slots:
    void process() override;

protected:
    MRIGTL_LIB_EXPORT bool initialize() override;
    MRIGTL_LIB_EXPORT void finalize() override;

private:
    // One (X, Y, Z) sample read from a tracking file.
    struct TrackingSample {
        double x;
        double y;
        double z;
    };

    // Load a tracking CSV file (as produced by script/log_to_tracking.py)
    // with columns: channel, timestamp, X, Y, Z. Returns true if at least
    // one sample was loaded successfully.
    bool loadTrackingFile(const QString& filePath);

    bool running;
    bool trackingEnabled;
    int trackingChannels;
    QMutex mutex;

    // Tracking data source: "random" (default) or "file"
    QString trackingSource;
    QString trackingFilePath;
    bool trackingFileLoaded;
    QMap<QString, QVector<TrackingSample>> trackingFileSamples;
    QMap<QString, int> trackingFileIndex;

    // Scan plane parameters
    QVector<QVariantMap> scanPlanes;
};

} // namespace mrigtlbridge
