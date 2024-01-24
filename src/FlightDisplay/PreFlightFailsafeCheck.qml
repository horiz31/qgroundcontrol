/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick 2.3

import QGroundControl           1.0
import QGroundControl.Controls  1.0
import QGroundControl.Vehicle   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls    1.0

PreFlightCheckButton {    
    name:               qsTr("Failsafe Check")
    telemetryFailure:   gcsFailSafeOutofBounds()   //this causes the button to go yellow/red
    allowTelemetryFailureOverride: true

    property var    _activeVehicle:         globals.activeVehicle

    FactPanelController { id: controller; }

    function gcsFailSafeOutofBounds()
    {
        var gcsFailsafeFact = controller.getParameterFact(-1, "FS_GCS_ENABL");  //should be 1 for HB
        var gcsFailsafeTimeFact = controller.getParameterFact(-1, "FS_LONG_TIMEOUT");  //should be < 30s
        //console.log("failsafe value is " + gcsFailsafeTimeFact.value)
        if (gcsFailsafeFact.value === 1 && gcsFailsafeTimeFact.value <= 30)
               return false

        return true
    }


    Component.onCompleted: updateTelemetryTextFailure()

    function updateTelemetryTextFailure() {      
        if(gcsFailSafeOutofBounds())
        {
            telemetryTextFailure = qsTr("Failure. The Loss of Comms Failsafe is disabled or set above 30 seconds.")
        }

    }
}

