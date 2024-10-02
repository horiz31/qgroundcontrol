/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtQuick.Dialogs              1.3
import QtQuick.Layouts              1.11

import QGroundControl               1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controllers   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

ColumnLayout{
    property Fact _camJoystickDZ: QGroundControl.settingsManager.appSettings.camJoystickDZ
    property Fact _joystickPitchRollEnableOption: QGroundControl.settingsManager.appSettings.camJoystickPitchRollEnableOption
    property Fact _camJoystickGain: QGroundControl.settingsManager.appSettings.camJoystickGain
    property Fact _camJoystickRollInvert: QGroundControl.settingsManager.appSettings.camJoystickRollInvert
    property Fact _camJoystickPitchInvert: QGroundControl.settingsManager.appSettings.camJoystickPitchInvert

    width:                  availableWidth
    height:                 mainCol.height + (ScreenTools.defaultFontPixelHeight * 2)
    readonly property real axisMonitorWidth: ScreenTools.defaultFontPixelWidth * 20
    ColumnLayout{
        id:                 mainCol
        spacing:            ScreenTools.defaultFontPixelHeight

        ListModel {
            id: pitchRollAxleModel

            ListElement {
                text:       qsTr("Hat Button")
            }
            ListElement {
                text:       qsTr("Left Analog Stick")
            }
            ListElement {
                text:       qsTr("Right Analog Stick")
            }
        }

        ListModel {
            id: pitchRollEnableOptionModel

            ListElement {
                text:       qsTr("Always")
            }
            ListElement {
                text:       qsTr("Only when 'Override Stick' button is depressed")
            }
        }

        GridLayout {
            columns:            2
            columnSpacing:      ScreenTools.defaultFontPixelWidth
            rowSpacing:         ScreenTools.defaultFontPixelHeight
            //---------------------------------------------------------------------
            //-- Enable Joystick
            QGCLabel {
                text:               qsTr("Enable joystick input")
                Layout.alignment:   Qt.AlignVCenter
                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 36
            }
            QGCCheckBox {
                id:             enabledSwitch
                enabled:        _activeCamJoystick ? true : false
                onClicked:      globals.activeVehicle.joystickCamEnabled = checked
                Component.onCompleted: {
                    checked = globals.activeVehicle.joystickCamEnabled
                }
                Connections {
                    target: globals.activeVehicle
                    onJoystickCamEnabledChanged: {
                        enabledSwitch.checked = globals.activeVehicle.joystickCamEnabled
                    }
                }
                Connections {
                    target: joystickManager
                    onActiveCamJoystickChanged: {
                        if(_activeCamJoystick) {
                            enabledSwitch.checked = Qt.binding(function() { return globals.activeVehicle.joystickCamEnabled })
                        }
                    }
                }
            }
            //---------------------------------------------------------------------
            //-- Joystick Selector
            QGCLabel {
                text:               qsTr("Active Camera joystick:")
                Layout.alignment:   Qt.AlignVCenter
            }
            QGCComboBox {
                id:                 joystickCombo
                width:              ScreenTools.defaultFontPixelWidth * 40
                Layout.alignment:   Qt.AlignVCenter
                model:              joystickManager.joystickNames
                onActivated:        joystickManager.activeCamJoystickName = textAt(index)
                Component.onCompleted: {
                    var index = joystickCombo.find(joystickManager.activeCamJoystickName)
                    if (index === -1) {
                        console.warn(qsTr("Active Camera joystick name not in combo"), joystickManager.activeCamJoystickName)
                    } else {
                        joystickCombo.currentIndex = index
                    }
                }
                Connections {
                    target: joystickManager
                    onAvailableJoysticksChanged: {
                        var index = joystickCombo.find(joystickManager.activeCamJoystickName)
                        if (index >= 0) {
                            joystickCombo.currentIndex = index
                        }
                    }
                }
            }

            //---------------------------------------------------------------------
            //-- Joystick Analog Axle Selector
            QGCLabel {
                text:               qsTr("Camera Pitch & Roll/Yaw Axle:")
                Layout.alignment:   Qt.AlignVCenter
            }

            QGCComboBox {
                id:                 pitchRollAxleCombo
                width:              ScreenTools.defaultFontPixelWidth * 40
                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 20
                Layout.alignment:   Qt.AlignVCenter
                model:              pitchRollAxleModel
                Component.onCompleted: {
                    pitchRollAxleCombo.currentIndex = _activeCamJoystick.camPitchRollAxle
                }
                onActivated:        _activeCamJoystick.camPitchRollAxle = index               
            }

            QGCLabel {
                text:               qsTr("Camera Pitch & Roll Enable:")
                Layout.alignment:   Qt.AlignVCenter
            }

            //the idea is that in scenarios where there are only two joysticks, an enable button can be configured which converts the roll/pitch input into camera control ONLY when the enable button is depressed. Otherwise it'll be normal aircraft
            FactComboBox {
                id:                 joystickPitchRollEnableOption
                sizeToContents: true
                //width:              ScreenTools.defaultFontPixelWidth * 80
                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 20
                Layout.alignment:   Qt.AlignVCenter
                fact:               _joystickPitchRollEnableOption
                indexModel:         false
                onActivated: {
                    joystickManager.activeCamJoystick.setCamJoystickPitchRollEnableOption(index)
                }
            }


            QGCLabel {
                text:                   qsTr("Joystick DZ[%]:")
                color:                  "white"
                font.pointSize:         ScreenTools.isMobile ? ScreenTools.mediumFontPointSize : 9
            }

            FactTextField {
                id:                     _joystickDz
                Layout.preferredWidth:  ScreenTools.isMobile ? 90 : 45
                maximumLength:          6
                font.pointSize:         ScreenTools.isMobile ? ScreenTools.mediumFontPointSize : 9
                fact:                   _camJoystickDZ
                showHelp:               false
                onEditingFinished: {
                    if ( _joystickDz.text < 0 || _joystickDz.text > 25 )
                    {
                        mainWindow.showMessageDialog(qsTr("Bad Dead Zone Value"), qsTr("You must enter a value between 0 - 25, Reverting to Default (3)"))
                        _camJoystickDZ.rawValue = 3
                        _joystickDz.text = 3
                    }
                    joystickManager.activeCamJoystick.setCamJoystickDZ(_joystickDz.text)
                }
            }

            QGCLabel {
                text:                   qsTr("Joystick Gain[%]:")
                color:                  "white"
                font.pointSize:         ScreenTools.isMobile ? ScreenTools.mediumFontPointSize : 9
            }

            FactTextField {
                id:                     _joystickGain
                Layout.preferredWidth:  ScreenTools.isMobile ? 90 : 45
                maximumLength:          6
                font.pointSize:         ScreenTools.isMobile ? ScreenTools.mediumFontPointSize : 9
                fact:                   _camJoystickGain
                showHelp:               false
                onEditingFinished: {
                    if ( _joystickGain.text < 10 || _joystickGain.text > 200 )
                    {
                        mainWindow.showMessageDialog(qsTr("Bad Gain Value"), qsTr("You must enter a value between 10 - 200, Reverting to Default (100)"))
                        _camJoystickGain.rawValue = 100
                        _joystickGain.text = 100
                    }
                    joystickManager.activeCamJoystick.setCamJoystickGain(_joystickGain.text)
                }
            }
        }
        RowLayout{
            spacing:                ScreenTools.defaultFontPixelWidth
            //---------------------------------------------------------------------
            //-- Axis Monitors
            Rectangle {
                id:                 axisRect
                color:              Qt.rgba(0,0,0,0)
                border.color:       qgcPal.text
                border.width:       1
                radius:             ScreenTools.defaultFontPixelWidth * 0.5
                Layout.minimumWidth: axisGrid.width  + (ScreenTools.defaultFontPixelWidth  * 2)
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                height:             axisGrid.height + (ScreenTools.defaultFontPixelHeight * 2)
                GridLayout {
                    id:                 axisGrid
                    columns:            3
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    rowSpacing:         ScreenTools.defaultFontPixelHeight
                    anchors.centerIn:   parent
                    QGCLabel {
                        text:               qsTr("Roll/Yaw")
                    }
                    AxisMonitor {
                        id:                 rollAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        Layout.minimumWidth: axisMonitorWidth
                        Layout.fillWidth: true
                        mapped:             controller.rollAxisMapped
                        reversed:           controller.rollAxisReversed
                    }

                    FactCheckBox {
                        id:         rollInvert
                        text:       qsTr("Reverse")
                        fact:       _camJoystickRollInvert
                        visible:    _camJoystickRollInvert.visible
                        onCheckedChanged: {
                            joystickManager.activeCamJoystick.setCamJoystickRollInvert(checked)
                        }
                    }

                    QGCLabel {
                        id:                 pitchLabel
                        text:               qsTr("Pitch")
                    }
                    AxisMonitor {
                        id:                 pitchAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        Layout.minimumWidth: axisMonitorWidth
                        Layout.fillWidth: true
                        mapped:             controller.pitchAxisMapped
                        reversed:           controller.pitchAxisReversed
                    }

                    FactCheckBox {
                        id:         pitchInvert
                        text:       qsTr("Reverse")
                        fact:       _camJoystickPitchInvert
                        visible:    _camJoystickPitchInvert.visible
                        onCheckedChanged: {
                            joystickManager.activeCamJoystick.setCamJoystickPitchInvert(checked)
                        }
                    }

                    Connections {
                        target:             _activeCamJoystick
                        onManualControlCamQml: {
                            rollAxis.axisValue      = roll_yaw
                            pitchAxis.axisValue     = pitch
                        }
                    }                   
                }
            }
            Rectangle {
                color:              Qt.rgba(0,0,0,0)
                border.color:       qgcPal.text
                border.width:       1
                radius:             ScreenTools.defaultFontPixelWidth * 0.5
                Layout.preferredWidth: axisRect.width;
                Layout.alignment: Qt.AlignRight
                height:             axisRect.height
                Flow {
                    width: parent.width
                    spacing:            -1
                    anchors.centerIn:   parent
                    Connections {
                        target:     _activeCamJoystick
                        onRawButtonPressedChanged: {
                            if (buttonMonitorRepeater.itemAt(index)) {
                                buttonMonitorRepeater.itemAt(index).pressed = pressed
                            }
                        }
                    }
                    Repeater {
                        id:         buttonMonitorRepeater
                        model:      _activeCamJoystick ? _activeCamJoystick.totalButtonCount : []
                        Rectangle {
                            width:          ScreenTools.defaultFontPixelHeight * 1.5
                            height:         width
                            border.width:   1
                            border.color:   qgcPal.text
                            color:          pressed ? qgcPal.buttonHighlight : qgcPal.windowShade
                            property bool pressed
                            QGCLabel {
                                anchors.fill:           parent
                                color:                  pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
                                horizontalAlignment:    Text.AlignHCenter
                                verticalAlignment:      Text.AlignVCenter
                                text:                   modelData
                            }
                        }
                    }
                }
            }
        }
    }
}


