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


class ATAKController : public FactPanelController
{
    Q_OBJECT
    
public:
    ATAKController();

    Q_PROPERTY(int              cotType         READ cotType            WRITE setCotType            NOTIFY cotTypeChanged)
    Q_PROPERTY(int              staleMinutes    READ staleMinutes       WRITE setStaleMinutes       NOTIFY staleMinutesChanged)
    Q_PROPERTY(QString          uid             READ uid                WRITE setUid                NOTIFY uidChanged)
    Q_PROPERTY(QStringList      cotTypes        READ cotTypes                                       CONSTANT)
    Q_PROPERTY(QStringList      staleMinuteList READ staleMinuteList                                CONSTANT)

    Q_INVOKABLE void send(QGeoCoordinate, QString);

    QStringList     cotTypes      () { return _cotTypes; }
    QStringList     staleMinuteList (){ return _staleMinuteList; }

    void        setCotType        (int idx);
    void        setStaleMinutes   (int mins);
    void        setUid            (QString uid);
    int         cotType           ();
    int         staleMinutes      ();
    QString     uid               ();
    QString     GetRandomString ();

signals:
    void        cotTypeChanged        ();
    void        staleMinutesChanged        ();
    void        uidChanged        ();

private slots:


private:
    QMap<QString, QString> _cotMap;
    QList<QPair<QString, int>> _minuteMap;
    QStringList _cotTypes;
    QStringList _staleMinuteList;    
    int _cotType = 0;
    int _staleMinutes = 0;
    QString _uid;
    QUdpSocket udpSocket4;
    QHostAddress _atakMcastAddress;
    int _atakMcastPort;

signals:
private:
};
