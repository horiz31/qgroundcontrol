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


class Annotation : public QObject
{
    Q_OBJECT

public:

    typedef struct {
        QString         uid;    // Required
        QString         displayName;
        QString         type;
        QGeoCoordinate  location;
        double          altitude;
        double          radius;
        QColor          color;
    } AnnotationInfo_t;

    Annotation(const AnnotationInfo_t& AnnotationInfo, QObject* parent);

    Q_PROPERTY(QString          uid         READ uid            CONSTANT)
    Q_PROPERTY(QString          displayName    READ displayName       NOTIFY displayNameChanged)
    Q_PROPERTY(QString          type        READ type           NOTIFY typeChanged)
    Q_PROPERTY(QGeoCoordinate   coordinate  READ coordinate     NOTIFY coordinateChanged)
    Q_PROPERTY(double           altitude    READ altitude       NOTIFY altitudeChanged)     // NaN for not available
    Q_PROPERTY(double           radius      READ radius         NOTIFY radiusChanged)     // NaN for not available

    Q_PROPERTY(QColor           color        READ color        NOTIFY colorChanged)      // NaN for not available

    QString         uid         (void) const { return static_cast<QString>(_uid); }
    QString         displayName    (void) const { return _displayName; }
    QString         type        (void) const { return _type; }
    QGeoCoordinate  coordinate  (void) const { return _coordinate; }
    double          altitude    (void) const { return _altitude; }
    double          radius    (void) const { return _radius; }
    QColor          color       (void) const { return _color; }

    QString         _uid;
    QString         _displayName;

    void update(const AnnotationInfo_t& AnnotationInfo);

    /// check if the marker is expired and should be removed
    bool expired();

signals:
    void coordinateChanged  ();
    void displayNameChanged    ();
    void altitudeChanged    ();   
    void radiusChanged    ();
    void colorChanged     ();
    void typeChanged        ();

private:


    QString         _type;
    QGeoCoordinate  _coordinate;
    double          _altitude;
    double          _radius;
    QColor          _color;

};

Q_DECLARE_METATYPE(Annotation::AnnotationInfo_t)


