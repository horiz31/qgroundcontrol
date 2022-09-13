/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick 2.3

import QGroundControl           1.0
import QGroundControl.Controls  1.0
import QGroundControl.Vehicle   1.0

PreFlightCheckButton {
    name:                   qsTr("IMU Temperature")
    telemetryTextFailure:   qsTr("Warning - The IMU is still warming, the temperature is " + _IMUTemp.toFixed(0) +" °C / 40 °C. It is recommended to wait until it is heated before takeoff.")
    telemetryFailure:       _unhealthyIMUTemp
    allowTelemetryFailureOverride:    true

    property int    _imuTempLimit: 40

    property real _IMUTemp: globals.activeVehicle ? globals.activeVehicle.imuTemperature.rawValue : 0
    property bool _unhealthyIMUTemp: _IMUTemp < _imuTempLimit
}
