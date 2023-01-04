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
    name:               qsTr("Actuator Check")
    telemetryFailure:   true  //this causes the button to go yellow/red
    allowTelemetryFailureOverride: true

    specifiedBottomPadding: getPadding()
    property var    _activeVehicle:         globals.activeVehicle
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController
    property var    _buttonLabel:   qsTr("Set Mode to FBWA")

    Button {
        id: modeButton
        text:           _buttonLabel
        visible:        notAcceptableMode()
        enabled:        true
        onClicked:      setMode()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth * 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function setMode()
        {
            _activeVehicle.flightMode = "FBW A"
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
        if (notAcceptableMode())
            return (Math.round(ScreenTools.defaultFontPixelHeight / 2) + modeButton.height + spacer.height + Math.round(ScreenTools.defaultFontPixelHeight / 2))
        else
            return Math.round(ScreenTools.defaultFontPixelHeight / 2)
    }

    function notAcceptableMode()
    {
        if (_activeVehicle.flightMode === "FBW A")
            return false
        return true
    }

    Component.onCompleted: updateTelemetryTextFailure()

    Connections {
        target: _activeVehicle
        onFlightModeChanged: updateTelemetryTextFailure()
    }


    function updateTelemetryTextFailure() {      
        if (notAcceptableMode())
        {
            telemetryTextFailure = qsTr("Pick up the aircraft move it, ensuring the control surfaces respond to input. Recommend to put the aircraft in FBWA mode for this check to ensure surfaces respond appropriately.")
            return
        }
        else
        {
            telemetryTextFailure = qsTr("Pick up the aircraft move it, ensuring the control surfaces respond to input.")
            return
        }


    }
}

