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
import QtQuick.Controls         2.12
import QGroundControl.ScreenTools   1.0
import QtQuick.Layouts  1.11
import QtQuick.Dialogs      1.2
import QGroundControl.Palette       1.0
import QGroundControl.SettingsManager 1.0

PreFlightCheckButton {
    name:               qsTr("Engine Run Up")
    manualText:         qsTr("A run up of the engine is recommended to ensure operation prior to takeoff. Click to acknowledge or perform a run up procedure.")

    specifiedBottomPadding: Math.round(ScreenTools.defaultFontPixelHeight / 2) + engineTestButton.height + Math.round(ScreenTools.defaultFontPixelHeight / 2)
    property bool   allowFailurePercentOverride:    false
    property string   _buttonLabel:   qsTr("Engine Run Up Test")
    readonly property int _sliderWidth:        25

    property bool   _joyStickInitialState: false
    property bool   _virtualJoyStickInitialState: false
    property string _modeInitialState: ""
    property bool _virtualJoystickEnabled: QGroundControl.settingsManager.appSettings.virtualJoystick.rawValue
    property real _joyValue: -1


    Button {
        id: engineTestButton
        text:           _buttonLabel
        visible:        true
        enabled:        true
        onClicked:      startEngineTest()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function startEngineTest()
        {          
            mainWindow.showPopupDialogFromComponent(iceMotorTestComponent)
        }

    }

    Component {
        id: iceMotorTestComponent
        QGCPopupDialog {
            title:      qsTr("Run Up Engine")
            buttons:    StandardButton.Close
            ColumnLayout {
                id: iceMotorTestCol
                Layout.fillWidth:   true
                Layout.fillHeight:   true

                RowLayout{
                    Layout.fillWidth:           true
                    Layout.fillHeight:          true
                    spacing: ScreenTools.defaultFontPixelWidth * 4
                    Column
                    {
                        width: ScreenTools.defaultFontPixelWidth * 60
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            height:     ScreenTools.defaultFontPixelHeight * 4
                            wrapMode:       Text.WordWrap
                            text:           qsTr("WARNING: Running up the engine is dangerous! Ensure that the prop is clear of the ground, obstacles and people. Set the desired throttle level, then push and hold the button below. Release to stop the engine.")
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }
                        Row{
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: ScreenTools.defaultFontPixelWidth
                            QGCLabel {
                                text:           qsTr("Throttle Level:")
                            }
                            QGCSlider {
                                id:                         iceMotorThrottle
                                width:                      ScreenTools.defaultFontPixelWidth * _sliderWidth
                                maximumValue:               100
                                minimumValue:               30
                                stepSize:                   10
                                value:                      70
                                updateValueWhileDragging:   true
                                visible:                    true
                                onValueChanged:             {
                                    iceMotorThrottleValue.text = value + "%"
                                }

                            }
                            QGCLabel {
                                id:   iceMotorThrottleValue
                                text:           iceMotorThrottle.value + "%"
                            }
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
                        }
                        DelayButton {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text:               qsTr("Push and Hold To Start Engine")
                            palette {
                                   button: "red"
                               }
                            delay: 1500
                            onActivated: {
                                console.log("Button Pressed and held");

                                if(globals.activeVehicle)
                                    globals.activeVehicle.setEngineRunUp(true)  //indicator that this is running, used to prevent system level activity which could interfere

                                // remember current mode
                                 _modeInitialState = globals.activeVehicle.flightMode
                                 console.log("mode is currently " + _modeInitialState)
                                //remember joystick state
                                if(globals.activeVehicle && joystickManager.activeJoystick) {
                                    if(globals.activeVehicle.joystickEnabled) {
                                        _joyStickInitialState = true
                                    }
                                    else {
                                        _joyStickInitialState = false
                                    }
                                }

                                if (_virtualJoystickEnabled)
                                {
                                    _virtualJoyStickInitialState = true
                                    QGroundControl.settingsManager.appSettings.virtualJoystick.value = false;
                                }

                                console.log("joystick is currently " + _joyStickInitialState)
                                //if enabled, disable joysticks
                                if (_joyStickInitialState && globals.activeVehicle)
                                {
                                    console.log("disabling the joystick")
                                    globals.activeVehicle.joystickEnabled = false
                                }

                                console.log("changing mode to manual")
                                globals.activeVehicle.flightMode = "Manual"

                                _joyValue = (iceMotorThrottle.value * 10) + 1000  //scale slider to 1000-2000
                                console.log("sending joystick throttle value of " + _joyValue)
                                globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 1000 to 2000
                                iceRunUpTimer.start()
                                text= qsTr("Release to Stop")
                                progress = 0.0
                            }
                            onReleased: {
                                iceRunUpTimer.stop()
                                globals.activeVehicle.sendRcOverrideThrottle(900)  //turn off                                
                                globals.activeVehicle.setEngineRunUp(false)

                                text= qsTr("Push and Hold To Start Engine")
                                //set throttle level 0
                                //set mode back to previous mode
                                //if joysticks were enabled, re-enable
                                console.log("Button released, changing mode back to what it was");
                                 globals.activeVehicle.flightMode = _modeInitialState
                                if (_joyStickInitialState && globals.activeVehicle)
                                {
                                    console.log("re-enabling the joystick")
                                    globals.activeVehicle.joystickEnabled = true
                                }
                                //set virtual joystick back if it was used
                                if (_virtualJoyStickInitialState)
                                    QGroundControl.settingsManager.appSettings.virtualJoystick.value = true;
                                //mark the state as passed
                                _manualState = _statePassed
                            }

                            Timer {
                                id:             iceRunUpTimer
                                interval:       500
                                repeat:         true

                                onTriggered: {
                                    _joyValue = (iceMotorThrottle.value * 10) + 1000  //scale slider to 1000-2000
                                    console.log("timer: sending joystick throttle value of " + _joyValue)
                                    globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 1000 to 2000
                                }

                            }
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }
                    }
                }
                Item {
                    width:  1
                    height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
                }
                RowLayout
                {
                    Layout.fillWidth:           true
                    Layout.alignment:   Qt.AlignHCenter
                    QGCButton {
                        text:               qsTr("Close")
                        onClicked: {
                            hideDialog()
                        }
                    }

                }
            }
            function reject() {
                hideDialog()
            }
        }
    }
}
