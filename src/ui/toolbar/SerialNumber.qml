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
//-- Simple Serial Number Indicator
Item {
    id:             _root
    width:          gimbalRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle


    Row {
        id:             gimbalRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth
        visible:        _activeVehicle


         QGCLabel {             
             text: _activeVehicle ? getName() : ""
             font.pointSize:     ScreenTools.largeFontPointSize
             font.family:        ScreenTools.normalFontFamily
             anchors.verticalCenter: parent.verticalCenter
             function getName(){
                if (_activeVehicle.vehicleModel === 0)
                {
                    if (_activeVehicle.brdSerialNumber !== 0)
                    {
                      return " Vehicle " + _activeVehicle.brdSerialNumber + " "
                    }
                    else
                    {
                       return "";
                    }
                }
                else
                {
                    if (_activeVehicle.brdSerialNumber !== 0)
                    {
                        if (_activeVehicle.vehicleModel === 0)
                            return " Vehicle " + _activeVehicle.brdSerialNumber + " "
                        else if (_activeVehicle.vehicleModel === 1)
                            return " Maven " + _activeVehicle.brdSerialNumber + " "
                    }
                    else
                    {
                       return " Maven ";
                    }
                }
            }
         }
    }

}
