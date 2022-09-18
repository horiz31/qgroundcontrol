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
            name: qsTr("Super Volo Hardware Checks")

            PreFlightCheckButton {
                name:           qsTr("Check Props")
                manualText:     qsTr("Are all props securely mounted and free of damage?")
            }
            PreFlightCheckButton {
                name:           qsTr("Check Servo Linkages")
                manualText:     qsTr("Do they move freely, are the linkages free of damage?")
            }
            PreFlightCheckButton {
                name:           qsTr("Check Surfaces")
                manualText:     qsTr("Check wings and control surfaces for damage, are they ok?")
            }
        }

        PreFlightCheckGroup {
            name: qsTr("Super Volo Servo and Sensor Checks")

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

            PreFlightAirSpeedHealthCheck {
                allowTelemetryFailureOverride:    true
            }

        }

        PreFlightCheckGroup {
            name: qsTr("Servos, Motors and Engine Test")

            PreFlightCheckButton {
                name:            qsTr("Actuators")
                manualText:      qsTr("Move the aircraft and ensure that control surfaces react. Did they work properly?")
            }

            PreFlightICEHealthCheck {
            }

            PreFlightMultiRotorHealthCheck {
            }


        }

        PreFlightCheckGroup {
            name: qsTr("Last preparations before launch")

            // Check list item group 2 - Final checks before launch
            PreFlightMissionCheck {
                allowTelemetryFailureOverride:    true
            }

            PreFlightCheckButton {
                name:        qsTr("Payload")
                manualText:  qsTr("Verify the payload is operations, e.g. you are receiving video?")
            }

            PreFlightCheckButton {
                name:        "Wind and Weather"
                manualText:  qsTr("Are the winds aloft too high to safely operate?")
            }

            PreFlightCheckButton {
                name:        qsTr("Flight area")
                manualText:  qsTr("Is the launch area and path free of obstacles/people?")
            }
        }
    }
}

