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
import MAVLink                              1.0

//-------------------------------------------------------------------------
//-- Fuel Indicator for the SuperVolo.  I currently believe that the fuel is reported via battery 1, percentRemaining
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          batteryIndicatorRow.width

    property bool showIndicator: true
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var battery: _activeVehicle && _activeVehicle.batteries.count > 0  ? _activeVehicle.batteries.get(1) : undefined  //the fuel is the one with battery id = 1. I'm unsure if this is a zero index thing

    Row {
        id:             batteryIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom

        Row {
            anchors.top:    parent.top
            anchors.bottom: parent.bottom


            QGCColoredImage {
                width:              height * 1.1
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                sourceSize.height:  height * .7
                source:             "/res/fuel.svg"
                fillMode:           Image.PreserveAspectFit
                color:              getFuelColor()

                function getFuelColor() {
                    if(battery)
                    {
                        if (battery.percentRemaining.value <= 20)
                            return qgcPal.colorOrange
                        else if (battery.percentRemaining.value <= 10)
                            return qgcPal.colorRed
                        return qgcPal.colorGreen
                    }
                    else
                         return qgcPal.text
                }

            }


            QGCLabel {
                text:                   battery? battery.percentRemaining.valueString + "%" : "--%"
                font.pointSize:         ScreenTools.mediumFontPointSize
                color:                  qgcPal.text
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}


