/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "GPSUnitsController.h"
#include "QGCGeo.h"

GPSUnitsController::GPSUnitsController(void)
{}

QString GPSUnitsController::convertToMGRS(QGeoCoordinate coordinate, int precision)
{
    QString mgrs = convertGeoToMGRS(coordinate, precision);
    return mgrs;
}

QString GPSUnitsController::convertToMGRS(double latitude, double longitude, int precision)
{
    QString mgrs = convertGeoToMGRS(latitude, longitude, precision);
    return mgrs;
}
