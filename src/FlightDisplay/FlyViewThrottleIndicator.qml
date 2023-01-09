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
    id:     throttleInd
    anchors.horizontalCenter: parent.horizontalCenter

    property var _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var _throttle: _activeVehicle ? _activeVehicle.throttlePct : 0
    property real   _margins:               ScreenTools.defaultFontPixelWidth / 2
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75


    Rectangle {
        id:                 throttlePanel
        height:             throttleLayout.height + (_toolsMargin * 2)
        width:              throttleLayout.width + (_toolsMargin * 2)
        color:              qgcPal.window
        radius:             ScreenTools.defaultFontPixelWidth / 2
        anchors.horizontalCenter: parent.horizontalCenter

        property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

        DeadMouseArea { anchors.fill: parent }

        ColumnLayout {
            id:                 throttleLayout
            width:              ScreenTools.defaultFontPixelWidth * 20
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                Layout.alignment:   Qt.AlignHCenter
                Layout.topMargin:   _toolsMargin
                ProgressBar {
                    id:                 throttleIndicator
                    to:                 100
                    height:             ScreenTools.defaultFontPixelHeight * 0.4
                    width:              parent.width
                    value:               _activeVehicle ? _activeVehicle.throttlePct.rawValue : 0
                    Layout.fillWidth:   true
                    Layout.alignment:   Qt.AlignHCenter | Qt.AlignVCenter
                    //palette.dark:       qgcPal.colorBlue

                    background: Rectangle {
                                                    anchors.fill: throttleIndicator
                                                    color: qgcPal.colorGrey
                                                    radius: ScreenTools.defaultFontPixelWidth / 2
                                                    border.width: 1
                                                    border.color: qgcPal.colorGrey
                                                }
                                                contentItem: Rectangle {
                                                    anchors.left: throttleIndicator.left
                                                    anchors.bottom: throttleIndicator.bottom
                                                    height: throttleIndicator.height
                                                    width: throttleIndicator.width * (throttleIndicator.value / 100)
                                                    color: throttleIndicator.value === 0.0 ? qgcPal.colorGrey : getColor()
                                                    radius: ScreenTools.defaultFontPixelWidth / 2
                                                    function getColor()
                                                    {
                                                        if (_activeVehicle.throttlePct.value <= 30)
                                                        {
                                                            return qgcPal.colorRed
                                                        }
                                                        else
                                                        {
                                                            return qgcPal.colorBlue
                                                        }
                                                    }
                                                }


                }
            }

             RowLayout {
                Layout.alignment:   Qt.AlignHCenter
                QGCLabel { text: _activeVehicle ? qsTr("Throttle: ") + _activeVehicle.throttlePct.value + "%" : qsTr("", "No data to display")
                    font.family:        ScreenTools.demiboldFontFamily
                    font.pointSize:     ScreenTools.mediumFontPointSize
                }
             }

        }
    }

}
