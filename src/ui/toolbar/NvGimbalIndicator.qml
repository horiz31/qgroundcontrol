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
                    rows:               8
                    anchors.horizontalCenter: parent.horizontalCenter

                    QGCLabel { text: qsTr("Mode:") }
                    QGCLabel { text: _activeVehicle? activeVehicle.nvGimbal.mode.value : "" }
                    QGCLabel { text: qsTr("Target Latitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLat.value) || (_activeVehicle.nvGimbal.groundCrossingLat.value === 400.0)) ? "--.------" : _activeVehicle.nvGimbal.groundCrossingLat.value.toFixed(7) + "°") : "--.-------°" }
                    QGCLabel { text: qsTr("Target Longitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingLon.value) || (_activeVehicle.nvGimbal.groundCrossingLon.value === 400.0)) ? "--.------" : _activeVehicle.nvGimbal.groundCrossingLon.value.toFixed(7) + "°") : "--.-------°" }
                    QGCLabel { text: qsTr("Target Altitude:") }
                    QGCLabel { text: _activeVehicle ? ((isNaN(_activeVehicle.nvGimbal.groundCrossingAlt.value) || (_activeVehicle.nvGimbal.groundCrossingAlt.value === 10000.0)) ? "----" : QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(_activeVehicle.nvGimbal.groundCrossingAlt.value).toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString) : "----" }
                    QGCLabel { text: qsTr("Slant Range:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.slantRange.value) ? "----" : QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(_activeVehicle.nvGimbal.slantRange.value).toFixed(0) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString) : "----" }
                    QGCLabel { text: qsTr("Generator:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.generator.value) ? "-- %" : _activeVehicle.hcu.generator.value.toFixed(0) + "%") : "-- %" }
                    QGCLabel { text: qsTr("Charge Rate:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.chargeRate.value) ? "-.-- A" : _activeVehicle.hcu.chargeRate.value.toFixed(2) + " A") : "-.-- A" }
                    QGCLabel { text: qsTr("Engine Health:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.engineHealth.value) ? "-- %" : _activeVehicle.hcu.engineHealth.value.toFixed(0) + "%") : "-- %" }
                }
            }
        }
    }

    Row {
        id:             gimbalRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth



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

                if ((_activeVehicle.nvGimbal.cpuTemperature.value > 70) || (_activeVehicle.nvGimbal.cameraTemperature > 70))
                    return qgcPal.colorRed               
                else
                    return qgcPal.colorGreen
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
        onClicked: {
            mainWindow.showIndicatorPopup(_root, gimbalInfo)
        }
    }
}
