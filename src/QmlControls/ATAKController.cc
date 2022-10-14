/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ATAKController.h"
#include "QGCApplication.h"
#include "QGCLoggingCategory.h"

#include <QDebug>

ATAKController::ATAKController(void)
{
    _cotTypes.append(QStringLiteral("Vehicle"));
    _cotTypes.append(QStringLiteral("Tank"));

}

void ATAKController::send(QGeoCoordinate coordinate)
{
    qDebug() << "Sending ATAK mcast message";
    qDebug() << "The specified type is" << _cotTypes[_cotType];
}

void
ATAKController::setCotType(int idx)
{
    _cotType = idx;
}

int
ATAKController::cotType()
{
    return _cotType;
}
