/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- NextVision Gimbal Indicator
Item {
    id:             _root
    width:          gimbalRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle

    Component {
        id: gimbalInfo

        Rectangle {
            width:  gimbalCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: gimbalCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 gimbalCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(gimbalGrid.width, gimbalLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             gimbalLabel
                    text:           _activeVehicle ? qsTr("Gimbal Status") : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 gimbalGrid
                    visible:            true
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    columns:            2
                    rows:               13
                    anchors.horizontalCenter: parent.horizontalCenter

                    QGCLabel { text: qsTr("Mode:") }
                    QGCLabel { text: _activeVehicle? _activeVehicle.nvGimbal.mode.value : "" }
                    QGCLabel { text: qsTr("Target Latitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLat.value) || (_activeVehicle.nvGimbal.groundCrossingLat.value === 400.0)) ? "--.-------°" : _activeVehicle.nvGimbal.groundCrossingLat.value.toFixed(7) + "°") : "--.-------°" }
                    QGCLabel { text: qsTr("Target Longitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLon.value) || (_activeVehicle.nvGimbal.groundCrossingLon.value === 400.0)) ? "--.-------°" : _activeVehicle.nvGimbal.groundCrossingLon.value.toFixed(7) + "°") : "--.-------°" }
                    QGCLabel { text: qsTr("Target Altitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingAlt.value) || (_activeVehicle.nvGimbal.groundCrossingAlt.value.toFixed(0) === 32808.0)) ? "----" : _activeVehicle.nvGimbal.groundCrossingAlt.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString) : "----" }
                    QGCLabel { text: qsTr("Slant Range:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.slantRange.value) ? "----" : _activeVehicle.nvGimbal.slantRange.value.toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString) : "----" }
                    QGCLabel { text: qsTr("Field of View:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.fov.value) ? "--°" : _activeVehicle.nvGimbal.fov.value.toFixed(0) + "°") : "--°" }
                    QGCLabel { text: qsTr("Active Sensor:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.activeSensor.value) ? "" : getActiveSensor()) : ""
                        function getActiveSensor() {
                            if (_activeVehicle.nvGimbal.activeSensor.value === 0)
                                return "Day"
                            else
                                return "IR"
                        }
                    }
                    QGCLabel { text: qsTr("SnapShot Status:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.isSnapshot.value) ? "" : getSnapShotStatus()) : ""
                        function getSnapShotStatus()
                        {
                            if (_activeVehicle.nvGimbal.isSnapshot.value === 0)
                                return "Idle"
                            else
                                return "Busy"
                        }
                    }
                    QGCLabel { text: qsTr("Recording Status:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.isRecording.value) ? "" : getRecordingStatus()) : ""
                        function getRecordingStatus()
                        {
                            if (_activeVehicle.nvGimbal.isRecording.value === 0)
                                return "No"
                            else if (_activeVehicle.nvGimbal.isRecording.value === 1)
                                return "Yes"
                            else
                                return "Disabled"
                        }
                    }
                    QGCLabel { text: qsTr("CPU Temperature:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.cpuTemperature.value) ? "-- " + QGroundControl.unitsConversion.appSettingsTemperatureUnitsString : _activeVehicle.nvGimbal.cpuTemperature.value.toFixed(0) + " °" + QGroundControl.unitsConversion.appSettingsTemperatureUnitsString) : "--" }
                    QGCLabel { text: qsTr("Camera Temperature:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.cameraTemperature.value) ? "-- " + QGroundControl.unitsConversion.appSettingsTemperatureUnitsString : _activeVehicle.nvGimbal.cameraTemperature.value.toFixed(0) + " °" + QGroundControl.unitsConversion.appSettingsTemperatureUnitsString) : "--" }
                    QGCLabel { text: qsTr("Camera Version:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? "" : _activeVehicle.nvGimbal.nvVersion.value.toFixed(3)) : "" }
                    QGCLabel { text: qsTr("TRIP Version:") }
                    QGCLabel { text: _activeVehicle ? _activeVehicle.nvGimbal.nvTripVersion.value : "" }
                    QGCLabel { text: qsTr("SD Card") }
                    QGCLabel { text: _activeVehicle ? getSDCardStatus() : ""
                        function getSDCardStatus()
                        {

                            if (!isNaN(_activeVehicle.nvGimbal.sdCapacity.value) && _activeVehicle.nvGimbal.sdCapacity.value !== 0)
                            {
                                var _sdPercentage = (((_activeVehicle.nvGimbal.sdCapacity.value - _activeVehicle.nvGimbal.sdAvailable.value) / _activeVehicle.nvGimbal.sdCapacity.value) * 100).toFixed(0)
                                return _sdPercentage + "% Full" + " (" + (_activeVehicle.nvGimbal.sdCapacity.value / 1024).toFixed(0) + " GiB)"
                            }
                            else
                            {                                
                                return "Not Present"
                            }
                        }
                    }
                }
            }
        }
    }

    Row {
        id:             gimbalRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth
        visible:        _activeVehicle ? (!isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? true : false) : false



        QGCColoredImage {
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceSize.height:  height
            source:             "/res/gimbal.svg"
            fillMode:           Image.PreserveAspectFit
            opacity:            1
            color:              getGimbalColor()

            function getGimbalColor() {
                if (!_activeVehicle)
                     return qgcPal.text

                var _sdPercentage = (((_activeVehicle.nvGimbal.sdCapacity.value - _activeVehicle.nvGimbal.sdAvailable.value) / _activeVehicle.nvGimbal.sdCapacity.value) * 100).toFixed(0)
                if ((_activeVehicle.nvGimbal.cpuTemperature.rawValue > 75) || (_activeVehicle.nvGimbal.cameraTemperature.rawValue > 75) || (_sdPercentage > 98))
                     return qgcPal.colorRed
                else if ((_activeVehicle.nvGimbal.cpuTemperature.rawValue > 65) || (_activeVehicle.nvGimbal.cameraTemperature.rawValue > 65) || (_sdPercentage > 90))
                    return qgcPal.colorYellow
                else
                    return qgcPal.text
            }

        }

         QGCLabel {             
             text: _activeVehicle ? _activeVehicle.nvGimbal.mode.value : ""
             font.pointSize:     ScreenTools.mediumFontPointSize
             anchors.verticalCenter: parent.verticalCenter

         }
    }

    MouseArea {
        anchors.fill:   parent
        enabled: _activeVehicle ? (!isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? true : false) : false
        onClicked: {
            mainWindow.showIndicatorPopup(_root, gimbalInfo)
        }
    }
}
