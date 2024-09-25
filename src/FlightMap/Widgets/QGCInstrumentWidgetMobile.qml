/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.12
import QtQuick.Layouts  1.12

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0

Item{
    id: root
    height: width
    property real _width : width
    property real   _windPanelWidth:        ScreenTools.defaultFontPixelWidth * 11
    property var    _activeVehicle:             QGroundControl.multiVehicleManager.activeVehicle
    property var _windDirection: _activeVehicle ? _activeVehicle.wind.direction.value.toFixed(0) : 0
    property var _windSpeed: _activeVehicle ? _activeVehicle.wind.speed.value.toFixed(0) : 0
    property var _windUnits: _activeVehicle ? _activeVehicle.wind.speed.units : ""

    QGCPalette { id: qgcPal }
    QGCAttitudeWidget {
        id:                     attitude
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        size:                   _width
        vehicle:                globals.activeVehicle
    }
    QGCCompassWidget {
        id:                     compass
        anchors.right: attitude.left
        anchors.verticalCenter: parent.verticalCenter
        size:                   _width
        vehicle:                globals.activeVehicle
    }
    Rectangle{
        id: windRoot
        anchors.horizontalCenter: attitude.left
        anchors.bottom: parent.verticalCenter
        width: _windPanelWidth
        height: (parent.height/2) + ScreenTools.defaultFontPixelHeight*1
        color:              qgcPal.window
        z: attitude.z>compass.z?compass.z-1:attitude.z-1
        radius: width/2
        visible:    !QGroundControl.videoManager.fullScreen
        QGCLabel {
            id:windLabel
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            text:        qsTr("Wind")
            font.family: ScreenTools.demiboldFontFamily
        }
        QGCColoredImage {
            id:                 windIcon
            source:             "/res/wind-arrow.svg"
            mipmap:             true
            height:  ScreenTools.defaultFontPixelHeight * 1.5
            width:   ScreenTools.defaultFontPixelHeight * 1.5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: windLabel.bottom
            color:              qgcPal.colorBlue//qgcPal.globalTheme === QGCPalette.Light?qgcPal.colorGrey:qgcPal.colorBlue
            transform: Rotation {
                origin.x: windIcon.width / 2
                origin.y: windIcon.height / 2
                angle: _windDirection
            }
        }
        QGCLabel {
            anchors.horizontalCenter: windIcon.horizontalCenter
            anchors.verticalCenter: windIcon.verticalCenter
            z: windIcon.z+1
            text:        _windSpeed + " " + _windUnits
            font.family: ScreenTools.demiboldFontFamily
            visible:     _activeVehicle ? (isNaN(_windSpeed) ? false : true) : false
        }


    }
}
