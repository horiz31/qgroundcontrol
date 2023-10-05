/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick              2.3
import QtLocation           5.3
import QtPositioning        5.3
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0

/// Marker for displaying an ATAK Marker on the map
MapQuickItem {
    property var    marker
    property var    map
    property double altitude:       Number.NaN                                      ///< NAN to not show
    property string callsign:       marker ? marker.callsign.value : ""                                              ///< marker callsign
    property double heading:        marker ? marker.heading.value : Number.NaN      ///< marker heading, NAN for none
    property bool   isLocal:        false                                           /// Identify if this is a locally generated marker or ingested from remote TAK system
    property real   size:           ScreenTools.defaultFontPixelHeight * 1.5        /// Size for icon, typically specified by the parent

    anchorPoint.x:  markerItem.width  / 2
    anchorPoint.y:  markerItem.height / 2
    visible:        coordinate.isValid

    property var    _map:           map

    sourceItem: Item {
        id:         markerItem
        width:      markerIcon.width
        height:     markerIcon.height
        opacity:    0.8

        Rectangle {
            id:                 markerShadow
            anchors.fill:       markerIcon
            color:              Qt.rgba(1,1,1,1)
            radius:             width * 0.5
            visible:            false
        }
        DropShadow {
            anchors.fill:       markerShadow
            visible:            false//markerIcon.visible
            horizontalOffset:   4
            verticalOffset:     4
            radius:             32.0
            samples:            65
            color:              Qt.rgba(0.94,0.91,0,0.5)
            source:             markerShadow
        }
        Image {
            id:                 markerIcon
            source:             "/qmlimages/atakMarker.svg"
            mipmap:             true
            width:              size
            sourceSize.width:   size
            fillMode:           Image.PreserveAspectCrop
            transform: Rotation {
                origin.x:       markerIcon.width  / 2
                origin.y:       markerIcon.height / 2
                angle:          isNaN(heading) ? 0 : heading
            }
        }

        QGCMapLabel {
            id:                         markerLabel
            anchors.top:                parent.bottom
            anchors.horizontalCenter:   parent.horizontalCenter
            map:                        _map
            text:                       markerLabelText
            font.pointSize:             ScreenTools.smallFontPointSize
            visible:                    true
            property string markerLabelText: visible ? callsign : ""

        }
    }
}
