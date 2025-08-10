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
    name:                           qsTr("GPS")
    telemetryFailure:               _3dLockFailure || _satCountFailure || gps2failure()
   // telemetryTextFailure:           _3dLockFailure ?
   //                                     qsTr("Waiting for 3D lock.") :
   //                                     (_satCountFailure ? _satCountFailureText : "")
   // allowTelemetryFailureOverride:  !_3dLockFailure && _satCountFailure && allowOverrideSatCount
    allowTelemetryFailureOverride: true

    property bool   allowOverrideSatCount:  false   ///< true: sat count above failureSatCount reguired to pass, false: user can click past satCount <= failureSetCount
    property int    failureSatCount:        10      ///< -1 indicates no sat count check

    property bool   _3dLock:                globals.activeVehicle ? globals.activeVehicle.gps.lock.rawValue >= 3 : false
    property bool   _isGPS2Present:         globals.activeVehicle ? !isNaN(globals.activeVehicle.gps2.lat.value) : false
    property int    _satCount:              globals.activeVehicle ? globals.activeVehicle.gps.count.rawValue : 0
    property bool   _3dLock2:                globals.activeVehicle ? globals.activeVehicle.gps2.lock.rawValue >= 3 : false
    property int    _satCount2:              globals.activeVehicle ? globals.activeVehicle.gps2.count.rawValue : 0
    property bool   _satCountFailure2:       failureSatCount !== -1 && _satCount2 <= failureSatCount
    property bool   _3dLockFailure:         !_3dLock
    property bool   _3dLockFailure2:        !_3dLock2
    property bool   _satCountFailure:       failureSatCount !== -1 && _satCount <= failureSatCount
    property string _satCountFailureText:   allowOverrideSatCount ? qsTr("Warning - Sat count on GPS1 below %1.").arg(failureSatCount + 1) : qsTr("Waiting for sat count above %1.").arg(failureSatCount)
    property string _satCountFailureText2:   allowOverrideSatCount ? qsTr("Warning - Sat count on GPS2 below %1.").arg(failureSatCount + 1) : qsTr("Waiting for sat count above %1.").arg(failureSatCount)

    Component.onCompleted: updateTelemetryTextFailure()

    function gps2failure()
    {
        if (globals.activeVehicle)
        {
            if (!_isGPS2Present)
                return true  //consider no GPS2 a failure
            else
                return _3dLockFailure2 || _satCountFailure2
        }
        else
            return false
    }

    function updateTelemetryTextFailure() {
        if (!_isGPS2Present)
            telemetryTextFailure = qsTr("GPS2 is not functional!")
        else if (_3dLockFailure)
            telemetryTextFailure = qsTr("Waiting for 3D lock on GPS1.")
        else if (_isGPS2Present & _3dLockFailure2)
            telemetryTextFailure = qsTr("Waiting for 3D lock on GPS2.")
        else if (_satCountFailure)
            telemetryTextFailure = _satCountFailureText
        else if (_isGPS2Present & _satCountFailure2)
            telemetryTextFailure = _satCountFailureText2
        else return ""
    }



}
