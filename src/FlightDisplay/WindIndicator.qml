import QtQuick 2.3

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0


Item {
    id:     windInd
    width:  ScreenTools.defaultFontPixelHeight * (5.5)
    height: ScreenTools.defaultFontPixelHeight * (5.5)
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins:    _toolsMargin

    property var _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var _windDirection: _activeVehicle ? _activeVehicle.wind.direction.value.toFixed(0) : 0
    property var _windSpeed: _activeVehicle ? _activeVehicle.wind.speed.value.toFixed(0) : 0
    property var _windUnits: _activeVehicle ? _activeVehicle.wind.speed.units : ""

    Rectangle {
        id:             borderRect
        anchors.fill:   parent
        radius:         width / 2
        color:          qgcPal.window
        border.color:   qgcPal.text
        border.width:   0

        Rectangle {
            id:             svgRect
            anchors.centerIn: parent;
            width:          parent.width * 0.5
            height:         parent.height * 0.5
            radius:         (width / 2)
            color:          qgcPal.window
            border.width:   0

            Image {
                id:                 windIcon
                source:             "/res/wind-arrow.svg"
                mipmap:             true
                anchors.centerIn:   svgRect
                anchors.fill:       svgRect

                transform: Rotation {
                    origin.x: svgRect.width / 2
                    origin.y: svgRect.height / 2
                    angle:  -_windDirection
                }
            }
        }
    }

    QGCLabel {
        text:               _windSpeed
        font.family:        ScreenTools.demiboldFontFamily
        color:              "black"
        font.pointSize:     ScreenTools.largeFontPointSize
        anchors.centerIn:   parent

    }

    QGCLabel {
        text:               _windUnits
        font.family:        ScreenTools.demiboldFontFamily
        color:              "white"
        font.pointSize:     ScreenTools.mediumFontPointSize
        anchors.bottom:    parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins:    ScreenTools.defaultFontPixelWidth
    }
    QGCLabel {
        text:               qsTr("Wind")
        font.family:        ScreenTools.demiboldFontFamily
        color:              "white"
        font.pointSize:     ScreenTools.mediumFontPointSize
        anchors.top:    parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins:    ScreenTools.defaultFontPixelWidth
    }

}
