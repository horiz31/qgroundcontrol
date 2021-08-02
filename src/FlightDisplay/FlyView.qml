/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                  2.12
import QtQuick.Controls         2.4
import QtQuick.Dialogs          1.3
import QtQuick.Layouts          1.12

import QtLocation               5.3
import QtPositioning            5.3
import QtQuick.Window           2.2
import QtQml.Models             2.1

import QGroundControl               1.0
import QGroundControl.Airspace      1.0
import QGroundControl.Airmap        1.0
import QGroundControl.Controllers   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0

Item {
    id: _root

    // These should only be used by MainRootWindow
    property var planController:    _planController
    property var guidedController:  _guidedController

    PlanMasterController {
        id:                     _planController
        flyView:                true
        Component.onCompleted:  start()
    }

    property bool   _mainWindowIsMap:       mapControl.pipState.state === mapControl.pipState.fullState
    property bool   _isFullWindowItemDark:  _mainWindowIsMap ? mapControl.isSatelliteMap : true
    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property var    _missionController:     _planController.missionController
    property var    _geoFenceController:    _planController.geoFenceController
    property var    _rallyPointController:  _planController.rallyPointController
    property real   _margins:               ScreenTools.defaultFontPixelWidth / 2
    property var    _guidedController:      guidedActionsController
    property var    _guidedActionList:      guidedActionList
    property var    _guidedAltSlider:       guidedAltSlider
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75
    property rect   _centerViewport:        Qt.rect(0, 0, width, height)
    property real   _rightPanelWidth:       ScreenTools.defaultFontPixelWidth * 30
    property var    _mapControl:            mapControl

    property real   _fullItemZorder:    0
    property real   _pipItemZorder:     QGroundControl.zOrderWidgets

    property int    _commsMode:           _activeVehicle ? _activeVehicle.commsMode : 0

    function _calcCenterViewPort() {
        var newToolInset = Qt.rect(0, 0, width, height)
        toolstrip.adjustToolInset(newToolInset)
        if (QGroundControl.corePlugin.options.instrumentWidget) {
            flightDisplayViewWidgets.adjustToolInset(newToolInset)
        }
    }

    QGCToolInsets {
        id:                     _toolInsets
        leftEdgeBottomInset:    _pipOverlay.visible ? _pipOverlay.x + _pipOverlay.width : 0
        bottomEdgeLeftInset:    _pipOverlay.visible ? parent.height - _pipOverlay.y : 0
    }

    FlyViewWidgetLayer {
        id:                     widgetLayer
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        anchors.left:           parent.left
        anchors.right:          guidedAltSlider.visible ? guidedAltSlider.left : parent.right
        z:                      _fullItemZorder + 1
        parentToolInsets:       _toolInsets
        mapControl:             _mapControl
        visible:                !QGroundControl.videoManager.fullScreen
    }

    FlyViewCustomLayer {
        id:                 customOverlay
        anchors.fill:       widgetLayer
        z:                  _fullItemZorder + 2
        parentToolInsets:   widgetLayer.totalToolInsets
        mapControl:         _mapControl
        visible:            !QGroundControl.videoManager.fullScreen
    }

    GuidedActionsController {
        id:                 guidedActionsController
        missionController:  _missionController
        actionList:         _guidedActionList
        altitudeSlider:     _guidedAltSlider
    }

    /*GuidedActionConfirm {
        id:                         guidedActionConfirm
        anchors.margins:            _margins
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          QGroundControl.zOrderTopMost
        guidedController:           _guidedController
        altitudeSlider:             _guidedAltSlider
    }*/

    GuidedActionList {
        id:                         guidedActionList
        anchors.margins:            _margins
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          QGroundControl.zOrderTopMost
        guidedController:           _guidedController
    }

    //-- Altitude slider
    GuidedAltitudeSlider {
        id:                 guidedAltSlider
        anchors.margins:    _toolsMargin
        anchors.right:      parent.right
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        z:                  QGroundControl.zOrderTopMost
        radius:             ScreenTools.defaultFontPixelWidth / 2
        width:              ScreenTools.defaultFontPixelWidth * 10
        color:              qgcPal.window
        visible:            false
    }

    FlyViewMap {
        id:                     mapControl
        planMasterController:   _planController
        rightPanelWidth:        ScreenTools.defaultFontPixelHeight * 9
        pipMode:                !_mainWindowIsMap
        toolInsets:             customOverlay.totalToolInsets
        mapName:                "FlightDisplayView"
    }

    FlyViewVideo {
        id: videoControl
    }

    QGCPipOverlay {
        id:                     _pipOverlay
        anchors.left:           parent.left
        anchors.bottom:         parent.bottom
        anchors.margins:        _toolsMargin
        item1IsFullSettingsKey: "MainFlyWindowIsMap"
        item1:                  mapControl
        item2:                  QGroundControl.videoManager.hasVideo ? videoControl : null
        fullZOrder:             _fullItemZorder
        pipZOrder:              _pipItemZorder
        show:                   !QGroundControl.videoManager.fullScreen &&
                                    (videoControl.pipState.state === videoControl.pipState.pipState || mapControl.pipState.state === mapControl.pipState.pipState)
    }

    Rectangle {
               id:                 patriosBox
               width:  patriosCol.width   + ScreenTools.defaultFontPixelWidth  * 3
               height: patriosCol.height  + ScreenTools.defaultFontPixelHeight * 2
               radius: ScreenTools.defaultFontPixelHeight * 0.5
               color:  Qt.rgba(0,0,0,0.25)
               border.color:   Qt.rgba(0,0,0,0.7) //qgcPal.text
               anchors.right:              parent.right
               anchors.bottom:             parent.bottom
               anchors.bottomMargin:       ScreenTools.toolbarHeight + _margins
               anchors.rightMargin:       ScreenTools.defaultFontPixelHeight * 2
               visible:        true
               z:                          _fullItemZorder + 1

               Column {
                   id:                 patriosCol
                   spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                   width:              Math.max(patriosGrid.width, patriosLabel.width)
                   anchors.margins:    ScreenTools.defaultFontPixelHeight
                   anchors.centerIn:   parent

                   QGCLabel {
                       id:             patriosLabel
                       text:           qsTr("Comm. Selector")
                       color:          "white"
                       font.family:    ScreenTools.demiboldFontFamily
                       anchors.horizontalCenter: parent.horizontalCenter
                   }


                   GridLayout {
                       id:                 patriosGrid
                       anchors.margins:    ScreenTools.defaultFontPixelHeight
                       columnSpacing:      ScreenTools.defaultFontPixelWidth
                       columns:            2
                       anchors.horizontalCenter: parent.horizontalCenter




                       Item {
                           height:                     ScreenTools.defaultFontPixelHeight * 3
                           width:                      height
                           z:                          _fullItemZorder + 1
                           visible:                    true
                           Rectangle {
                               id:                 commsAutoBtnBackground
                               anchors.top:        parent.top
                               anchors.bottom:     parent.bottom
                               width:              height
                               radius:             height
                               color:              (_commsMode == 0) ? "blue" : "gray"


                           }
                           QGCColoredImage {
                               anchors.top:                parent.top
                               anchors.bottom:             parent.bottom
                               anchors.horizontalCenter:   parent.horizontalCenter
                               width:                      height * 0.625
                               sourceSize.width:           width
                               source:                     "/qmlimages/AutoIcon.svg"
                               visible:                    commsAutoBtnBackground.visible
                               fillMode:                   Image.PreserveAspectFit
                               color:                      "white"
                           }
                           MouseArea {
                               anchors.fill:   parent
                               enabled:        true
                               onClicked: {
                                   if (_commsMode != 0) {
                                       _activeVehicle.setCommMode(0)
                                       // reset blinking animation
                                       commsAutoBtnBackground.opacity = 1
                                   }
                               }
                           }
                       }
                       QGCLabel {
                           text:           qsTr("Automatic")
                           color:          "white"
                           font.family:    ScreenTools.demiboldFontFamily
                       }

                       // Line of sight line
                       Item {
                           height:                     ScreenTools.defaultFontPixelHeight * 3
                           width:                      height
                           z:                          _fullItemZorder + 1
                           visible:                    true
                           Rectangle {
                               id:                 commsLOSBtnBackground
                               anchors.top:        parent.top
                               anchors.bottom:     parent.bottom
                               width:              height
                               radius:             height
                               color:              (_commsMode == 1) ? "blue" : "gray"


                           }
                           QGCColoredImage {
                               anchors.top:                parent.top
                               anchors.bottom:             parent.bottom
                               anchors.horizontalCenter:   parent.horizontalCenter
                               width:                      height * 0.625
                               sourceSize.width:           width
                               source:                     "/qmlimages/WifiIcon.svg"
                               visible:                    commsAutoBtnBackground.visible
                               fillMode:                   Image.PreserveAspectFit
                               color:                      "white"
                           }
                           MouseArea {
                               anchors.fill:   parent
                               enabled:        true
                               onClicked: {
                                   if (_commsMode != 1) {
                                       _activeVehicle.setCommMode(1)
                                       // reset blinking animation
                                       commsLOSBtnBackground.opacity = 1
                                   }
                               }
                           }
                       }
                       QGCLabel {
                           text:           qsTr("Line of Sight")
                           color:          "white"
                           font.family:    ScreenTools.demiboldFontFamily
                       }

                       // Cellular line
                       Item {
                           height:                     ScreenTools.defaultFontPixelHeight * 3
                           width:                      height
                           z:                          _fullItemZorder + 1
                           visible:                    true
                           Rectangle {
                               id:                 commsCellularBtnBackground
                               anchors.top:        parent.top
                               anchors.bottom:     parent.bottom
                               width:              height
                               radius:             height
                               color:              (_commsMode == 2) ? "blue" : "gray"


                           }
                           QGCColoredImage {
                               anchors.top:                parent.top
                               anchors.bottom:             parent.bottom
                               anchors.horizontalCenter:   parent.horizontalCenter
                               width:                      height * 0.625
                               sourceSize.width:           width
                               source:                     "/qmlimages/CellularIcon.svg"
                               visible:                    commsAutoBtnBackground.visible
                               fillMode:                   Image.PreserveAspectFit
                               color:                      "white"
                           }
                           MouseArea {
                               anchors.fill:   parent
                               enabled:        true
                               onClicked: {
                                   if (_commsMode != 2) {
                                       _activeVehicle.setCommMode(2)
                                       // reset blinking animation
                                       commsCellularBtnBackground.opacity = 1
                                   }
                               }
                           }
                       }
                       QGCLabel {
                           text:           qsTr("Cellular")
                           color:          "white"
                           font.family:    ScreenTools.demiboldFontFamily
                       }
                   }
               }
    }



}
