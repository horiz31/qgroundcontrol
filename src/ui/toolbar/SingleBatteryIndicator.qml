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
//-- Single Battery Indicator for the SuperVolo.  data mostly comes from sys_status, with used_capacity coming from battery-status
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          batteryIndicatorRow.width

    property bool showIndicator: true
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle  
    property var _batteryRemaining: _activeVehicle ? _activeVehicle.svBattPercentRemaining.value.toFixed(0) : undefined  
    property var battery: _activeVehicle && _activeVehicle.batteries.count > 0  ? _activeVehicle.batteries.get(0) : undefined  //the one containing current_consumed is battery 0
    property int _batteryCount: _activeVehicle ? _activeVehicle.batteries.count : 0

    on_BatteryCountChanged:
    {
        if (_activeVehicle.batteries.count > 0)
        {
            battery = _activeVehicle.batteries.get(0)
        }
    }

    Component {
        id: svBatteryInfo

        Rectangle {
            width:  svCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: svCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 svCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(svGrid.width, svBattLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             svBattLabel
                    text:           _activeVehicle ? qsTr("Battery Status") : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 svGrid
                    visible:            true
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    columns:            2
                    rows:               3
                    anchors.horizontalCenter: parent.horizontalCenter

                    QGCLabel { text: qsTr("Voltage:") }
                    QGCLabel { text: _activeVehicle ? (_activeVehicle.svBattVoltage.value.toFixed(1) + " V") : "--.- V" }
                    QGCLabel { text: qsTr("Current:") }
                    QGCLabel { text: _activeVehicle ? (_activeVehicle.svBattCurrent.value.toFixed(1) + " A") : "--.- A" }
                    QGCLabel { text: qsTr("Used Capacity:") }
                    QGCLabel { text: _activeVehicle ? (battery.mahConsumed ? battery.mahConsumed.value.toFixed(0) + " mA" : "---- mA") : "---- mA" }

                }
            }
        }
    }
    Row {
        id:             batteryIndicatorRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom


            QGCColoredImage {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                width:              height
                sourceSize.width:   width
                source:             "/qmlimages/Battery.svg"
                fillMode:           Image.PreserveAspectFit
                color:              getBatteryColor()
                function getBatteryColor() {
                    if (_batteryRemaining)
                    {
                        if (_batteryRemaining > 60)
                            return qgcPal.colorGreen
                        else if (_batteryRemaining > 50)
                            return qgcPal.colorOrange
                        else
                            return qgcPal.colorRed
                    }
                    else
                        return qgcPal.text
                }
            }

            QGCLabel {
                text:                   _batteryRemaining ? _batteryRemaining + " %" : "--- %";
                font.pointSize:         ScreenTools.mediumFontPointSize
                color:                  qgcPal.text
                anchors.verticalCenter: parent.verticalCenter


            }
        }
        MouseArea {
            anchors.fill:   parent
            onClicked: {
                mainWindow.showIndicatorPopup(_root, svBatteryInfo)
            }
        }
}


