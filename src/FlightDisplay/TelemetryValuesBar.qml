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
    radius:             ScreenTools.defaultFontPixelWidth / 2

    property bool       bottomMode: true
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    DeadMouseArea { anchors.fill: parent }


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

        QGCMouseArea {
            id:                         mouseArea
            x:                          telemetryLayout.x
            y:                          telemetryLayout.y
            width:                      telemetryLayout.width
            height:                     telemetryLayout.height
            hoverEnabled:               true
            propagateComposedEvents:    true
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
            Layout.fillWidth:   true
            guidedController:   _guidedController
            altitudeSlider:     _guidedAltSlider
        }
    }
}
