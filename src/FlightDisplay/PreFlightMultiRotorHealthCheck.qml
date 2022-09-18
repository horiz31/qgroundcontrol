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
    name:               qsTr("Quad Motor Run Up")
    manualText:         qsTr("A run up of the electric quadrotor motors is recommend to ensure proper operation prior to flight.")

    specifiedBottomPadding: Math.round(ScreenTools.defaultFontPixelHeight / 2) + motorTestButton.height + Math.round(ScreenTools.defaultFontPixelHeight / 2)
    property bool   allowFailurePercentOverride:    false
    property string   _buttonLabel:   qsTr("Quad Motor Run Up")
    property int    _motorTestThrottle: 20  //percent throttle to use for motor runup
    property int    _motorTestDurationSec: 3  //seconds for the motor test to last
    property int    _testDelayDurationSec: 5  //seconds to delay between each motor test message

    Button {
        id:             motorTestButton
        text:           _buttonLabel
        visible:        true
        enabled:        true
        onClicked:      startMotorTest()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.round(ScreenTools.defaultFontPixelHeight / 2)

        function startMotorTest()
        {          
            mainWindow.showPopupDialogFromComponent(motorTestComponent)
        }

    }

    Component {
        id: motorTestComponent
        QGCPopupDialog {
            title:      qsTr("Quad Motor Run Up")
            buttons:    StandardButton.Close
            ColumnLayout {
                id: motorTestCol
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
                            height:         ScreenTools.defaultFontPixelHeight * 4
                            wrapMode:       Text.WordWrap
                            text:           qsTr("WARNING: Running up the quad motors is dangerous! Ensure the props are clear, then push and hold the button below. The test will spin each motor sequentially for 3 seconds each. Watch each motor and ensure it spins normally.")
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * 1)
                        }
                        DelayButton {
                            id: multiDelayButton
                            anchors.horizontalCenter: parent.horizontalCenter
                            text:               qsTr("Push and Hold To Start Motor Test")
                            palette {
                                   button: "red"
                               }
                            delay: 1500
                            onActivated: {
                                visible = false
                                setTimeout(startMotorTest, 0, 1)
                                setTimeout(startMotorTest, _testDelayDurationSec * 1000, 2)
                                setTimeout(startMotorTest, (_testDelayDurationSec * 1000) * 2, 3)
                                setTimeout(startMotorTest, (_testDelayDurationSec * 1000) * 3, 4)
                                setTimeout(finishMotorTest, (_testDelayDurationSec * 1000) * 4, null)
                            }

                            function setTimeout(func, interval, ...params) {
                                    return setTimeoutComponent.createObject(null, { func, interval, params} );
                                }

                                function clearTimeout(timerObj) {
                                    timerObj.stop();
                                    timerObj.destroy();
                                }

                                Component {
                                    id: setTimeoutComponent
                                    Timer {
                                        property var func
                                        property var params
                                        running: true
                                        repeat: false
                                        onTriggered: {
                                            func(...params);
                                            destroy();
                                        }
                                    }
                                }
                            function startMotorTest(motor)
                            {
                                motorTestStatus.text= qsTr("Test Running! (Motor "+ motor +")")

                                console.log("Starting motor test for motor " + motor);
                                globals.activeVehicle.motorTest(motor, _motorTestThrottle, _motorTestDurationSec, true)
                            }
                            function finishMotorTest()
                            {
                                motorTestStatus.text =""
                                visible = true
                                _manualState = _statePassed  //mark check as passed
                            }
                        }
                        Item {
                            width:  1
                            height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
                        }
                        QGCLabel {
                            id: motorTestStatus
                            anchors.horizontalCenter: parent.horizontalCenter
                            text:           qsTr("")
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
