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
    property bool _engineRunupAllowed: QGroundControl.settingsManager.appSettings.allowRunupInChecklist.rawValue
    property real _joyValue: -1
    property bool _isJoystickRunupActive: false
    property bool _cancelTest: false  //flag to keep track of the user requesting a test cancel
    property bool _isMotorTestStarting: false  //flat to keep track of if the test is running
    property bool _isMotorTestRunning: false
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle


    Button {
        id: engineTestButton
        text:           _buttonLabel
        visible:        true
        enabled:        true
        onClicked:      startEngineTest()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth * 3
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function startEngineTest()
        {          
            preFlightChecklistWindow.showPopupDialogFromComponent(iceMotorTestComponent)
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
                            visible:    _engineRunupAllowed
                            wrapMode:       Text.WordWrap
                            text:           qsTr("WARNING: Running up the engine is dangerous! Ensure that the prop is clear of the ground, obstacles and people. Set the desired throttle level, then push and hold the button below. Release to stop the engine.")
                        }
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            height:     ScreenTools.defaultFontPixelHeight * 4
                            visible:    !_engineRunupAllowed
                            wrapMode:       Text.WordWrap
                            text:           qsTr("WARNING: Running up the engine is dangerous! Ensure that the prop is clear of the ground, obstacles and people. Follow the instructions below to do an engine running using the hand controller.")
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }

                        //we want two sets of instructions, depending on if _engineRunupAllowed is enabled
                        //if it is, then we use the instructions below, if not, then we instruct to use the hand held controllers

                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            height:     ScreenTools.defaultFontPixelHeight * 4
                            wrapMode:       Text.WordWrap
                            visible:    _engineRunupAllowed
                            text:           qsTr("Use the slider below to start the engine runup test. Alternatively, if you have the 'Hold for Engine Runup' joystick button action configured, you can press and hold that button to start the test.")
                        }
                        Item {
                            width:  1
                            visible:    _engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }

                        //these instructions are shown if enginerunup is NOT allowed
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            //height:     ScreenTools.defaultFontPixelHeight * 1
                            wrapMode:       Text.WordWrap
                            visible:    !_engineRunupAllowed
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           qsTr("1. Enter MANUAL control mode by raising the lower right toggle switch on the hand remote then depressing the top left push button.")
                        }
                        Item {
                            width:  1
                            visible:    !_engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .3)
                        }
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            //height:     ScreenTools.defaultFontPixelHeight * 1
                            wrapMode:       Text.WordWrap
                            visible:    !_engineRunupAllowed
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           qsTr("2. Lift the tail of the aircraft to provide clearance for the prop, then raise the throttle to 100%.")
                        }
                        Item {
                            width:  1
                            visible:    !_engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .3)
                        }
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            //height:     ScreenTools.defaultFontPixelHeight * 1
                            wrapMode:       Text.WordWrap
                            visible:    !_engineRunupAllowed
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           qsTr("3. Allow engine to run while monitoring the cyclinder temperature below, then shut down and restart to ensure the engine is starting quickly.")
                        }
                        Item {
                            width:  1
                            visible:    !_engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .3)
                        }
                        QGCLabel {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:          ScreenTools.defaultFontPixelWidth * 60
                            //height:     ScreenTools.defaultFontPixelHeight * 1
                            wrapMode:       Text.WordWrap
                            visible:    !_engineRunupAllowed
                            font.pointSize: ScreenTools.mediumFontPointSize
                            text:           qsTr("4. Lower throttle on the hand remote then return all toggle switches to the down position.")
                        }
                        Item {
                            width:  1
                            visible:    _engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }
                        Item {
                            width:  1
                            visible:    !_engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
                        }

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: ScreenTools.defaultFontPixelWidth
                            QGCLabel { text: qsTr("Cylinder Temp (Target 150°F+): ")
                                       font.pointSize: ScreenTools.mediumFontPointSize
                                       font.family:    ScreenTools.demiboldFontFamily
                            }
                            QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.cylinderTemp.value) ? "---.- °F" : _activeVehicle.hcu.cylinderTemp.value.toFixed(1) + " F") : "---.- °F"
                                       font.pointSize: ScreenTools.mediumFontPointSize
                                       font.family:    ScreenTools.demiboldFontFamily
                                       color:              getEngineColor()

                                       function getEngineColor() {
                                           if (!_activeVehicle)
                                                return qgcPal.text

                                           if (_activeVehicle.hcu.cylinderTemp.value > 275)
                                               return qgcPal.colorRed
                                           else if (_activeVehicle.hcu.cylinderTemp.value > 260)
                                               return qgcPal.colorOrange
                                           else if (_activeVehicle.hcu.cylinderTemp.value > 250)
                                               return qgcPal.colorYellow
                                           else if (_activeVehicle.hcu.cylinderTemp.value > 150)
                                               return qgcPal.colorGreen
                                           else
                                               return qgcPal.colorBlue
                                       }
                            }
                        }
                        Item {
                            width:  1
                            visible:    _engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * 1.5)
                        }
                        Row{
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: ScreenTools.defaultFontPixelWidth
                            visible:    _engineRunupAllowed
                            QGCLabel {
                                text:           qsTr("Throttle Level:")
                            }
                            QGCSlider {
                                id:                         iceMotorThrottle
                                width:                      ScreenTools.defaultFontPixelWidth * _sliderWidth
                                maximumValue:               100
                                minimumValue:               55
                                stepSize:                   10
                                value:                      75
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
                            visible: _isJoystickRunupActive && _engineRunupAllowed
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: ScreenTools.mediumFontPointSize
                            font.family:    ScreenTools.demiboldFontFamily
                            text:           qsTr("Hold button for 2 seconds...")
                            color:          qgcPal.text
                        }
                        Item {
                            width:  1
                            visible: _isJoystickRunupActive && _engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
                        }

                        SliderSwitch {
                            id:                     iceMotorTestButton
                            visible:    _engineRunupAllowed && !iceMotorJoystickRunupLabel.visible
                            confirmText:            qsTr("Slide to Start Engine RunUp")
                            Layout.minimumWidth:    Math.max(implicitWidth, ScreenTools.defaultFontPixelWidth * 30)
                            anchors.horizontalCenter: parent.horizontalCenter

                            onAccept: {
                                visible = false
                                startRunup(false)  //false in this case means source is not from joystick
                                _isMotorTestStarting=true;
                            }

                            Timer {
                                id:             iceRunUpTimer
                                interval:       250
                                repeat:         true

                                onTriggered: {
                                    _joyValue = (iceMotorThrottle.value * 10)  //scale slider to 0-1000
                                    console.log("timer: sending joystick throttle value of " + _joyValue)
                                    globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 0 to 1000
                                }

                            }



                            Connections {
                                target: engineRunupController
                                onJoystickStartRunup: {
                                    if (!_engineRunupAllowed)
                                        return
                                    //console.log("qml got joystick runup start")
                                    _cancelTest= false
                                    _isJoystickRunupActive = true
                                    //start oneshot timer
                                    timer1.setTimeout(function(){}, 2 * 1000);
                                }
                                onJoystickStopRunup: {
                                    if (!_engineRunupAllowed)
                                        return
                                    //console.log("qml got joystick runup stop")
                                    timer1.stop()
                                    stopRunup(true)
                                    _isJoystickRunupActive = false
                                    _cancelTest = true
                                    iceMotorJoystickRunupLabel.text = qsTr("Hold button for 2 seconds....")
                                    iceMotorJoystickRunupLabel.color = qgcPal.text
                                   }
                            }

                        }

                        QGCButton {
                            text:               qsTr("STOP ENGINE")
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible: !iceMotorTestButton.visible && _engineRunupAllowed && !iceMotorJoystickRunupLabel.visible
                            warning: true
                            onClicked: {

                                stopRunup(false)
                                _isMotorTestStarting = false
                                iceMotorTestButton.visible = true
                               }

                            PropertyAnimation on opacity {
                                easing.type:    Easing.OutQuart
                                from:           0.5
                                to:             1
                                loops:          Animation.Infinite
                                running:        true
                                alwaysRunToEnd: true
                                duration:       1000
                            }


                        }
                        Item {
                            width:  1
                            visible: _engineRunupAllowed
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }

                        /*
                        DelayButton {
                            id: iceMotorTestButton
                            visible: !_isJoystickRunupActive
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 20
                            font.pointSize: ScreenTools.mediumFontPointSize
                            font.family:    ScreenTools.demiboldFontFamily
                            text: qsTr("Push and Hold To Start Engine")                            
                            palette {
                                   button: "red"
                               }
                            palette.buttonText: "white"
                            delay: 1500
                            onActivated: {
                                startRunup(false)
                                _isMotorTestStarting=true;
                                text= qsTr("Release to Stop")
                                progress = 0.0
                            }

                            onDownChanged: {
                                console.log("on down changed");
                            }

                            onReleased: {
                                if (_isMotorTestStarting)
                                    stopRunup(false)
                                _isMotorTestStarting = false
                                text= qsTr("Push and Hold To Start Engine")
                            }

                            Timer {
                                id:             iceRunUpTimer
                                interval:       250
                                repeat:         true

                                onTriggered: {
                                    _joyValue = (iceMotorThrottle.value * 10)  //scale slider to 0-1000
                                    console.log("timer: sending joystick throttle value of " + _joyValue)
                                    globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 0 to 1000
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

                        */
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
                                        iceMotorJoystickRunupLabel.text = qsTr("Engine running, release to stop...")
                                        iceMotorJoystickRunupLabel.color = "red"
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
                    visible: _engineRunupAllowed
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
                            stopRunup(false)
                            _isMotorTestStarting = false
                            iceMotorTestButton.visible = true
                            hideDialog()
                        }
                    }
                }
            }
            function startRunup(fromJoystick)
            {
                if(globals.activeVehicle)
                    globals.activeVehicle.setEngineRunUp(true)  //indicator that this is running, used to prevent system level activity which could interfere

                // remember current mode
                 _modeInitialState = globals.activeVehicle.flightMode
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
                //if enabled, disable joysticks
                if (_joyStickInitialState && globals.activeVehicle && fromJoystick===false)
                {
                    globals.activeVehicle.joystickEnabled = false
                }

                globals.activeVehicle.flightMode = "Manual"

                _joyValue = (iceMotorThrottle.value * 10)  //scale slider to 0 to 1000
                globals.activeVehicle.sendRcOverrideThrottle(_joyValue)  //scaled 0 to 1000
                iceRunUpTimer.start()  //this is needed to send rc override periodically

                _isMotorTestRunning = true;

            }

            function stopRunup(fromJoystick)
            {
                //if (_isMotorTestRunning)
                //{
                    iceRunUpTimer.stop()
                    //set throttle level 0
                    for (var i = 0; i < 4; i++)  {
                      globals.activeVehicle.sendRcOverrideThrottle(0)  //turn off multiple times to make sure one gets in
                    }

                    globals.activeVehicle.setEngineRunUp(false)

                    //set mode back to previous mode
                    //if joysticks were enabled, re-enable

                    globals.activeVehicle.flightMode = _modeInitialState
                    if (_joyStickInitialState && globals.activeVehicle && fromJoystick===false)
                    {
                        globals.activeVehicle.joystickEnabled = true
                    }
                    //set virtual joystick back if it was used
                    if (_virtualJoyStickInitialState)
                        QGroundControl.settingsManager.appSettings.virtualJoystick.value = true;
                    //mark the state as passed
                    _manualState = _statePassed

                    globals.activeVehicle.sendRcOverrideThrottle(0)  //turn off one more time to ensure if comms glitch it makes it
                //}
                _isMotorTestRunning = false;
            }
            function reject() {
                stopRunup(false);
                engineRunupController.disconnectJoystick()
                hideDialog()
            }
        }
    }
}
