/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include <QGeoCoordinate>
#include <QElapsedTimer>


class ATAKMarker : public QObject
{
    Q_OBJECT

public:

    typedef struct {
        QString         uid;    // Required
        QString         callsign;
        QString         type;
        QGeoCoordinate  location;
        double          altitude;
        double          heading;
        bool            isLocal;
        qint64          staleTime;
        qint64          startTime;
    } ATAKMarkerInfo_t;

    ATAKMarker(const ATAKMarkerInfo_t& atakMarkerInfo, QObject* parent);

    Q_PROPERTY(QString          uid         READ uid            CONSTANT)
    Q_PROPERTY(QString          callsign    READ callsign       NOTIFY callsignChanged)
    Q_PROPERTY(QString          type        READ type           NOTIFY typeChanged)
    Q_PROPERTY(QGeoCoordinate   coordinate  READ coordinate     NOTIFY coordinateChanged)
    Q_PROPERTY(double           altitude    READ altitude       NOTIFY altitudeChanged)     // NaN for not available
    Q_PROPERTY(double           heading     READ heading        NOTIFY headingChanged)      // NaN for not available
    Q_PROPERTY(bool             isLocal     READ isLocal        CONSTANT)
    Q_PROPERTY(qint64           staleTime   READ staleTime      NOTIFY staleTimeChanged)
    Q_PROPERTY(qint64           startTime   READ startTime      NOTIFY startTimeChanged)

    QString         uid         (void) const { return static_cast<QString>(_uid); }
    QString         callsign    (void) const { return _callsign; }
    QString         type        (void) const { return _type; }
    QGeoCoordinate  coordinate  (void) const { return _coordinate; }
    double          altitude    (void) const { return _altitude; }
    double          heading     (void) const { return _heading; }
    bool            isLocal     (void) const { return (_isLocal); }
    qint64          staleTime   (void) const { return (_staleTime); }
    qint64          startTime   (void) const { return (_startTime); }

    void update(const ATAKMarkerInfo_t& atakMarkerInfo);

    /// check if the marker is expired and should be removed
    bool expired();

signals:
    void coordinateChanged  ();
    void callsignChanged    ();
    void altitudeChanged    ();
    void headingChanged     ();
    void staleTimeChanged   ();
    void startTimeChanged   ();
    void typeChanged        ();

private:
    QString         _uid;
    QString         _callsign;
    QString         _type;
    QGeoCoordinate  _coordinate;
    double          _altitude;
    double          _heading;
    bool            _isLocal;
    qint64          _staleTime;
    qint64          _startTime;

};

Q_DECLARE_METATYPE(ATAKMarker::ATAKMarkerInfo_t)

