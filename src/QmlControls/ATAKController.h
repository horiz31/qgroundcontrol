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
#include <QGeoCoordinate>
#include "FactPanelController.h"


class ATAKController : public FactPanelController
{
    Q_OBJECT
    
public:
    ATAKController();

    Q_PROPERTY(int              cotType       READ cotType          WRITE setCotType            NOTIFY cotTypeChanged)
    Q_PROPERTY(QStringList      cotTypes      READ cotTypes                                     CONSTANT)

    Q_INVOKABLE void send(QGeoCoordinate);

    QStringList     cotTypes      () { return _cotTypes; }

    void        setCotType        (int idx);
    int         cotType            ();


signals:
    void        cotTypeChanged        ();

private slots:


private:
    QStringList _cotTypes;
    int _cotType = 0;


signals:
private:
};
