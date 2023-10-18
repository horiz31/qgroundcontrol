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
#include <QString>
#include <QtNetwork>
#include <QGeoCoordinate>
#include "FactPanelController.h"


class AnnotationController : public FactPanelController
{
    Q_OBJECT
    
public:
    AnnotationController();

    Q_PROPERTY(int              color           READ color              WRITE setColor            NOTIFY colorChanged)
    Q_PROPERTY(int              type            READ type               WRITE setType           NOTIFY typeChanged)
    Q_PROPERTY(QString          uid             READ uid                WRITE setUid              NOTIFY uidChanged)
    Q_PROPERTY(QStringList      colorList       READ colorList                                CONSTANT)
    Q_PROPERTY(QStringList      typeList        READ typeList                                CONSTANT)
    Q_PROPERTY(double           altitude        READ altitude           WRITE setAltitude         NOTIFY altitudeChanged)
    Q_PROPERTY(double           radius          READ radius             WRITE setRadius        NOTIFY radiusChanged)


    Q_INVOKABLE void create(QGeoCoordinate, QString, QString, QString);

    QStringList     colorList (){ return _colorList; }
    QStringList     typeList (){ return _typeList; }

    double     altitude (){ return _altitude; }
    double     radius (){ return _radius; }

    void        setColor          (int color);
    void        setType           (int type);
    void        setUid            (QString uid);
    void        setAltitude       (double altitude);
    void        setRadius         (double radius);
    int         type              ();
    int         color             ();
    QString     uid               ();
    QString     GetRandomString ();

signals:
    void        colorChanged        ();
    void        typeChanged        ();
    void        altitudeChanged     ();
    void        radiusChanged     ();
    void        uidChanged        ();

private slots:


private:
    QList<QPair<QString, QColor>> _colorMap;
    QList<QPair<QString, QString>> _typeMap;
    QStringList _colorList;
    QStringList _typeList;
    QString _uid;
    double _altitude;
    double _radius;
    int _colorIndex = 0;
    int _typeIndex = 0;
    QGCToolbox*         _toolbox = nullptr;

signals:
private:
};
