import QtQuick                      2.12
import QtQuick.Layouts              1.12

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4

Item {
    id:     checklistWarningInd
    anchors.horizontalCenter: parent.horizontalCenter

    property var _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle   
    property real   _margins:               ScreenTools.defaultFontPixelWidth / 2
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75


    Rectangle {
        id:                 checklistPanel
        height:             ScreenTools.defaultFontPixelHeight * 1.5
        width:              ScreenTools.defaultFontPixelWidth * 40
        color:              qgcPal.colorOrange
        radius:             ScreenTools.defaultFontPixelWidth / 2
        anchors.horizontalCenter: parent.horizontalCenter

        property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

        QGCMouseArea {
            fillItem:   checklistPanel
            onClicked: {
                if (!_isCheckListWindowVisible && _checkListWindow)
                {
                    _checkListWindow.show()
                }
            }

        }
        PropertyAnimation on opacity {
            easing.type:    Easing.OutQuart
            from:           0.5
            to:             1
            loops:          Animation.Infinite
            running:        true
            alwaysRunToEnd: true
            duration:       2000
        }

        ColumnLayout {
            id:                 checklistLayout
            width:              ScreenTools.defaultFontPixelWidth * 40
            anchors.fill:       parent

            QGCLabel { text: _activeVehicle ? qsTr("Complete Checklist Prior to Takeoff") : qsTr("", "")

                font.family:        ScreenTools.demiboldFontFamily
                font.pointSize:     ScreenTools.mediumFontPointSize
                color:         qgcPal.buttonText
                verticalAlignment: Text.AlignVCenter
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
            }
        }
    }
}
