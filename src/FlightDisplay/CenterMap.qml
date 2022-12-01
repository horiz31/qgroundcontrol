/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QGroundControl           1.0
import QGroundControl.Controls  1.0

ToolStripAction {
    text:           qsTr("Center Map")
    iconSource:      "/qmlimages/MapCenter.svg"
    visible:        true
    enabled:        _activeVehicle ? true : false

    property var  _activeVehicle:   QGroundControl.multiVehicleManager.activeVehicle
}
