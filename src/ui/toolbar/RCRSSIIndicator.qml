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
//-- RC RSSI Indicator
Item {
    id:             _root
    width:          rssiRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: _activeVehicle.supportsRadio

    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property var    _vehicleManager:     QGroundControl.multiVehicleManager
    property bool   _rcRSSIAvailable:   true //activeVehicle ? activeVehicle.mavlinkLossPercent >= 0 && activeVehicle.mavlinkLossPercent <= 100 : false
    //property bool   _rcRSSIAvailable:   activeVehicle ? activeVehicle.rcRSSI > 0 && activeVehicle.rcRSSI <= 100 : false

    function getRssiColor() {
        if(_vehicleManager.doodleRSSI.length > 0) {
            if(_vehicleManager.doodleRSSIMax > 50) {
                return qgcPal.text
            }
            if(_vehicleManager.doodleRSSIMax > 35) {
                return qgcPal.colorOrange
            }
            if(_vehicleManager.doodleRSSIMax >= 0) {
                return qgcPal.colorRed
            }

        }
        return qgcPal.text
    }

    Component {
        id: rcRSSIInfo

        Rectangle {
            width:  rcrssiCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: rcrssiCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 rcrssiCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                //width:              Math.max(rcrssiGrid.width, rssiLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             rssiLabel
                    text:           activeVehicle ? (_vehicleManager.doodleRSSI.length >= 1 ? qsTr("Doodle RSSI Status") : qsTr("Mavlink Packet Loss Ratio")) : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    id: mavPacket
                    visible: (_vehicleManager.doodleRSSI.length === 0) ? true : false
                    QGCLabel { text: activeVehicle ? (activeVehicle.mavlinkLossPercent.toFixed(0) + "% Lost ") : 0 }
                }

                Row {
                    id: singleRssi
                    visible: (_vehicleManager.doodleRSSI.length === 1) ? true : false

                    //QGCLabel { text: activeVehicle ? (100 - activeVehicle.mavlinkLossPercent.toFixed(0) + "%") : 0 }
                    QGCLabel { text : (_vehicleManager.doodleRSSI.length === 1) ? _vehicleManager.doodleRSSI[0].qmac + ", " : "" }
                    QGCLabel { text : (_vehicleManager.doodleRSSI.length === 1) ? _vehicleManager.doodleRSSI[0].qsignal + " dBm " : ""}
                    QGCLabel { text : (_vehicleManager.doodleRSSI.length === 1) ? "(" +_vehicleManager.doodleRSSI[0].qpercentage + "%)" : "" }

                }

                    Repeater {
                        id:     rssiList
                        model:  _vehicleManager.doodleRSSI


                        Row {
                            visible: (_vehicleManager.doodleRSSI.length > 1) ? true : false
                            QGCLabel { text : modelData.qmac + ", " }
                            QGCLabel { text : modelData.qsignal + " dBm " }
                            QGCLabel { text : "(" +modelData.qpercentage + "%)" }


                        }
                    }
                   // QGCLabel { text: qsTr("RSSI:") }
                    //QGCLabel { text: activeVehicle ? (100 - activeVehicle.mavlinkLossPercent.toFixed(0) + "%") : 0 }
                  //  QGCLabel { text : _activeVehicle ? _activeVehicle.doodleRSSI[0].qsignal : 0 }

            }
        }
    }

    Row {
        id:             rssiRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth

        /*QGCColoredImage {
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceSize.height:  height
            source:             "/qmlimages/RC.svg"
            fillMode:           Image.PreserveAspectFit
            opacity:            _rcRSSIAvailable ? 1 : 0.5
            color:              qgcPal.buttonText
        }
        */

        SignalStrength {
            anchors.verticalCenter: parent.verticalCenter
            size:                   parent.height * 0.5
            percent:                _activeVehicle ? ((_vehicleManager.doodleRSSIMax != -255) ? _vehicleManager.doodleRSSIMax : (100 - activeVehicle.mavlinkLossPercent.toFixed(0))) : 0

        }
        QGCLabel {
            id:             rssiLabel2
            text:           _activeVehicle ? ((_vehicleManager.doodleRSSIMax != -255) ? _vehicleManager.doodleRSSIMax : (100 - activeVehicle.mavlinkLossPercent.toFixed(0))) + "%" : 0
            color:          getRssiColor()
            font.pointSize:         ScreenTools.mediumFontPointSize
            anchors.verticalCenter: parent.verticalCenter
        }


    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showPopUp(_root, rcRSSIInfo)
        }
    }
}
