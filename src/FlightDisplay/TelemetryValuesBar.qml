

/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/
import QtQuick 2.12
import QtQuick.Layouts 1.12

import QGroundControl 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.Vehicle 1.0
import QGroundControl.Controls 1.0
import QGroundControl.Palette 1.0

Rectangle {
    id: telemetryPanel
    height: telemetryLayout.height + (_toolsMargin * 2)
    width: telemetryLayout.width + (_toolsMargin * 2)
    color: qgcPal.window
    opacity: 0.95
    radius: ScreenTools.defaultFontPixelWidth / 2

    property var mouseCursor
    property bool bottomMode: true
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool _nvRecording: _activeVehicle ? _activeVehicle.nvGimbal.isRecording.value
                                                 === 1 : false
    property string _targetCoordinates: _activeVehicle
                                        && (_activeVehicle.nvGimbal.groundCrossingLat.value
                                            !== 400.0
                                            && _activeVehicle.nvGimbal.groundCrossingLon.value
                                            !== 400.0) ? QGroundControl.unitsConversion.appSettingsGeoCoordinateToString(_activeVehicle.nvGimbal.groundCrossingLat.value, _activeVehicle.nvGimbal.groundCrossingLon.value, 7) : QGroundControl.unitsConversion.appSettingsGeoCoordinateToString()
    property string _targetAltitude: _activeVehicle ? ((isNaN(
                                                            _activeVehicle.nvGimbal.groundCrossingAlt.value) || (_activeVehicle.nvGimbal.groundCrossingAlt.rawValue === 10000)) ? "----" : _activeVehicle.nvGimbal.groundCrossingAlt.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString) + " MSL" : "----"
    property string _slantRange: _activeVehicle ? (isNaN(
                                                       _activeVehicle.nvGimbal.slantRange.value) ? "----" : _activeVehicle.nvGimbal.slantRange.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString) : "----"
    property string _fov: _activeVehicle ? (isNaN(
                                                _activeVehicle.nvGimbal.fov.value) ? "--°" : _activeVehicle.nvGimbal.fov.value.toFixed(0) + "°") : "--°"
    property string _azimuth: _activeVehicle ? (isNaN(
                                                    _activeVehicle.nvGimbal.azimuth.value) ? "--°" : _activeVehicle.nvGimbal.azimuth.value.toFixed(0) + "°") : "--°"
    property string _pitch: _activeVehicle ? (isNaN(
                                                  _activeVehicle.nvGimbal.pitch.value) ? "--°" : _activeVehicle.nvGimbal.pitch.value.toFixed(0) + "°") : "--°"
    property bool _nvPresent: _activeVehicle ? (_activeVehicle.nvGimbal.mode.value
                                                !== "" ? true : false) : false
    property string _vehicleCoordinate: _activeVehicle ? QGroundControl.unitsConversion.appSettingsGeoCoordinateToString(
                                                             _activeVehicle.coordinate,
                                                             7) : QGroundControl.unitsConversion.appSettingsGeoCoordinateToString()

    function toggleNvPanel() {
        if (!nvTelemGrid.visible && _nvPresent) {
            nvTelemGrid.visible = true
            nvTitle.visible = true
        } else {
            nvTelemGrid.visible = false
            nvTitle.visible = false
        }
    }

    function toggleDetails() {
        if (!detailGrid.visible)
            detailGrid.visible = true
        else
            detailGrid.visible = false
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            toggleNvPanel()
        }
    }

    Connections {
        target: _activeVehicle
        onNvShowQuickPanelChanged: {
            toggleNvPanel()
        }
    }
    ColumnLayout {
        id: telemetryLayout
        anchors.margins: _toolsMargin
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        RowLayout {
            id: nvTitle
            visible: false
            Item {
                Layout.fillWidth: true
            }

            QGCColoredImage {
                Layout.alignment: Qt.AlignLeft
                source: "/res/target.svg"
                mipmap: true
                width: ScreenTools.defaultFontPixelHeight
                height: width
                sourceSize.width: width
                color: qgcPal.text
                fillMode: Image.PreserveAspectFit
            }
            QGCLabel {
                text: qsTr("Gimbal Target Information")
                font.family: ScreenTools.demiboldFontFamily
                font.pointSize: ScreenTools.mediumFontPointSize
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
            rowSpacing: ScreenTools.defaultFontPixelHeight
            columnSpacing: ScreenTools.defaultFontPixelWidth * 2
            rows: 4
            columns: 2
            Layout.bottomMargin: ScreenTools.defaultFontPixelHeight * .5

            ColumnLayout {
                Layout.fillWidth: true
                RowLayout {
                    QGCLabel {
                        text: qsTr("Target Coordinates: ") + _targetCoordinates
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        Layout.fillWidth: false
                    }
                    QGCColoredImage {
                        Layout.alignment: Qt.AlignLeft
                        source: "/res/content_copy.svg"
                        mipmap: true
                        width: ScreenTools.defaultFontPixelHeight
                        height: width
                        sourceSize.width: width
                        color: qgcPal.text
                        fillMode: Image.PreserveAspectFit
                        QGCMouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                textEdit.text = _targetCoordinates
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
                QGCLabel {
                    text: qsTr("Target Altitude: ") + _targetAltitude
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
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
                    QGCLabel {
                        text: _nvRecording ? qsTr(
                                                 "Recording to SD: YES") : qsTr(
                                                 "Recording to SD: NO")
                        id: recIndicator
                        color: _nvRecording ? qgcPal.colorRed : qgcPal.text
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                QGCLabel {
                    text: qsTr("Slant Range: ") + _slantRange
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }

                QGCLabel {
                    text: qsTr("Azimuth: ") + _azimuth
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }
                QGCLabel {
                    text: qsTr("Pitch: ") + _pitch
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
                    Layout.fillWidth: true
                }
            }
        }

        // Details Panel (vehicle lat/lon, cursor lat/lon)
        GridLayout {

            id: detailGrid
            visible: false
            Layout.fillWidth: true
            rowSpacing: ScreenTools.defaultFontPixelHeight
            columnSpacing: ScreenTools.defaultFontPixelWidth * 2
            rows: 3
            columns: 1
            Layout.bottomMargin: ScreenTools.defaultFontPixelHeight * .5

            ColumnLayout {
                Layout.fillWidth: true
                RowLayout {
                    QGCLabel {
                        text: qsTr("Vehicle Coordinates: ") + _vehicleCoordinate
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        Layout.fillWidth: false
                    }
                    QGCColoredImage {
                        Layout.alignment: Qt.AlignLeft
                        source: "/res/content_copy.svg"
                        mipmap: true
                        width: ScreenTools.defaultFontPixelHeight
                        height: width
                        sourceSize.width: width
                        color: qgcPal.text
                        fillMode: Image.PreserveAspectFit
                        QGCMouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                textEdit3.text = _vehicleCoordinate
                                textEdit3.selectAll()
                                textEdit3.copy()
                            }
                            TextEdit {
                                id: textEdit3
                                visible: false
                            }
                        }
                    }
                }
                RowLayout {
                    QGCLabel {
                        text: qsTr("Cursor Coordinates: ")
                              + (mouseCursor ? QGroundControl.unitsConversion.appSettingsGeoCoordinateToString(
                                                   mouseCursor,
                                                   7) : QGroundControl.unitsConversion.appSettingsGeoCoordinateToString(
                                                   ))
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        Layout.fillWidth: true
                    }
                }
                RowLayout {

                    QGCLabel {
                        text: qsTr("Vector from Cursor: ")
                              + ((mouseCursor
                                  && _activeVehicle) ? mouseCursor.distanceTo(
                                                           _activeVehicle.coordinate).toFixed(
                                                           1) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString + "∠" + mouseCursor.azimuthTo(
                                                           _activeVehicle.coordinate).toFixed(
                                                           1) + " °" : "")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        Layout.fillWidth: true
                    }
                }
            }
        }

        //Super Volo Telemetry Value Panel
        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelWidth

            id: superVoloTelemGrid
            RowLayout {
                spacing: ScreenTools.defaultFontPixelWidth * 2
                Layout.alignment: Qt.AlignHCenter
                RowLayout {
                    //air speed
                    QGCColoredImage {
                        source:  "/InstrumentValueIcons/speedometer.svg"
                        mipmap: true
                        height: ScreenTools.defaultFontPixelHeight * 1.5
                        width: ScreenTools.defaultFontPixelHeight * 1.5
                        color: qgcPal.text
                    }
                    QGCLabel {
                        text: "Air:"
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        color: qgcPal.text
                    }
                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.airSpeed.value.toFixed(0) : "--"
                        color: qgcPal.text
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle ? QGroundControl.unitsConversion.appSettingsSpeedUnitsString : ""
                        color: qgcPal.text
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                }
                RowLayout {
                    //ground speed
                    QGCColoredImage {
                        source:  "/InstrumentValueIcons/speedometer.svg"
                        mipmap: true
                        height: ScreenTools.defaultFontPixelHeight * 1.5
                        width: ScreenTools.defaultFontPixelHeight * 1.5
                        color: qgcPal.text
                    }
                    QGCLabel {
                        text: "Ground:"
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        color: qgcPal.text
                    }
                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.groundSpeed.value.toFixed(0) : "--"
                        color: qgcPal.text
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle ? QGroundControl.unitsConversion.appSettingsSpeedUnitsString : ""
                        color: qgcPal.text
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                }

                RowLayout {
                    //wind
                    property var _windDirection: _activeVehicle ? _activeVehicle.wind.direction.value.toFixed(
                                                                      0) : 0
                    property var _windSpeed: _activeVehicle ? _activeVehicle.wind.speed.value.toFixed(
                                                                  0) : 0
                    property var _windUnits: _activeVehicle ? _activeVehicle.wind.speed.units : ""

                    QGCLabel {
                        text: "Wind:"
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.mediumFontPointSize
                        color: qgcPal.text
                    }
                    QGCColoredImage {
                        id: windIcon
                        source: "/res/wind-arrow.svg"
                        mipmap: true
                        height: ScreenTools.defaultFontPixelHeight * 1.5
                        width: ScreenTools.defaultFontPixelHeight * 1.5
                        color: qgcPal.text

                        transform: Rotation {
                            origin.x: windIcon.width / 2
                            origin.y: windIcon.height / 2
                            angle: _windDirection
                        }
                    }

                    QGCLabel {
                        text: _windSpeed
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                        visible: _activeVehicle ? (isNaN(
                                                       _windSpeed) ? false : true) : false
                    }
                    QGCLabel {
                        text: _windUnits
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.smallFontPointSize
                        visible: _activeVehicle ? (isNaN(
                                                       _windSpeed) ? false : true) : false
                    }
                }
            }

            RowLayout {
                spacing: ScreenTools.defaultFontPixelWidth * 2

                RowLayout {
                    Image {
                        source: "/res/distance-to-wp.svg"
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        sourceSize.height: ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.missionItemIndex.value : qsTr(
                                                   "-", "No data to display")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: qsTr("|")
                        font.pointSize: ScreenTools.largeFontPointSize
                    }

                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.distanceToNextWP.value.toFixed(
                                                   0) : qsTr(
                                                   "--", "No data to display")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.distanceToNextWP.units : qsTr(
                                                   "")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: qsTr("    ")
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                }

                RowLayout {
                    Image {
                        source: "/res/distance-to-home.svg" //todo make distance to wp icon
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        sourceSize.height: ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel {
                        text: _activeVehicle && !isNaN(
                                  _activeVehicle.distanceToHome.rawValue) ? _activeVehicle.distanceToHome.value.toFixed(0) : qsTr("--", "No data to display")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.distanceToHome.units : qsTr(
                                                   "")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle && !isNaN(
                                  _activeVehicle.bearingFromHome.rawValue) ? "∠" + _activeVehicle.bearingFromHome.rawValue.toFixed(0) + "°" : qsTr("--", "No data to display")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.bearingFromHome.units : qsTr(
                                                   "")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                    QGCLabel {
                        text: qsTr("    ")
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                }

                RowLayout {
                    Image {
                        source: "/res/flight_timer.svg"
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        sourceSize.height: ScreenTools.largeFontPointSize * 1.5
                    }

                    QGCLabel {
                        text: _activeVehicle ? _activeVehicle.flightTime.valueString : qsTr(
                                                   "--:--:--",
                                                   "No data to display")
                        font.family: ScreenTools.demiboldFontFamily
                        font.pointSize: ScreenTools.largeFontPointSize
                    }
                }

                RowLayout {
                    QGCColoredImage {
                        source: "/res/menu.svg"
                        mipmap: true
                        width: ScreenTools.defaultFontPixelHeight
                        height: width
                        sourceSize.width: width
                        color: qgcPal.text
                        fillMode: Image.PreserveAspectFit
                        QGCMouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                toggleDetails()
                            }
                        }
                    }
                }
            }
        }
        //Set to false, so we replace this with our own custom telemetry output above
        HorizontalFactValueGrid {
            id: valueArea
            userSettingsGroup: telemetryBarUserSettingsGroup
            defaultSettingsGroup: telemetryBarDefaultSettingsGroup
            visible: false //true
        }

        GuidedActionConfirm {
            opacity: 1.0
            Layout.fillWidth: true
            guidedController: _guidedController
            altitudeSlider: _guidedAltSlider
        }
    }
}
