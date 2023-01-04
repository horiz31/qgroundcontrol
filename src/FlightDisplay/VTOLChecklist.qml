/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtQml.Models                 2.1

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Vehicle       1.0

Item {
    property var model: listModel
    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property real   _heading:     _activeVehicle ? _activeVehicle.heading.rawValue.toFixed(0) : 0    
    PreFlightCheckModel {
        id:     listModel

        PreFlightCheckGroup {
            name: qsTr("Aircraft Assembly and Condition Checks")

            PreFlightCheckButton {
                name:        qsTr("Verify Assembly")
                manualText:  qsTr("Aircraft is fully assembled, 6x bolts are secured with torque limited T-handle?")
            }
            PreFlightCheckButton {
                name:        qsTr("Inspect VTOL Motors and Props")
                manualText:  qsTr("VTOL motors spin free, are securely mounted and without defects, verify the props are security mounted and without damage.")
            }
            PreFlightCheckButton {
                name:        qsTr("Inspect Control Surfaces")
                manualText:  qsTr("Inspect the wings and control surfaces for any damange or binding.")
            }
            PreFlightCheckButton {
                name:           qsTr("Check Servo Linkages")
                manualText:     qsTr("Do they move freely, are the linkages free of damage?")
            }

            PreFlightCheckButton {
                name:        qsTr("Engine Inspection")
                manualText:  qsTr("Check that the engine is securely mounted, there are no cracks in the motor/generator, and the prop is in good condition.")
            }
        }
        PreFlightCheckGroup {
            name: qsTr("Mission and Payload Checks")

            PreFlightMissionCheck {
                allowTelemetryFailureOverride:    true
            }

            PreFlightCheckButton {
                name:        qsTr("Payload")
                manualText:  qsTr("Verify the payload is operational, e.g. you are receiving video?")
            }

        }

        PreFlightCheckGroup {
            name: qsTr("Sensor Checks")

            PreFlightAirSpeedHealthCheck {
                allowTelemetryFailureOverride:    true
            }

            PreFlightBatteryCheck {
                failurePercent:                 50
                allowFailurePercentOverride:    true
            }

            PreFlightSensorsHealthCheck {
            }

            PreFlightGPSCheck {
                failureSatCount:        9
                allowOverrideSatCount:  true
            }
            PreFlightCheckButton {
                name:            qsTr("Compass")
                manualText:      qsTr("Is the aircraft heading (" + _heading + "°) correct?")
            }
            PreFlightIMUTempCheck {
            }

            PreFlightActuatorCheck {

            }

           // PreFlightCheckButton {
           //     name:            qsTr("Actuators")
           //     manualText:      qsTr("Pickup and move the aircraft and ensure that control surfaces react. Did they work properly?")
           // }
            PreFlightJoystickCheck {
                allowTelemetryFailureOverride:    false
            }

        }

        PreFlightCheckGroup {
            name: qsTr("Final Engine and Motor Test")

            PreFlightCheckButton {
                name:            qsTr("Position Aircraft")
                manualText:      qsTr("Move the aircraft to the desired launch location.")
            }
            PreFlightCheckButton {
                name:        qsTr("Ensrure safe flight area")
                manualText:  qsTr("Ensure the launch area and takeoff path free of obstacles and non-essential people.")
            }

            PreFlightMultiRotorHealthCheck {
            }
            PreFlightICEHealthCheck {
            }

        }

    }
}

