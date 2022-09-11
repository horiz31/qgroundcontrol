/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleRPMFactGroup.h"
#include "Vehicle.h"

#include <QtMath>

const char* VehicleRPMFactGroup::_rpm1FactName =      "rpm1";
const char* VehicleRPMFactGroup::_rpm2FactName =       "rpm2";


VehicleRPMFactGroup::VehicleRPMFactGroup(QObject* parent)
    : FactGroup(1000, ":/json/Vehicle/RPMFact.json", parent)
    , _rpm1Fact    (0, _rpm1FactName,     FactMetaData::valueTypeDouble)
    , _rpm2Fact        (0, _rpm2FactName,         FactMetaData::valueTypeDouble)
{
    _addFact(&_rpm1Fact,       _rpm1FactName);
    _addFact(&_rpm2Fact,           _rpm2FactName);


    // Start out as not available "--.--"
    _rpm1Fact.setRawValue      (qQNaN());
    _rpm2Fact.setRawValue          (qQNaN());

}

void VehicleRPMFactGroup::handleMessage(Vehicle* /* vehicle */, mavlink_message_t& message)
{
    switch (message.msgid) {

    case MAVLINK_MSG_ID_RPM:
        _handleRPM(message);
        break;
    default:
        break;
    }
}


void VehicleRPMFactGroup::_handleRPM(mavlink_message_t& message)
{
    mavlink_rpm_t rpm;
    mavlink_msg_rpm_decode(&message, &rpm);

    //qDebug() << "setting rpm1 to " << rpm.rpm1;
    (rpm.rpm1 < 1) ? rpm1()->setRawValue(0): rpm1()->setRawValue(rpm.rpm1);
    (rpm.rpm2 < 1) ? rpm2()->setRawValue(0): rpm2()->setRawValue(rpm.rpm2);
    _setTelemetryAvailable(true);
}

