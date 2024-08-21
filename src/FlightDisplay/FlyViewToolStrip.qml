/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQml.Models 2.12

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0

ToolStrip {
    id:     _root
    title:  qsTr("Fly")

    signal displayPreFlightChecklist
    signal clearFlightPath
    signal goFullScreen
    signal centerMap
    signal clearMeasure

    FlyViewToolStripActionList {
        id: flyViewToolStripActionList

        onDisplayPreFlightChecklist: _root.displayPreFlightChecklist()
        onClearFlightPath: _root.clearFlightPath()
        onGoFullScreen: _root.goFullScreen()
        onCenterMap:    _root.centerMap()
        onClearMeasure: _root.clearMeasure()
    }

    model: flyViewToolStripActionList.model
}
