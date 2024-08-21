/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQml.Models 2.12

import QGroundControl           1.0
import QGroundControl.Controls  1.0

ToolStripActionList {
    id: _root

    signal displayPreFlightChecklist
    signal clearFlightPath
    signal goFullScreen
    signal centerMap
    signal clearMeasure


    model: [
        ToolStripAction {
            text:           qsTr("Plan")
            iconSource:     "/qmlimages/Plan.svg"
            onTriggered:    mainWindow.showPlanView()
            enabled:        _activeVehicle && _activeVehicle.parameterManager.parametersReady
        },
        PreFlightCheckListShowAction
        {
            onTriggered: displayPreFlightChecklist()
        },
        ClearFlightTrail {
            onTriggered:  clearFlightPath()
        },
        ToolStripAction {
            text:           qsTr("Stop Meas.")
            iconSource:     "/qmlimages/clearmeasure.svg"
            visible:        _showDistanceToAircraft
            enabled:        _showDistanceToAircraft
            onTriggered: clearMeasure()
        },
        CenterMap {  onTriggered:  centerMap()  },
        FullScreen {
            //visible if decoding and video is current full screen mode
            visible:      QGroundControl.videoManager.decoding && videoControl.pipState.state === videoControl.pipState.fullState
            onTriggered:  goFullScreen()
        },
        GuidedActionTakeoff { },
        GuidedActionActionLandingList { },  //GuidedActionLand
        GuidedActionRTL { },
        GuidedActionPause { },
        GuidedActionActionList { }

    ]
}
