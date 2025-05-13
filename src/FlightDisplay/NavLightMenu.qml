
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
    id: messageDialogComponent
    QGCPopupDialog {
        id: navLightPopup
        title: "Nav Lights"
        buttons:  StandardButton.Close
        ColumnLayout{
            RowLayout {
                QGCColumnButton {

                    id: onButton
                    text:                   "On"
                    iconSource:             "/InstrumentValueIcons/light-bulb.svg"
                    pointSize:              ScreenTools.defaultFontPointSize * 3.5
                    backRadius:             width / 40
                    heightFactor:           0.75
                    Layout.preferredHeight: offButton.height
                    Layout.preferredWidth:  offButton.width
                    iconColor:  qgcPal.colorGreen
                    onClicked: {
                        _activeVehicle.sendNavLightAction(Vehicle.NavLight_On)
                        navLightPopup.hideDialog()
                    }
                    Timer{
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered:{
                            if(onButton.iconColor === qgcPal.colorGreen)
                            {
                                onButton.iconColor = qgcPal.colorRed
                            }
                            else
                            {
                                onButton.iconColor = qgcPal.colorGreen
                            }
                        }
                    }
                }
                QGCColumnButton {
                    id: offButton
                    text:                   "Off"
                    iconSource:             "/InstrumentValueIcons/light-bulb.svg"
                    pointSize:              ScreenTools.defaultFontPointSize * 3.5
                    backRadius:             width / 40
                    heightFactor:           0.75
                    Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 27
                    Layout.preferredHeight: Layout.preferredWidth / 1.20
                    iconColor:  "gray"
                    onClicked: {
                        _activeVehicle.sendNavLightAction(Vehicle.NavLight_Off)
                        navLightPopup.hideDialog()
                    }
                }
            }
            RowLayout {
                visible: false //DEBUG ONLY
                QGCTextField {
                    id:               debugRC7TextField
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator:              IntValidator {bottom: 0; top: 9999;}
                }
                QGCButton {
                    id:        sendButton
                    text:      qsTr("Send RC7")
                    onClicked: {
                        _activeVehicle.rc7 = debugRC7TextField.text
                    }
                }
            }


        }
    }
}
