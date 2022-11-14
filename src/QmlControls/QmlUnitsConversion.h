/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef QMLUNITSCONVERSION_H
#define QMLUNITSCONVERSION_H

#include <QObject>
#include <qmath.h>
#include "FactMetaData.h"

class QmlUnitsConversion : public QObject
{
    Q_OBJECT
public:
    QmlUnitsConversion(QObject *parent=nullptr): QObject(parent) {}
    ~QmlUnitsConversion() = default;

    Q_PROPERTY(QString appSettingsHorizontalDistanceUnitsString   READ appSettingsHorizontalDistanceUnitsString CONSTANT)
    Q_PROPERTY(QString appSettingsTemperatureUnitsString          READ appSettingsTemperatureUnitsString CONSTANT)
    Q_PROPERTY(QString appSettingsVerticalDistanceUnitsString   READ appSettingsVerticalDistanceUnitsString   CONSTANT)
    Q_PROPERTY(QString appSettingsAreaUnitsString               READ appSettingsAreaUnitsString               CONSTANT)
    Q_PROPERTY(QString appSettingsWeightUnitsString             READ appSettingsWeightUnitsString             CONSTANT)
    Q_PROPERTY(QString appSettingsSpeedUnitsString              READ appSettingsSpeedUnitsString              CONSTANT)

    /// Converts from meters to the user specified distance unit
    Q_INVOKABLE QVariant metersToAppSettingsHorizontalDistanceUnits(const QVariant& meters) const { return FactMetaData::metersToAppSettingsHorizontalDistanceUnits(meters); }

    /// Converts from celcius to the user specified temperature unit
    Q_INVOKABLE QVariant celciusToAppSettingsTemperatureUnits(const QVariant& celcius) const { return FactMetaData::celciusToAppSettingsTemperatureUnits(celcius); }

    /// Converts from user specified distance unit to meters
    Q_INVOKABLE QVariant appSettingsHorizontalDistanceUnitsToMeters(const QVariant& distance) const { return FactMetaData::appSettingsHorizontalDistanceUnitsToMeters(distance); }

    /// Converts from user specified speed unit to meters per second
    Q_INVOKABLE QVariant appSettingsSpeedUnitsToMetersPerSecond(const QVariant& speed) const { return FactMetaData::appSettingsSpeedUnitsToMetersPerSecond(speed); }

    QString appSettingsHorizontalDistanceUnitsString(void) const { return FactMetaData::appSettingsHorizontalDistanceUnitsString(); }       

    /// Converts from meters to the user specified distance unit
    Q_INVOKABLE QVariant metersToAppSettingsVerticalDistanceUnits(const QVariant& meters) const { return FactMetaData::metersToAppSettingsVerticalDistanceUnits(meters); }

    /// Converts from metersPerSecond to the user specified speed unit
    Q_INVOKABLE QVariant metersPerSecondToAppSettingsSpeedUnits(const QVariant& metersPerSecond) const { return FactMetaData::metersPerSecondToAppSettingsSpeedUnits(metersPerSecond); }

    /// Converts from user specified distance unit to meters
    Q_INVOKABLE QVariant appSettingsVerticalDistanceUnitsToMeters(const QVariant& distance) const { return FactMetaData::appSettingsVerticalDistanceUnitsToMeters(distance); }

    QString appSettingsVerticalDistanceUnitsString(void) const { return FactMetaData::appSettingsVerticalDistanceUnitsString(); }

    QString appSettingsTemperatureUnitsString(void) const { return FactMetaData::appSettingsTemperatureUnitsString(); }

    /// Converts from grams to the user specified weight unit
    Q_INVOKABLE QVariant gramsToAppSettingsWeightUnits(const QVariant& meters) const { return FactMetaData::gramsToAppSettingsWeightUnits(meters); }

    /// Converts from user specified weight unit to grams
    Q_INVOKABLE QVariant appSettingsWeightUnitsToGrams(const QVariant& distance) const { return FactMetaData::appSettingsWeightUnitsToGrams(distance); }

    QString appSettingsWeightUnitsString(void) const { return FactMetaData::appSettingsWeightUnitsString(); }

    /// Converts from square meters to the user specified area unit
    Q_INVOKABLE QVariant squareMetersToAppSettingsAreaUnits(const QVariant& meters) const { return FactMetaData::squareMetersToAppSettingsAreaUnits(meters); }

    /// Converts from user specified area unit to square meters
    Q_INVOKABLE QVariant appSettingsAreaUnitsToSquareMeters(const QVariant& area) const { return FactMetaData::appSettingsAreaUnitsToSquareMeters(area); }

    QString appSettingsAreaUnitsString(void) const { return FactMetaData::appSettingsAreaUnitsString(); }

    /// Returns the string for speed units which has configued by user
    QString appSettingsSpeedUnitsString() { return FactMetaData::appSettingsSpeedUnitsString(); }

    Q_INVOKABLE double degreesToRadians(double degrees) { return qDegreesToRadians(degrees); }
    Q_INVOKABLE double radiansToDegrees(double radians) { return qRadiansToDegrees(radians); }
};

#endif // QMLUNITSCONVERSION_H
