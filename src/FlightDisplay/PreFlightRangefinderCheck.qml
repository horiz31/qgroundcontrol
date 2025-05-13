import QtQuick 2.3

import QGroundControl           1.0
import QGroundControl.Controls  1.0
import QGroundControl.Vehicle   1.0

PreFlightCheckButton {
    name:                           qsTr("Rangefinder")
    telemetryFailure:               _rangefinderLow
    telemetryTextFailure:           qsTr("Warning - Rangefinder data is outside of the expected range and may not be functioning. Please inspect hardware or contact EchoMAV for support. It is safe to fly without this sensor. Click to acknowledge and proceed.")
    allowTelemetryFailureOverride:  true

    property real    failureValue:                 0
    property real    _rangefinderValue: globals.activeVehicle ? globals.activeVehicle.rangefinder.rawValue : 0 //TODO find a way to get this from the active Vehicle
    property bool    _rangefinderLow:   _rangefinderValue <= failureValue
}
