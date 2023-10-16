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
#include "Annotation.h"


class AnnotationController : public FactPanelController
{
    Q_OBJECT
    
public:
    AnnotationController();

    Q_PROPERTY(int              color           READ color              WRITE setColor            NOTIFY colorChanged)
    Q_PROPERTY(QString          uid             READ uid                WRITE setUid              NOTIFY uidChanged)
    Q_PROPERTY(QStringList      colorList       READ colorList                                CONSTANT)
    Q_PROPERTY(double           altitude        READ altitude           WRITE setAltitude         NOTIFY altitudeChanged)

    Q_INVOKABLE void create(QGeoCoordinate, QString, QString);

    QStringList     colorList (){ return _colorList; }

    double     altitude (){ return _altitude; }

    void        setColor          (int color);
    void        setUid            (QString uid);
    void        setAltitude       (double altitude);
    int         color             ();
    QString     uid               ();
    QString     GetRandomString ();

signals:
    void        colorChanged        ();
    void        altitudeChanged     ();
    void        uidChanged        ();

private slots:


private:
    QList<QPair<QString, QColor>> _colorMap;
    QStringList _colorList;
    QString _uid;
    double _altitude;
    int _colorIndex = 0;
    QGCToolbox*         _toolbox = nullptr;

signals:
private:
};
