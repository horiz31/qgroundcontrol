

/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/**
 * @file
 *   @brief QGC Attitude Instrument
 *   @author Gus Grubba <gus@auterion.com>
 */
import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.2

import QGroundControl 1.0
import QGroundControl.Controls 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.Palette 1.0
import QGroundControl.SettingsManager 1.0

Item {
    id: root

    property bool showPitch: true
    property var vehicle: null
    property real size
    property bool showHeading: false //this is now in compass widget

    property real _rollAngle: vehicle ? vehicle.roll.rawValue : 0
    property real _pitchAngle: vehicle ? vehicle.pitch.rawValue : 0
    property var _unitsSettings: QGroundControl.settingsManager.unitsSettings
    property bool _isMobile: ScreenTools.isMobile

    width: size
    height: size

    QGCPalette {
        id: qgcPal
        colorGroupEnabled: enabled
    }

    Item {
        id: instrument
        anchors.fill: parent
        visible: false

        //----------------------------------------------------
        //-- Artificial Horizon
        QGCArtificialHorizon {
            rollAngle: _rollAngle
            pitchAngle: _pitchAngle
            anchors.fill: parent
        }
        //----------------------------------------------------
        //-- Pointer
        Image {
            id: pointer
            source: "/qmlimages/attitudePointer.svg"
            mipmap: true
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            sourceSize.height: parent.height
        }
        //----------------------------------------------------
        //-- Instrument Dial
        Image {
            id: instrumentDial
            source: "/qmlimages/attitudeDial.svg"
            mipmap: true
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            sourceSize.height: parent.height
            transform: Rotation {
                origin.x: root.width / 2
                origin.y: root.height / 2
                angle: -_rollAngle
            }
        }
        //----------------------------------------------------
        //-- Pitch
        QGCPitchIndicator {
            id: pitchWidget
            visible: root.showPitch
            size: root.size * 0.5
            anchors.verticalCenter: parent.verticalCenter
            pitchAngle: _pitchAngle
            rollAngle: _rollAngle
            color: Qt.rgba(0, 0, 0, 0)
        }
        //----------------------------------------------------
        //-- Cross Hair
        Image {
            id: crossHair
            anchors.centerIn: parent
            source: "/qmlimages/crossHair.svg"
            mipmap: true
            width: size * 0.75
            sourceSize.width: width
            fillMode: Image.PreserveAspectFit
        }
        //airspeed and altitude background
        Rectangle {
            id: asaltBackground
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            width: hudAltitudeLayout.width + (ScreenTools.defaultFontPixelWidth * 1.5)
            height: hudAltitudeLayout.height + (ScreenTools.defaultFontPixelHeight * 1.5)
            color: "black"
            opacity: 0.5
            radius: 5
        }
    }

    Rectangle {
        id: mask
        anchors.fill: instrument
        radius: width / 2
        color: "black"
        visible: false
    }

    OpacityMask {
        anchors.fill: instrument
        source: instrument
        maskSource: mask
    }

    Rectangle {
        id: borderRect
        anchors.fill: parent
        radius: width / 2
        color: Qt.rgba(0, 0, 0, 0)
        border.color: qgcPal.text
        border.width: 1
    }

    //Heading Label, bottom center
    QGCLabel {
        anchors.bottomMargin: Math.round(
                                  ScreenTools.defaultFontPixelHeight * .75)
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: _headingString3
        color: "black"
        font.family: ScreenTools.normalFontFamily
        font.pointSize: ScreenTools.largeFontPointSize
        visible: showHeading

        property string _headingString: vehicle ? vehicle.heading.rawValue.toFixed(
                                                      0) + "°" : "---"
        property string _headingString2: _headingString.length
                                         === 1 ? "0" + _headingString : _headingString
        property string _headingString3: _headingString2.length
                                         === 2 ? "0" + _headingString2 : _headingString2
    }

    //Altitude, right, vertical center
    GridLayout {
        id: hudAltitudeLayout
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: Math.round(ScreenTools.defaultFontPixelWidth * 1.5)
        rowSpacing: 0
        columns: 3

        QGCLabel {
            text: vehicle ? QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(
                                vehicle.altitudeRelative.rawValue).toFixed(
                                0) : "----"
            color: vehicle ? parent.getAltColor() : "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.mediumFontPointSize
            Layout.alignment: Qt.AlignRight
        }
        QGCLabel {
            text: vehicle ? QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString : ""
            color: vehicle ? parent.getAltColor() : "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.mediumFontPointSize
            Layout.alignment: Qt.AlignRight
        }
        QGCLabel {
            text: "AHL"
            color: vehicle ? parent.getAltColor() : "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.mediumFontPointSize
            Layout.alignment: Qt.AlignRight
        }

        QGCLabel {
            text: vehicle ? QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(
                                vehicle.altitudeAMSL.rawValue).toFixed(
                                0) : "----"
            color: "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.smallFontPointSize
            Layout.alignment: Qt.AlignRight
        }
        QGCLabel {
            text: vehicle ? QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString : ""
            color: "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.smallFontPointSize
            Layout.alignment: Qt.AlignRight
        }
        QGCLabel {
            text: "MSL"
            color: "white"
            font.family: ScreenTools.normalFontFamily
            font.pointSize: ScreenTools.smallFontPointSize
            Layout.alignment: Qt.AlignRight
        }

        QGCColoredImage {
            width: height
            height: ScreenTools.defaultFontPixelWidth * 2.2
            sourceSize.height: height
            source: getClimbrateArrow()
            color: "white"
            Layout.alignment: Qt.AlignRight
            function getClimbrateArrow() {
                if (vehicle) {
                    if (vehicle.climbRate.value > 1.5)
                        return "/InstrumentValueIcons/arrow-thick-up.svg"
                    else if (vehicle.climbRate.value < -1.5)
                        return "/InstrumentValueIcons/arrow-thick-down.svg"
                }
                return ""
            }
        }

        function getAltColor() {
            if (!vehicle.flying)
                return "white"
            else if (vehicle.altitudeRelative.value < 15.24)
                return "red"
            else if (vehicle.altitudeRelative.value < 30.48)
                return "orange"
            else
                return "white"
        }
    }
}
