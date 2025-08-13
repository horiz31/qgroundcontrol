/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- Telemetry RSSI
Item {
    id:             _root
    anchors.top:    parent.top
    anchors.bottom: parent.bottom
    width:          telemRow.width * 1.1

    property bool showIndicator: true //_hasTelemetry

    property var  _activeVehicle:   QGroundControl.multiVehicleManager.activeVehicle
    property bool _hasTelemetry:    _activeVehicle ? _activeVehicle.telemetryLRSSI !== 0 : false

    Component {
        id: telemRSSIInfo
        Rectangle {
            width:  telemCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: telemCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text
            Column {
                id:                 telemCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(telemGrid.width, telemLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent
                QGCLabel {
                    id:             telemLabel
                    text:           qsTr("Telemetry RSSI Status")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                GridLayout {
                    id:                 telemGrid
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    columns:            2
                    anchors.horizontalCenter: parent.horizontalCenter
                    QGCLabel { text: qsTr("Local RSSI:") }
                    QGCLabel { text: _activeVehicle.telemetryLRSSI + " dBm"}
                    QGCLabel { text: qsTr("Remote RSSI:") }
                    QGCLabel { text: _activeVehicle.telemetryRRSSI + " dBm"}
                    QGCLabel { text: qsTr("RX Errors:") }
                    QGCLabel { text: _activeVehicle.telemetryRXErrors }
                    QGCLabel { text: qsTr("Errors Fixed:") }
                    QGCLabel { text: _activeVehicle.telemetryFixed }
                    QGCLabel { text: qsTr("TX Buffer:") }
                    QGCLabel { text: _activeVehicle.telemetryTXBuffer }
                    QGCLabel { text: qsTr("Local Noise:") }
                    QGCLabel { text: _activeVehicle.telemetryLNoise }
                    QGCLabel { text: qsTr("Remote Noise:") }
                    QGCLabel { text: _activeVehicle.telemetryRNoise }
                }
            }
        }
    }

    Row {
        id:             telemRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth

        QGCColoredImage {
            id:                 telemIcon
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceSize.height:  height
            source:             "/qmlimages/RC.svg"
            fillMode:           Image.PreserveAspectFit
            opacity:            _hasTelemetry ? 1 : 0.5
            color:              qgcPal.buttonText
        }

        SignalStrength {
            anchors.verticalCenter: parent.verticalCenter
            size:                   parent.height * 0.5
            percent:                _hasTelemetry ? remoteRssiPercent() : 0
            property int remoteRssiPercent: {
                var percent = ((_activeVehicle.telemetryLRSSI + 100) / 50) * 100
                if (percent < 0) percent = 0
                if (percent > 100) percent = 100
                return Math.round(percent)
            }
        }

        QGCLabel {
            id:             rssiLabel2
            text:           _hasTelemetry ? _activeVehicle.telemetryLRSSI + " dBm" : "--"
            color:          getColor()
            opacity:            _hasTelemetry ? 1 : 0.5
            font.pointSize:         ScreenTools.mediumFontPointSize
            anchors.verticalCenter: parent.verticalCenter
            function getColor() {

                if (!_hasTelemetry)
                    return qgcPal.buttonText
                // Clamp RSSI to range -100 to -50
                var minDbm = -100;
                var maxDbm = -50;
                var clamped = Math.max(minDbm, Math.min(maxDbm, _activeVehicle.telemetryLRSSI));

                // Map to 0..1 range (0 = bad, 1 = good)
                var t = (clamped - minDbm) / (maxDbm - minDbm);

                // Interpolate between red (bad) and green (good)
                var r = Math.round(255 * (1 - t));
                var g = Math.round(255 * t);
                var b = 0;

                return Qt.rgba(r / 255, g / 255, b / 255, 1);
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, telemRSSIInfo)
        }
    }
}
