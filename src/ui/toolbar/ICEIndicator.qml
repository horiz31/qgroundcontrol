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
//-- Internal Combustion Engine (ICE) Indicator for SuperVolo
Item {
    id:             _root
    width:          iceRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle


    Component {
        id: iceInfo

        Rectangle {
            width:  iceCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: iceCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 iceCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(iceGrid.width, iceLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             iceLabel
                    text:           _activeVehicle ? qsTr("Engine Status") : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 iceGrid
                    visible:            true
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    columns:            2
                    rows:               7
                    anchors.horizontalCenter: parent.horizontalCenter

                    QGCLabel { text: qsTr("Throttle:") }

                    //(isNaN(_activeVehicle.throttlePct.value) ? "-- %" : _activeVehicle.throttlePct.value.toFixed(0) + "%")
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.throttlePct.value) ? "-- %" : _activeVehicle.throttlePct.value.toFixed(0) + " %") : "-- %" }
                    QGCLabel { text: qsTr("E-Thrust:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.eThrust.value) ? "-- %" : _activeVehicle.hcu.eThrust.value.toFixed(0) + " %") : "-- %" }
                    QGCLabel { text: qsTr("Carb Servo:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.carbPosition.value) ? "-- %" : _activeVehicle.hcu.carbPosition.value.toFixed(0) + " %") : "-- %" }
                    QGCLabel { text: qsTr("Cylinder Temp:") }
                    QGCLabel { text: _activeVehicle ? (isNaN(_activeVehicle.hcu.cylinderTemp.value) ? "---.- °F" : _activeVehicle.hcu.cylinderTemp.value.toFixed(1) + " F") : "---.- °F" }
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
        id:             iceRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth



        QGCColoredImage {
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceSize.height:  height
            source:             "/res/pistons.svg"
            fillMode:           Image.PreserveAspectFit
            opacity:            1
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

         QGCLabel {             
             text: _activeVehicle ? (isNaN(_activeVehicle.hcu.rpm.value) ? "----" : _activeVehicle.hcu.rpm.value.toFixed(0)) : qsTr("----")
             font.pointSize:     ScreenTools.mediumFontPointSize
             anchors.verticalCenter: parent.verticalCenter

         }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, iceInfo)
        }
    }
}
