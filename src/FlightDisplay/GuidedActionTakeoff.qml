/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QGroundControl.FlightDisplay 1.0

GuidedToolStripAction {
    text:       _guidedController.takeoffTitle
    iconSource: "/res/takeoff.svg"
    visible:    (_guidedController.showTakeoff || !_guidedController.showLand) && _guidedController.showActionList
    enabled:    true //_guidedController.showTakeoff
    actionID:   _guidedController.actionStartMission //_guidedController.actionTakeoff //sv modification, sv does not support NAV_CMD_TAKEOFF, so I have modified this to start the mission and ensure it is only visible when a mission is loaded
}


