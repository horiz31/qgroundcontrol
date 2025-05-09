
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
        RowLayout {
            QGCColumnButton {
                id: onButton
                text:                   "On"
                iconSource:             "/res/NavLightOn.svg"  //TODO get a more appropriate icon
                pointSize:              ScreenTools.defaultFontPointSize * 3.5
                backRadius:             width / 40
                heightFactor:           0.75
                Layout.preferredHeight: offButton.height
                Layout.preferredWidth:  offButton.width
                onClicked: {
                    _activeVehicle.sendNavLightAction(Vehicle.NavLight_On)
                    navLightPopup.hideDialog()
                }
            }
            QGCColumnButton {
                id: offButton
                text:                   "Off"
                iconSource:             "/res/NavLightOff.svg"  //TODO get a more appropriate icon
                pointSize:              ScreenTools.defaultFontPointSize * 3.5
                backRadius:             width / 40
                heightFactor:           0.75
                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 27
                Layout.preferredHeight: Layout.preferredWidth / 1.20
                onClicked: {
                    _activeVehicle.sendNavLightAction(Vehicle.NavLight_Off)
                    navLightPopup.hideDialog()
                }
            }
        }
    }
}
