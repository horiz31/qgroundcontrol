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
    property bool   _rcRSSIAvailable:   true //activeVehicle ? activeVehicle.mavlinkLossPercent >= 0 && activeVehicle.mavlinkLossPercent <= 100 : false
    //property bool   _rcRSSIAvailable:   activeVehicle ? activeVehicle.rcRSSI > 0 && activeVehicle.rcRSSI <= 100 : false

    function getRssiMaxColor() {
        if(_activeVehicle.MPU5RSSI.length > 0) {
            if(_activeVehicle.MPU5RSSIMax > 50) {
                return qgcPal.text
            }
            if(_activeVehicle.MPU5RSSIMax > 35) {
                return qgcPal.colorOrange
            }
            if(_activeVehicle.MPU5RSSIMax >= 0) {
                return qgcPal.colorRed
            }

        }
        return qgcPal.text
    }

    function getRssiLabel() {

        if (!_activeVehicle)
            return 0

        if (_activeVehicle.MPU5RSSI.length > 1)  //a mesh detectected, return worst case neightbor
            return _activeVehicle.MPU5RSSIMin + "% (Mesh)"
        else if (_activeVehicle.MPU5RSSIMax != -255) //single MPU5 detected, return max which will be the single value in this case
            return _activeVehicle.MPU5RSSIMax + "%"
        else  //no MPU5 data, so return mavlink loss %
            return (100 - _activeVehicle.mavlinkLossPercent.toFixed(0)) + "%"

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
                    text:           _activeVehicle ? (_activeVehicle.MPU5RSSI.length >= 1 ? qsTr("MPU5 RSSI Status") : qsTr("Mavlink Packet Loss Ratio")) : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    id: mavPacket
                    visible: (_activeVehicle.MPU5RSSI.length === 0) ? true : false
                    QGCLabel { text: _activeVehicle ? (_activeVehicle.mavlinkLossPercent.toFixed(0) + "% Lost ") : 0 }
                }

                Row {
                    id: singleRssi
                    visible: (_activeVehicle.MPU5RSSI.length === 1) ? true : false

                    //QGCLabel { text: activeVehicle ? (100 - activeVehicle.mavlinkLossPercent.toFixed(0) + "%") : 0 }
                    QGCLabel { text : (_activeVehicle.MPU5RSSI.length === 1) ? _activeVehicle.MPU5RSSI[0].qmac + ", " : "" }
                    QGCLabel { text : (_activeVehicle.MPU5RSSI.length === 1) ? _activeVehicle.MPU5RSSI[0].qsignal + " dBm " : ""}
                    QGCLabel { text : (_activeVehicle.MPU5RSSI.length === 1) ? "(" +_activeVehicle.MPU5RSSI[0].qpercentage + "%)" : "" }

                }

                    Repeater {
                        id:     rssiList
                        model:  _activeVehicle.MPU5RSSI


                        Row {
                            visible: (_activeVehicle.MPU5RSSI.length > 1) ? true : false
                            QGCLabel { text : modelData.qmac + ", " }
                            QGCLabel { text : modelData.qsignal + " dBm " }
                            QGCLabel { text : "(" + modelData.qpercentage + "%)" }


                        }
                    }
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
            percent:                _activeVehicle ? ((_activeVehicle.MPU5RSSIMin != 255) ? _activeVehicle.MPU5RSSIMin : (100 - _activeVehicle.mavlinkLossPercent.toFixed(0))) : 0

        }
        QGCLabel {
            id:             rssiLabel2
            text:           getRssiLabel()
            color:          getRssiMaxColor()
            font.pointSize:         ScreenTools.mediumFontPointSize
            anchors.verticalCenter: parent.verticalCenter
        }


    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, rcRSSIInfo)
        }
    }
}
