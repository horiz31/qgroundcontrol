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
    id:  airSpeedCheckButton
    name:               qsTr("Air Speed Sensor")
    telemetryFailure:   (_airSpeed > _airSpeedLimit) || (Math.abs(_windOffsetfact) > _offSetError) //this causes the button to go yellow/red
    allowTelemetryFailureOverride: false
    specifiedBottomPadding: _telemetryState != _statePassed ? (Math.round(ScreenTools.defaultFontPixelHeight / 2) + calibrateButton.height + Math.round(ScreenTools.defaultFontPixelHeight / 2)) : Math.round(ScreenTools.defaultFontPixelHeight / 2)
    property real   _airSpeed:        globals.activeVehicle ? globals.activeVehicle.airSpeed.rawValue : 0
    property real   _offSetError:     150.0
    property real   _airSpeedLimit:   3.0
    property string   _buttonLabel:   qsTr("Calibrate Air Speed")
    property string   _buttonActionLabel:   qsTr("Calibrating...")


    FactPanelController { id: factController; }
    property bool   _windOffsetFailure:      Math.abs(_windOffsetfact) > _offSetError ? true : false
    property real   _windOffsetfact: factController.getParameterFact(-1, "ARSPD_OFFSET").value

    Button {
        id: calibrateButton
        text:           _buttonLabel
        visible:        _telemetryState != _statePassed
        enabled:        true
        onClicked:      calibrateAirSpeed()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function calibrateAirSpeed()
        {
            calibrateButton.text = _buttonActionLabel
            windTimer.restart()            
            globals.activeVehicle.startAirSpeedCalibration()
        }

        SequentialAnimation
        {
            id:     calButtonAnimator
            loops:          Animation.Infinite
            running: windTimer.running

            OpacityAnimator {
                   target: calibrateButton;
                   from: 1.0;
                   to: 0.7;
                   duration: 500
                   easing.type: Easing.OutQuad
               }
            OpacityAnimator {
                   target: calibrateButton;
                   from: 0.7;
                   to: 01;
                   duration: 500
                   easing.type: Easing.InQuad
               }
        }

        Timer {
            id:             windTimer
            interval:       3000
            repeat:         false
            running:        false

            onTriggered: {
                calButtonAnimator.stop();
                calibrateButton.opacity = 1.0;
                calibrateButton.text = _buttonLabel
            }

        }

    }


    //below example of a button with a confirmation, to be useful in motor startups
    /*
    QGCButton {
        text:           qsTr("Clear Saved Answers")
        enabled:        _enableAirMapFact.rawValue
        onClicked:      clearDialog.open()
        anchors.verticalCenter: parent.verticalCenter
        MessageDialog {
            id:                 clearDialog
            visible:            false
            icon:               StandardIcon.Warning
            standardButtons:    StandardButton.Yes | StandardButton.No
            title:              qsTr("Clear Saved Answers")
            text:               qsTr("All saved ruleset answers will be cleared. Is this really what you want?")
            onYes: {
                QGroundControl.airspaceManager.ruleSets.clearAllFeatures()
                clearDialog.close()
            }
            onNo: {
                clearDialog.close()
            }
        }
    }*/

    on_AirSpeedChanged: updateTelemetryTextFailure()

    //on_TelemetryStateChanged: console.log("telem state changed to " + _telemetryState)


    Component.onCompleted: updateTelemetryTextFailure()

    function updateTelemetryTextFailure() {      
        if((_airSpeed > _airSpeedLimit) || _windOffsetFailure) {           
            if (_airSpeed > _airSpeedLimit)               telemetryTextFailure = qsTr("Failure. Air speed measures " + _airSpeed.toFixed(1) + " m/s, which is above the maximum of 3 m/s. Please cover the sensor and restart the vehicle to recalibrate.")
            else if(_windOffsetFailure)                 telemetryTextFailure = qsTr("Failure. The Air Speed sensor calibration offset parameter is too high ("+ _windOffsetfact.toFixed(0) + "). The maximum expected value is +/- 150. Possibly a bad sensor, or shield the sensor and recalibrate.")
        }
    }
}
