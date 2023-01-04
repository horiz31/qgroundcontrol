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
import QGroundControl.Controllers   1.0

PreFlightCheckButton {
    name:               qsTr("Engine Run Up")
    manualText:         qsTr("A run up of the engine is recommended to warm the engine and validate operation immediately prior to takeoff. Click below to start the run up procedure. Take off within a few minutes of a runup while the engine is still warm.")

    specifiedBottomPadding: Math.round(ScreenTools.defaultFontPixelHeight / 2) + engineTestButton.height + Math.round(ScreenTools.defaultFontPixelHeight / 2)
    property bool   allowFailurePercentOverride:    false
    property string   _buttonLabel:   qsTr("Engine Run Up Test")
    readonly property int _sliderWidth:        25

    property bool   _joyStickInitialState: false
    property bool   _virtualJoyStickInitialState: false
    property string _modeInitialState: ""
    property bool _virtualJoystickEnabled: QGroundControl.settingsManager.appSettings.virtualJoystick.rawValue
    property real _joyValue: -1
    property bool _isJoystickRunupActive: false
    property bool _cancelTest: false  //flag to keep track of the user requesting a test cancel
    property bool _isMotorTestStarting: false  //flat to keep track of if the test is running
    property bool _isMotorTestRunning: false


    Button {
        id: engineTestButton
        text:           _buttonLabel
        visible:        true
        enabled:        true
        onClicked:      startEngineTest()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth * 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function startEngineTest()
        {          
            mainWindow.showPopupDialogFromComponent(iceMotorTestComponent)
            engineRunupController.connectJoystick()
        }
    }

    EngineRunupController {
        id: engineRunupController
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
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            height:     ScreenTools.defaultFontPixelHeight * 4
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Use the button below to start the engine runup test. Alternatively, if you have the 'Hold for Engine Runup' joystick action configured, you can press and hold that button to start the test.")
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
                                minimumValue:               55
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
                        QGCLabel {
                            id: iceMotorJoystickRunupLabel
                            visible: _isJoystickRunupActive
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: ScreenTools.mediumFontPointSize
                            font.family:    ScreenTools.demiboldFontFamily
                            text:           qsTr("Hold button for 2 seconds...")
                        }
                        DelayButton {
                            id: iceMotorTestButton
                            visible: !_isJoystickRunupActive
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 20

                            text: qsTr("Push and Hold To Start Engine")
                            palette {
                                   button: "red"
                               }
                            delay: 1500
                            onActivated: {
                                startRunup(false)
                                _isMotorTestStarting=true;
                                text= qsTr("Release to Stop")
                                progress = 0.0
                            }
                            onReleased: {
                                if (_isMotorTestStarting)
                                    stopRunup(false)
                                _isMotorTestStarting = false
                                text= qsTr("Push and Hold To Start Engine")
                            }

                            Timer {
                                id:             iceRunUpTimer
                                interval:       500
                                repeat:         true

                                onTriggered: {
                                    _joyValue = (iceMotorThrottle.value * 10) + 1000  //scale slider to 1000-2000
                                    //console.log("timer: sending joystick throttle value of " + _joyValue)
                                    globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 1000 to 2000
                                }

                            }
                            Connections {
                                target: engineRunupController
                                onJoystickStartRunup: {
                                    //console.log("qml got joystick runup start")
                                    _cancelTest= false
                                    _isJoystickRunupActive = true
                                    //start oneshot timer
                                    timer1.setTimeout(function(){}, 2 * 1000);
                                }
                                onJoystickStopRunup: {
                                    //console.log("qml got joystick runup stop")
                                    timer1.stop()
                                    stopRunup(true)
                                    _isJoystickRunupActive = false
                                    _cancelTest = true
                                    iceMotorJoystickRunupLabel.text = qsTr("Hold button for 2 seconds....")
                                   }
                            }
                        }
                        Timer {
                            id: timer1
                            function setTimeout(cb, delayTime) {
                                timer1.interval = delayTime;
                                timer1.repeat = false;
                                timer1.triggered.connect(cb);
                                timer1.triggered.connect(function release () {
                                    timer1.triggered.disconnect(cb);
                                    timer1.triggered.disconnect(release);
                                    //if not cancelled
                                    if (!_cancelTest)
                                    {
                                        //console.log("starting runup after delay")
                                        iceMotorJoystickRunupLabel.text = qsTr("Engine running, release to stop...")
                                        timer1.stop()
                                        startRunup(true)
                                    }

                                });
                                timer1.start();
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
                            engineRunupController.disconnectJoystick()
                            hideDialog()
                        }
                    }
                }
            }
            function startRunup(fromJoystick)
            {
                //TBD, put start up logic here
                if(globals.activeVehicle)
                    globals.activeVehicle.setEngineRunUp(true)  //indicator that this is running, used to prevent system level activity which could interfere

                // remember current mode
                 _modeInitialState = globals.activeVehicle.flightMode
                //console.log("mode is currently " + _modeInitialState)
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

                //if this was initiated with joystick click, then I want to leave them enabled, but only disable the throttle
                //console.log("joystick is currently " + _joyStickInitialState)
                //if enabled, disable joysticks
                if (_joyStickInitialState && globals.activeVehicle && fromJoystick===false)
                {
                    //console.log("disabling the joystick")
                    globals.activeVehicle.joystickEnabled = false
                }

                //console.log("changing mode to manual")
                globals.activeVehicle.flightMode = "Manual"

                _joyValue = (iceMotorThrottle.value * 10) + 1000  //scale slider to 1000-2000
                //console.log("sending joystick throttle value of " + _joyValue)
                globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 1000 to 2000
                iceRunUpTimer.start()  //this is needed to send rc override periodically

                _isMotorTestRunning = true;

            }

            function stopRunup(fromJoystick)
            {
                if (_isMotorTestRunning)
                {
                    iceRunUpTimer.stop()
                    globals.activeVehicle.sendRcOverrideThrottle(900)  //turn off
                    globals.activeVehicle.setEngineRunUp(false)

                    //set throttle level 0
                    //set mode back to previous mode
                    //if joysticks were enabled, re-enable

                     globals.activeVehicle.flightMode = _modeInitialState
                    if (_joyStickInitialState && globals.activeVehicle && fromJoystick===false)
                    {
                        //console.log("re-enabling the joystick")
                        globals.activeVehicle.joystickEnabled = true
                    }
                    //set virtual joystick back if it was used
                    if (_virtualJoyStickInitialState)
                        QGroundControl.settingsManager.appSettings.virtualJoystick.value = true;
                    //mark the state as passed
                    _manualState = _statePassed
                }
                _isMotorTestRunning = false;
            }
            function reject() {
                engineRunupController.disconnectJoystick()
                hideDialog()
            }
        }
    }
}
