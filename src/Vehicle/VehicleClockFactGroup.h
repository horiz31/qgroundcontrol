/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"

class Vehicle;

class VehicleClockFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleClockFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* currentTime READ currentTime CONSTANT)
    Q_PROPERTY(Fact* currentDate READ currentDate CONSTANT)

    Fact* currentTime () { return &_currentTimeFact; }
    Fact* currentDate () { return &_currentDateFact; }

    static const char* _currentTimeFactName;
    static const char* _currentDateFactName;

    static const char* _settingsGroup;

    // Overrides from FactGroup
    virtual void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

private slots:

private:
    void _handleSystemTime(mavlink_message_t& message);

    Fact            _currentTimeFact;
    Fact            _currentDateFact;
};
