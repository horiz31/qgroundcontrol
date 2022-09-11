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

class VehicleRPMFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleRPMFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* rpm1      READ rpm1      CONSTANT)
    Q_PROPERTY(Fact* rpm2          READ rpm2          CONSTANT)


    Fact* rpm1     () { return &_rpm1Fact; }
    Fact* rpm2         () { return &_rpm2Fact; }

    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _rpm1FactName;
    static const char* _rpm2FactName;


private:
    void _handleRPM        (mavlink_message_t& message);


    Fact        _rpm1Fact;
    Fact        _rpm2Fact;

};
