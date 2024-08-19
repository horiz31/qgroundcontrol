/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleClockFactGroup.h"
#include "Vehicle.h"

const char* VehicleClockFactGroup::_currentTimeFactName = "currentTime";
const char* VehicleClockFactGroup::_currentDateFactName = "currentDate";

VehicleClockFactGroup::VehicleClockFactGroup(QObject* parent)
    : FactGroup(1000, ":/json/Vehicle/ClockFact.json", parent)
    , _currentTimeFact  (0, _currentTimeFactName,    FactMetaData::valueTypeString)
    , _currentDateFact  (0, _currentDateFactName,    FactMetaData::valueTypeString)
{
    _addFact(&_currentTimeFact, _currentTimeFactName);
    _addFact(&_currentDateFact, _currentDateFactName);

    // Start out as not available "--.--"
    _currentTimeFact.setRawValue(std::numeric_limits<float>::quiet_NaN());
    _currentDateFact.setRawValue(std::numeric_limits<float>::quiet_NaN());
}

void VehicleClockFactGroup::handleMessage(Vehicle* /* vehicle */, mavlink_message_t& message)
{
    switch (message.msgid)
    {
    case MAVLINK_MSG_ID_SYSTEM_TIME:
        _handleSystemTime(message);
        break;
    default:
        break;
    }
}

void VehicleClockFactGroup::_handleSystemTime(mavlink_message_t& message)
{
    mavlink_system_time_t systemTime;
    mavlink_msg_system_time_decode(&message, &systemTime);
    //convert to milliseconds and then to string
    auto const milliseconds = systemTime.time_unix_usec / 1000;
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(milliseconds);
    QTime time = dateTime.time();
    _currentTimeFact.setRawValue(time.toString());
    _currentDateFact.setRawValue(
        dateTime.toString(QLocale::system().dateFormat(QLocale::ShortFormat)));
    _setTelemetryAvailable(true);
}
