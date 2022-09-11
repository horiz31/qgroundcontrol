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

class VehicleHCUFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleHCUFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* rpm      READ rpm      CONSTANT)
    Q_PROPERTY(Fact* eThrust          READ eThrust          CONSTANT)
    Q_PROPERTY(Fact* carbPosition          READ carbPosition          CONSTANT)
    Q_PROPERTY(Fact* cylinderTemp          READ cylinderTemp          CONSTANT)
    Q_PROPERTY(Fact* generator          READ generator          CONSTANT)
    Q_PROPERTY(Fact* chargeRate          READ chargeRate          CONSTANT)
    Q_PROPERTY(Fact* engineHealth          READ engineHealth          CONSTANT)



    Fact* rpm     () { return &_rpmFact; }
    Fact* eThrust     () { return &_eThrustFact; }
    Fact* carbPosition     () { return &_carbPositionFact; }
    Fact* cylinderTemp     () { return &_cylinderTempFact; }
    Fact* generator     () { return &_generatorFact; }
    Fact* chargeRate     () { return &_chargeRateFact; }
    Fact* engineHealth     () { return &_engineHealthFact; }


    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _rpmFactName;
    static const char* _eThrustFactName;
    static const char* _carbPositionFactName;
    static const char* _cylinderTempFactName;
    static const char* _generatorFactName;
    static const char* _chargeRateFactName;
    static const char* _engineHealthFactName;



private:
    void _handleEFI        (mavlink_message_t& message);


    Fact        _rpmFact;
    Fact        _eThrustFact;
    Fact        _carbPositionFact;
    Fact        _cylinderTempFact;
    Fact        _generatorFact;
    Fact        _chargeRateFact;
    Fact        _engineHealthFact;


};
