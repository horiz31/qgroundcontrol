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
    name:               qsTr("Mission Check")
    telemetryFailure:   missionLacksLoiterUnlimited() || missionLacksVTOLTakeoff()   //this causes the button to go yellow/red
    allowTelemetryFailureOverride: true

    property var    _activeVehicle:         globals.activeVehicle
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController

    function missionLacksVTOLTakeoff()
    {
            if (_activeVehicle.fixedWing || _activeVehicle.vtol)
            {
                for (var i = 1; i < _missionController.visualItems.count; i++) {
                    var missionItem = _missionController.visualItems.get(i)
                    if (missionItem.command === 84) { //MAV_CMD_NAV_VTOL_TAKEOFF
                       return false
                    }
                }
            }
            return true
    }
    function missionLacksLoiterUnlimited()
    {
            if (_activeVehicle.fixedWing || _activeVehicle.vtol)
            {
                for (var i = 1; i < _missionController.visualItems.count; i++) {
                    var missionItem = _missionController.visualItems.get(i)
                    if (missionItem.command === 17) { //MAV_CMD_NAV_LOITER_UNLIM
                       return false
                    }
                }
            }
             return true
    }
    function missionLacksWP()
    {
            if (_activeVehicle.fixedWing || _activeVehicle.vtol)
            {
                for (var i = 1; i < _missionController.visualItems.count; i++) {
                    var missionItem = _missionController.visualItems.get(i)
                    if (missionItem.command === 16) { //MAV_CMD_NAV_WAYPOINT
                       return false
                    }
                }
            }
            return true
    }
    function missionLacksVTOLLand()
    {
            if (_activeVehicle.fixedWing || _activeVehicle.vtol)
            {
                for (var i = 1; i < _missionController.visualItems.count; i++) {
                    var missionItem = _missionController.visualItems.get(i)
                    if (missionItem.command === 85) { //MAV_CMD_NAV_VTOL_LAND
                        return false
                    }
                }
            }
            return true
    }
    function missionLacksDoStartLand()
    {
        if (_activeVehicle.fixedWing || _activeVehicle.vtol)
        {
            for (var i = 1; i < _missionController.visualItems.count; i++) {
                var missionItem = _missionController.visualItems.get(i)
                if (missionItem.command === 189) {  //MAV_CMD_DO_LAND_START
                   return false
                }
            }
        }
        return true
    }

    Component.onCompleted: updateTelemetryTextFailure()

    function updateTelemetryTextFailure() {      
        if(missionLacksLoiterUnlimited() || missionLacksVTOLTakeoff()) {
            if (missionLacksLoiterUnlimited())        telemetryTextFailure = qsTr("Failure. It is recommended that a mission contain a loiter unlimited waypoint.")
            else if(missionLacksVTOLTakeoff())        telemetryTextFailure = qsTr("Failure. The loaded mission does not contain a VTOL Takeoff command.")
        }
    }
}

