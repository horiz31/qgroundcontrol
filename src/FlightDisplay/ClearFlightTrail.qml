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
    text:           qsTr("Clear Path")
    iconSource:     "/qmlimages/clearpath.svg"
    visible:        true
    enabled:        true

    property var  _activeVehicle:   QGroundControl.multiVehicleManager.activeVehicle
}
