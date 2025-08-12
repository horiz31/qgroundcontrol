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
import QGroundControl.FactSystem      1.0
import QGroundControl.FactControls    1.0
import QtQuick.Controls         2.12
import QGroundControl.ScreenTools   1.0

PreFlightCheckButton {    
    name:               qsTr("Mission Check")
    telemetryFailure:   missionLacksLoiterUnlimited() || missionLacksVTOLTakeoff() || missionWPisZero()//this causes the button to go yellow/red
    allowTelemetryFailureOverride: true
    specifiedBottomPadding: getPadding()
    property var    _activeVehicle:         globals.activeVehicle
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController
    property int    _currentMissionIndex:   _missionController.currentMissionIndex

    Button {
        id: modeButton
        text:           "Set Mission Index To Beginning"
        visible:        missionWPisZero()
        enabled:        true
        onClicked:      setMissionToZero()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth * 3
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function setMissionToZero()
        {
            _activeVehicle.setCurrentMissionSequence(0)
        }

    }

    Row{
        id: spacer
        visible: missionWPisZero()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom:  modeButton.top
        Item {
            width:  1
            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
        }
    }

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
            if (_activeVehicle && (_activeVehicle.fixedWing || _activeVehicle.vtol) && _missionController.visualItems)
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
           if (_activeVehicle && (_activeVehicle.fixedWing || _activeVehicle.vtol))
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
            if (_activeVehicle && (_activeVehicle.fixedWing || _activeVehicle.vtol))
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
       if (_activeVehicle && (_activeVehicle.fixedWing || _activeVehicle.vtol))
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

    function missionWPisZero()
    {
        if (_activeVehicle && (_activeVehicle.fixedWing || _activeVehicle.vtol))
        {
            if (_activeVehicle.missionItemIndex.value === 0 || _activeVehicle.missionItemIndex.value === 1)
            {
                return false
            }
        }
        return true

    }

    function getPadding()
    {
        if (missionWPisZero())
            return (Math.round(ScreenTools.defaultFontPixelHeight / 2) + modeButton.height + spacer.height + Math.round(ScreenTools.defaultFontPixelHeight / 2))
        else
            return Math.round(ScreenTools.defaultFontPixelHeight / 2)
    }

    Component.onCompleted: updateTelemetryTextFailure()

    Connections {
        target:                 _missionController
        ignoreUnknownSignals:   true
        function onNewItemsFromVehicle() {            
            updateTelemetryTextFailure();
        }
    }
    Connections {
        target:                 _missionController
        ignoreUnknownSignals:   true
        function onMissionItemCountChanged() {            
            updateTelemetryTextFailure();
        }
    }

    Connections {
        target: _missionController
        onCurrentMissionIndexChanged: {          
            updateTelemetryTextFailure();
        }
    }




    function updateTelemetryTextFailure() {              
        if(missionLacksLoiterUnlimited() || missionLacksVTOLTakeoff() || missionWPisZero()) {
            if (missionLacksLoiterUnlimited())        telemetryTextFailure = qsTr("Failure. It is recommended that a mission contain a loiter unlimited waypoint.")
            else if(missionLacksVTOLTakeoff())        telemetryTextFailure = qsTr("Failure. The loaded mission does not contain a VTOL Takeoff command.")
            else if(missionWPisZero())                telemetryTextFailure = qsTr("Failure. The current waypoint is not waypoint at the start of mission")
        }
    }


}

