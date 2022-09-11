/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleHCUFactGroup.h"
#include "Vehicle.h"

#include <QtMath>

const char* VehicleHCUFactGroup::_rpmFactName =             "rpm";
const char* VehicleHCUFactGroup::_eThrustFactName =         "eThrust";
const char* VehicleHCUFactGroup::_carbPositionFactName =    "carbPosition";
const char* VehicleHCUFactGroup::_cylinderTempFactName =    "cylinderTemp";
const char* VehicleHCUFactGroup::_generatorFactName =       "generator";
const char* VehicleHCUFactGroup::_chargeRateFactName =      "chargeRate";
const char* VehicleHCUFactGroup::_engineHealthFactName =    "engineHealth";


VehicleHCUFactGroup::VehicleHCUFactGroup(QObject* parent)
    : FactGroup(1000, ":/json/Vehicle/HCUFact.json", parent)
    , _rpmFact              (0, _rpmFactName,     FactMetaData::valueTypeDouble)
    , _eThrustFact          (0, _eThrustFactName,         FactMetaData::valueTypeDouble)
    , _carbPositionFact     (0, _carbPositionFactName,         FactMetaData::valueTypeDouble)
    , _cylinderTempFact     (0, _cylinderTempFactName,         FactMetaData::valueTypeDouble)
    , _generatorFact        (0, _generatorFactName,         FactMetaData::valueTypeDouble)
    , _chargeRateFact       (0, _chargeRateFactName,         FactMetaData::valueTypeDouble)
    , _engineHealthFact     (0, _engineHealthFactName,         FactMetaData::valueTypeDouble)
{
    _addFact(&_rpmFact,            _rpmFactName);
    _addFact(&_eThrustFact,        _eThrustFactName);
    _addFact(&_carbPositionFact,   _carbPositionFactName);
    _addFact(&_cylinderTempFact,   _cylinderTempFactName);
    _addFact(&_generatorFact,      _generatorFactName);
    _addFact(&_chargeRateFact,     _chargeRateFactName);
    _addFact(&_engineHealthFact,   _engineHealthFactName);


    // Start out as not available "--.--"
    _rpmFact.setRawValue                (qQNaN());
    _eThrustFact.setRawValue            (qQNaN());
    _carbPositionFact.setRawValue       (qQNaN());
    _cylinderTempFact.setRawValue       (qQNaN());
    _generatorFact.setRawValue          (qQNaN());
    _chargeRateFact.setRawValue         (qQNaN());
    _engineHealthFact.setRawValue       (qQNaN());

}

void VehicleHCUFactGroup::handleMessage(Vehicle* /* vehicle */, mavlink_message_t& message)
{
    switch (message.msgid) {

    case MAVLINK_MSG_ID_EFI_STATUS:
        _handleEFI(message);
        break;
    default:
        break;
    }
}


void VehicleHCUFactGroup::_handleEFI(mavlink_message_t& message)
{
    mavlink_efi_status_t efi_status;
    mavlink_msg_efi_status_decode(&message, &efi_status);

    //qDebug() << "setting rpm to " << efi_status.rpm;
    (efi_status.rpm < 1) ? rpm()->setRawValue(0): rpm()->setRawValue(efi_status.rpm);
    (efi_status.spark_dwell_time < 1) ? eThrust()->setRawValue(0): eThrust()->setRawValue(efi_status.spark_dwell_time);
    (efi_status.throttle_position < 1) ? carbPosition()->setRawValue(0): carbPosition()->setRawValue(efi_status.throttle_position);
    (efi_status.cylinder_head_temperature < 1) ? cylinderTemp()->setRawValue(0): cylinderTemp()->setRawValue(efi_status.cylinder_head_temperature);
    (efi_status.engine_load < 1) ? generator()->setRawValue(0): generator()->setRawValue(efi_status.engine_load);
    (efi_status.barometric_pressure < 1) ? chargeRate()->setRawValue(0): chargeRate()->setRawValue(efi_status.barometric_pressure);
    (efi_status.intake_manifold_pressure < 1) ? engineHealth()->setRawValue(0): engineHealth()->setRawValue(efi_status.intake_manifold_pressure);

    _setTelemetryAvailable(true);
}

