/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCToolbox.h"
#include "QmlObjectListModel.h"
#include "ATAKMarker.h"

#include <QThread>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QGeoCoordinate>

class ATAKMarkerManagerSettings;

class ATAKUDPLink : public QThread
{
    Q_OBJECT

public:
    ATAKUDPLink(const QString& hostAddress, int port, QObject* parent);
    ~ATAKUDPLink();

signals:
    void atakMarkerUpdate(const ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo);
    void error(const QString errorMsg);

protected:
    void run(void) final;

private slots:
    void _readBytes(void);

private:
    void _hardwareConnect(void);
    void _parseData(const QString& line);

    QString         _hostAddress;
    int             _port;
    QUdpSocket*     _socket =   nullptr;
};

class ATAKMarkerManager : public QGCTool {
    Q_OBJECT
    
public:
    ATAKMarkerManager(QGCApplication* app, QGCToolbox* toolbox);

    Q_PROPERTY(QmlObjectListModel* atakMarkers READ atakMarkers CONSTANT)

    QmlObjectListModel* atakMarkers(void) { return &_ATAKMarkers; }

    // QGCTool overrides
    void setToolbox(QGCToolbox* toolbox) final;

public slots:
    void atakMarkerUpdate  (const ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo);
    void _udpError          (const QString errorMsg);

private slots:
    void _cleanupStaleMarkers(void);

private:
    QmlObjectListModel              _ATAKMarkers;
    QMap<QString, ATAKMarker*>      _atakUidMap;
    QTimer                          _ATAKMarkerCleanupTimer;
    ATAKUDPLink*                    _udpLink = nullptr;
};
