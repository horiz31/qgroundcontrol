/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.12
import QtQuick.Layouts              1.12

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QtGraphicalEffects           1.0

Item {

    property var     _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    ColumnLayout {
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.bottom: parent.bottom

         Rectangle {
             visible: rangeFinderText.getRFVisible()
             width: rangeFinderText.width * 1.2
             height: rangeFinderText.height * 1.1
             color:              qgcPal.window
             opacity:            0.85
             border.color: "black"
             border.width: 0
             radius: 10
             Layout.alignment: Qt.AlignHCenter

             QGCLabel {
                 id: rangeFinderText
                 visible:  getRFVisible()
                 text: _activeVehicle ? qsTr("Ground: ") + QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(_activeVehicle.rangefinder.rawValue).toFixed(0) + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString : ""
                 font.family:        ScreenTools.demiboldFontFamily
                 font.pointSize:     ScreenTools.largeFontPointSize * 1.2
                 color: qgcPal.text
                 anchors.horizontalCenter: parent.horizontalCenter

                   function getRFColor() {
                       if (_activeVehicle)
                       {
                           if (!_activeVehicle.flying)
                               return "white"
                           if (_activeVehicle.rangefinder.rawValue < 15)
                                 return qgcPal.colorRed
                           else if (_activeVehicle.rangefinder.rawValue < 25)
                               return "orange"
                           else if (_activeVehicle.rangefinder.rawValue < 45)
                               return "yellow"
                           else
                               return "white"
                       }
                       return "white"
                   }
                   function getRFVisible() {
                       if (_activeVehicle)
                       {
                           if (!_activeVehicle.flying)
                               return false
                           else if (_activeVehicle.rangefinder.rawValue < 45)
                               return true
                       }
                       return false
                   }


             }
         }

         QGCColoredImage {
             id:     rangefinderIcon
             visible:  rangeFinderText.getRFVisible()
             Layout.alignment:  Qt.AlignHCenter
             source:             "/res/rangefinderWarning.svg"
             mipmap:             true
             width:              450
             height:             100
             sourceSize.width:   width
             color:              rangeFinderText.getRFColor()
             fillMode:           Image.PreserveAspectFit
             layer.enabled: true
                       layer.effect: DropShadow {
                             horizontalOffset: 0
                             verticalOffset: 0
                             radius: 3.0
                             samples: 12
                             source: rangefinderIcon
                             color:              Qt.rgba(0,0,0,1.0)
                         }

           Timer{
                   id: timer
                   interval: 500
                   running: true
                   repeat: true
                   onTriggered: rangefinderIcon.opacity = rangefinderIcon.opacity === 0 ? 1 : 0
                }

        }

     }
}
