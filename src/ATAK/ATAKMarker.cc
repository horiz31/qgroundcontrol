/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ATAKMarker.h"
#include "QGCLoggingCategory.h"
#include "QGC.h"

#include <QDebug>
#include <QtMath>

ATAKMarker::ATAKMarker(const ATAKMarkerInfo_t& atakMarkerInfo, QObject* parent)
    : QObject       (parent)
    , _uid  (atakMarkerInfo.uid)
    , _callsign     (atakMarkerInfo.callsign)
    , _altitude     (qQNaN())
    , _heading      (qQNaN())
    , _isLocal      (atakMarkerInfo.isLocal)
{
    update(atakMarkerInfo);
}

void ATAKMarker::update(const ATAKMarkerInfo_t& atakMarkerInfo)
{
    if (_uid != atakMarkerInfo.uid) {
        qCWarning(ATAKMarkerManagerLog) << "uid mismatch expected:actual" << _uid << atakMarkerInfo.uid;
        return;
    }
    if (atakMarkerInfo.callsign != _callsign) {
        _callsign = atakMarkerInfo.callsign;
        emit callsignChanged();
    }

    if (_coordinate != atakMarkerInfo.location) {
        _coordinate = atakMarkerInfo.location;
        emit coordinateChanged();
    }
    if (!QGC::fuzzyCompare(atakMarkerInfo.altitude, _altitude)) {
        _altitude = atakMarkerInfo.altitude;
        emit altitudeChanged();
    }
    if (!QGC::fuzzyCompare(atakMarkerInfo.heading, _heading)) {
        _heading = atakMarkerInfo.heading;
        emit headingChanged();
    }

    _lastUpdateTimer.restart();
}

bool ATAKMarker::expired()
{
    return _lastUpdateTimer.hasExpired(expirationTimeoutMs);
}
