import QtQuick                  2.3
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.4
import QtQuick.Dialogs          1.2
import QtLocation               5.3
import QtPositioning            5.3
import QtQuick.Layouts          1.2

import QGroundControl                           1.0
import QGroundControl.ScreenTools               1.0
import QGroundControl.Controls                  1.0
import QGroundControl.Palette                   1.0
import QGroundControl.Vehicle                   1.0
import QGroundControl.FlightMap                 1.0

Component {
    id: airSpeedDialogComponent

    QGCPopupDialog {
         property var vehicle:  globals.activeVehicle
        title: qsTr("Change Target Airspeed")
        buttons: StandardButton.Close
        ColumnLayout {
            id: airSpeedCol
            Layout.fillWidth: true
            GridLayout {
                columnSpacing: ScreenTools.defaultFontPixelWidth * 2
                columns: 3
                QGCLabel {
                    text: qsTr("Target Airspeed:")
                }
                QGCTextField {
                    id: targetAirspeed
                    inputMethodHints: Qt.ImhDigitsOnly
                    text: vehicle ? getCurrentAirSpeed() : ""
                    enabled: true

                    function getCurrentAirSpeed() {
                        console.log("target airspeed is "
                                    + vehicle.targetAirSpeedSetPoint.value)
                        return QGroundControl.unitsConversion.metersPerSecondToAppSettingsSpeedUnits(
                                    vehicle.targetAirSpeedSetPoint.value).toFixed(
                                    0)
                    }
                }
                QGCLabel {
                    text: QGroundControl.unitsConversion.appSettingsSpeedUnitsString
                    enabled: true
                }
            }

            Item {
                width: 1
                height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
            }
            QGCSlider {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                maximumValue: QGroundControl.unitsConversion.metersPerSecondToAppSettingsSpeedUnits(
                                  33.5).toFixed(0)
                minimumValue: QGroundControl.unitsConversion.metersPerSecondToAppSettingsSpeedUnits(
                                  22.35).toFixed(0)
                value: vehicle ? QGroundControl.unitsConversion.metersPerSecondToAppSettingsSpeedUnits(
                                     vehicle.targetAirSpeedSetPoint.value).toFixed(
                                     0) : 0
                updateValueWhileDragging: true
                visible: true
                onValueChanged: targetAirspeed.text = value.toFixed(0)
            }
            Item {
                width: 1
                height: Math.round(ScreenTools.defaultFontPixelHeight * .5)
            }
            GridLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                columnSpacing: ScreenTools.defaultFontPixelWidth * 2
                columns: 2

                QGCButton {
                    text: qsTr("Cancel")
                    onClicked: {
                        hideDialog()
                    }
                }
                QGCButton {
                    function testEnabled() {
                        if (vehicle) {
                            var temp = QGroundControl.unitsConversion.appSettingsSpeedUnitsToMetersPerSecond(
                                        targetAirspeed.text)
                            //convert to m/s first
                            if (temp >= 20 && temp <= 33.6)
                                return true
                            return false
                        }
                        return false
                    }
                    enabled: testEnabled()
                    text: qsTr("Apply")
                    onClicked: {
                        console.log("sending speed " + QGroundControl.unitsConversion.appSettingsSpeedUnitsToMetersPerSecond(
                                        targetAirspeed.text))
                        vehicle.setAirSpeed(
                                    QGroundControl.unitsConversion.appSettingsSpeedUnitsToMetersPerSecond(
                                        targetAirspeed.text)) //this should be sent in m/s
                        hideDialog()
                    }
                }
            }
        }
    }
}
