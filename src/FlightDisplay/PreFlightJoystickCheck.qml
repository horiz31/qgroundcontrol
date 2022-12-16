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
    name:               qsTr("Joystick and Mode Check")
    telemetryFailure:   joysticksDisabled() || notAcceptableMode()  //this causes the button to go yellow/red
    allowTelemetryFailureOverride: false

    specifiedBottomPadding: getPadding()
    property var    _activeVehicle:         globals.activeVehicle
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController
    property var    _buttonLabel:   qsTr("Set Mode to QHover")

    Button {
        id: modeButton
        text:           _buttonLabel
        visible:        notAcceptableMode()
        enabled:        true
        onClicked:      setMode()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function setMode()
        {
            _activeVehicle.flightMode = "QuadPlane Hover"
        }

    }

    Row{
        id: spacer
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom:  modeButton.top
        Item {
            width:  1
            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
        }
    }

    function getPadding()
    {
        if (notAcceptableMode() && joysticksDisabled())
            return (Math.round(ScreenTools.defaultFontPixelHeight / 2) + modeButton.height + spacer.height + Math.round(ScreenTools.defaultFontPixelHeight / 2))
        else
            return Math.round(ScreenTools.defaultFontPixelHeight / 2)
    }

    function joysticksDisabled()
    {
        if (joystickManager.activeJoystick && _activeVehicle.joystickEnabled)
        {
            return false
        }
        else
            return true
    }
    function notAcceptableMode()
    {
        if (_activeVehicle.flightMode === "QuadPlane Loiter" || _activeVehicle.flightMode === "QuadPlane Hover")
            return false
        return true
    }

    Component.onCompleted: updateTelemetryTextFailure()

    function updateTelemetryTextFailure() {      
        if(joysticksDisabled()) {
            telemetryTextFailure = qsTr("Failure. No joystick found or it is disabled. It is not recommended to operate the aircraft without a functional joystick.")
            return
         }

        if (notAcceptableMode())
        {
            telemetryTextFailure = qsTr("Failure. Set the aircraft to either QHOVER or QLOITER for initial takeoff.")
        }
    }
}

