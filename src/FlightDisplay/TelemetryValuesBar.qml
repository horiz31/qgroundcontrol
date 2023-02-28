/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.12
import QtQuick.Layouts              1.12

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0




Rectangle {
    id:                 telemetryPanel
    height:             telemetryLayout.height + (_toolsMargin * 2)
    width:              telemetryLayout.width + (_toolsMargin * 2)
    color:              qgcPal.window
    opacity:            0.95
    radius:             ScreenTools.defaultFontPixelWidth / 2

    property bool    bottomMode: true
    property var     _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool    _nvRecording: _activeVehicle? _activeVehicle.nvGimbal.isRecording.value === 1 : false
    property string  _targetLatitude: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLat.value) || (_activeVehicle.nvGimbal.groundCrossingLat.value === 400.0)) ? "--.-------°" : _activeVehicle.nvGimbal.groundCrossingLat.value.toFixed(7) + "°") : "--.-------°"
    property string  _targetLongitude: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLon.value) || (_activeVehicle.nvGimbal.groundCrossingLon.value === 400.0)) ? "--.-------°" : _activeVehicle.nvGimbal.groundCrossingLon.value.toFixed(7) + "°") : "--.-------°"
    property string  _targetAltitude: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingAlt.value) || (_activeVehicle.nvGimbal.groundCrossingAlt.rawValue === 10000)) ? "----" : _activeVehicle.nvGimbal.groundCrossingAlt.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString) + " MSL" : "----"
    property string  _slantRange:_activeVehicle ? (isNaN(_activeVehicle.nvGimbal.slantRange.value) ? "----" : _activeVehicle.nvGimbal.slantRange.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString) : "----"
    property string  _fov: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.fov.value) ? "--°" : _activeVehicle.nvGimbal.fov.value.toFixed(0) + "°") : "--°"
    property string  _azimuth: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.azimuth.value) ? "--°" : _activeVehicle.nvGimbal.azimuth.value.toFixed(0) + "°") : "--°"
    property bool    _nvPresent:  _activeVehicle ? (_activeVehicle.nvGimbal.mode.value !== "" ? true : false) : false


    //DeadMouseArea { anchors.fill: parent }


    function toggleNvPanel()
    {
        if (!nvTelemGrid.visible && _nvPresent)
        {
            nvTelemGrid.visible = true
            nvTitle.visible = true
        }
        else {
            nvTelemGrid.visible = false
            nvTitle.visible = false
        }
    }
    MouseArea {
           anchors.fill: parent
           onClicked: {
             toggleNvPanel()
           }
    }

    Connections {
        target: _activeVehicle
        onNvShowQuickPanelChanged:
        {
            toggleNvPanel()
        }
    }
    ColumnLayout {
        id:                 telemetryLayout
        anchors.margins:    _toolsMargin
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left

         RowLayout {
            //visible: mouseArea.containsMouse || valueArea.settingsUnlocked
             visible: false

            QGCColoredImage {
                source:             "/res/layout-bottom.svg"
                mipmap:             true
                width:              ScreenTools.minTouchPixels * 0.75
                height:             width
                sourceSize.width:   width
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit
                visible:            !bottomMode

                QGCMouseArea {
                    fillItem:   parent
                    onClicked:  bottomMode = true
                }
            }

            QGCColoredImage {
                source:             "/res/layout-right.svg"
                mipmap:             true
                width:              ScreenTools.minTouchPixels * 0.75
                height:             width
                sourceSize.width:   width
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit
                visible:            bottomMode

                QGCMouseArea {
                    fillItem:   parent
                    onClicked:  bottomMode = false
                }
            }

            QGCColoredImage {
                source:             valueArea.settingsUnlocked ? "/res/LockOpen.svg" : "/res/pencil.svg"
                mipmap:             true
                width:              ScreenTools.minTouchPixels * 0.75
                height:             width
                sourceSize.width:   width
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit               

                QGCMouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape:  Qt.PointingHandCursor
                    onClicked:    valueArea.settingsUnlocked = !valueArea.settingsUnlocked
                }
            }
        }
/*
        QGCMouseArea {
            id:                         mouseArea
            x:                          telemetryLayout.x
            y:                          telemetryLayout.y
            width:                      telemetryLayout.width
            height:                     telemetryLayout.height
            hoverEnabled:               true
            propagateComposedEvents:    true
        }
*/


         RowLayout
         {
             id: nvTitle
             visible: false
             Item {
                 Layout.fillWidth: true
             }

             QGCColoredImage {
                 Layout.alignment:  Qt.AlignLeft
                 source:             "/res/target.svg"
                 mipmap:             true
                 width:              ScreenTools.defaultFontPixelHeight
                 height:             width
                 sourceSize.width:   width
                 color:              qgcPal.text
                 fillMode:           Image.PreserveAspectFit
            }
            QGCLabel { text: qsTr("Gimbal Target Information")
                font.family:        ScreenTools.demiboldFontFamily
                font.pointSize:     ScreenTools.mediumFontPointSize
                Layout.alignment: Qt.AlignHCenter
            }
            Item {
                Layout.fillWidth: true
            }
         }

        // NextVision Tracking Value Panel
        GridLayout {

            id: nvTelemGrid
            visible: false
            Layout.fillWidth: true
            rowSpacing:     ScreenTools.defaultFontPixelHeight
            columnSpacing:  ScreenTools.defaultFontPixelWidth * 2
            rows: 4
            columns: 2
            Layout.bottomMargin: ScreenTools.defaultFontPixelHeight * .5



            ColumnLayout {
                Layout.fillWidth: true
                    RowLayout{
                    QGCLabel { text: qsTr("Target Latitude: ") + _targetLatitude
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.mediumFontPointSize
                        Layout.fillWidth: false

                    }
                    QGCColoredImage {
                        Layout.alignment:  Qt.AlignLeft
                        source:             "/res/content_copy.svg"
                        mipmap:             true
                        width:              ScreenTools.defaultFontPixelHeight
                        height:             width
                        sourceSize.width:   width
                        color:              qgcPal.text
                        fillMode:           Image.PreserveAspectFit
                        QGCMouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape:  Qt.PointingHandCursor
                            onClicked:    {
                                textEdit.text = _targetLatitude + ", " + _targetLongitude
                                textEdit.selectAll()
                                textEdit.copy()
                            }
                            TextEdit {
                                   id: textEdit
                                   visible: false
                            }
                        }
                   }
                    Item {
                        Layout.fillWidth: true
                    }
                }



                QGCLabel { text: qsTr("Target Longitude: ") + _targetLongitude
                    font.family:        ScreenTools.demiboldFontFamily
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                    QGCMouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape:  Qt.PointingHandCursor
                        onClicked:    {
                            textEdit2.text = _targetLatitude + ", " + _targetLongitude
                            textEdit2.selectAll()
                            textEdit2.copy()
                        }
                        TextEdit {
                               id: textEdit2
                               visible: false
                        }
                    }
                }
                QGCLabel { text: qsTr("Target Altitude: ") + _targetAltitude
                    font.family:        ScreenTools.demiboldFontFamily
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }


            }
            ColumnLayout {
                Layout.alignment: Qt.AlignRight
                Layout.fillWidth: true
                Layout.fillHeight: true
                Item {
                        width: recIndicator.implicitWidth
                        height: recIndicator.implicitHeight
                        QGCLabel { text: _nvRecording ? qsTr("Recording to SD: YES") : qsTr("Recording to SD: NO")
                            id: recIndicator
                            color: _nvRecording ? qgcPal.colorRed : qgcPal.text
                            font.family:        ScreenTools.demiboldFontFamily
                            font.pointSize:     ScreenTools.mediumFontPointSize
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }

                QGCLabel { text: qsTr("Slant Range: ") + _slantRange
                    font.family:        ScreenTools.demiboldFontFamily
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }

                QGCLabel { text: qsTr("Azimuth: ") + _azimuth
                    font.family:        ScreenTools.demiboldFontFamily
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }

            }


        }


        //Super Volo Telemetry Value Panel

        GridLayout {

            id: superVoloTelemGrid
            visible: true
            columns: 3
            rows: 1
            rowSpacing:     ScreenTools.defaultFontPixelWidth
            columnSpacing:  ScreenTools.defaultFontPixelWidth * 2

            RowLayout {
                    Image {
                        source:             "/res/distance-to-wp.svg"
                        mipmap:             true
                        fillMode:           Image.PreserveAspectFit
                        sourceSize.height:  ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel { text: _activeVehicle ? _activeVehicle.missionItemIndex.value : qsTr("-", "No data to display")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: qsTr("|")
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }

                    QGCLabel { text: _activeVehicle ? _activeVehicle.distanceToNextWP.value.toFixed(0) : qsTr("--", "No data to display")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.distanceToNextWP.units : qsTr("")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: qsTr("    ")
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }

            }


            RowLayout {
                    Image {
                        source:             "/res/distance-to-home.svg"  //todo make distance to wp icon
                        mipmap:             true
                        fillMode:           Image.PreserveAspectFit
                        sourceSize.height:  ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel { text: _activeVehicle && !isNaN(_activeVehicle.distanceToHome.rawValue) ? _activeVehicle.distanceToHome.value.toFixed(0) : qsTr("--", "No data to display")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.distanceToHome.units : qsTr("")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: _activeVehicle && !isNaN(_activeVehicle.bearingFromHome.rawValue) ? "∠" + _activeVehicle.bearingFromHome.rawValue.toFixed(0) + "°" : qsTr("--", "No data to display")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.bearingFromHome.units : qsTr("")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
                    QGCLabel { text: qsTr("    ")
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }
            }


            RowLayout {
                    Image {
                        source:             "/res/flight_timer.svg"
                        mipmap:             true
                        fillMode:           Image.PreserveAspectFit
                        sourceSize.height:  ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel { text: _activeVehicle ? _activeVehicle.flightTime.valueString : qsTr("--:--:--", "No data to display")
                        font.family:        ScreenTools.demiboldFontFamily
                        font.pointSize:     ScreenTools.largeFontPointSize
                    }

            }



        }
        //Set to false, so we replace this with our own custom telemetry output above
        HorizontalFactValueGrid {
            id:                     valueArea
            userSettingsGroup:      telemetryBarUserSettingsGroup
            defaultSettingsGroup:   telemetryBarDefaultSettingsGroup
            visible: false //true
        }

        GuidedActionConfirm {
            opacity: 1.0
            Layout.fillWidth:   true
            guidedController:   _guidedController
            altitudeSlider:     _guidedAltSlider
        }
    }
}
