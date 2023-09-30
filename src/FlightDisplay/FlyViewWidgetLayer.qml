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
import QtGraphicalEffects       1.0

import QGroundControl               1.0
import QGroundControl.Controls      1.0
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



// This is the ui overlay layer for the widgets/tools for Fly View
Item {
    id: _root

    property var    parentToolInsets
    property var    totalToolInsets:        _totalToolInsets
    property var    mapControl
    property var    mapMouseCursor
    property var    _mapPosition:           QGroundControl.flightMapPosition
    property var    _activeVehicle:             QGroundControl.multiVehicleManager.activeVehicle
    property var    _activeVehicleCoordinate:   _activeVehicle ? _activeVehicle.coordinate : QtPositioning.coordinate()
    property var    _planMasterController:  globals.planMasterControllerFlyView
    property var    _missionController:     _planMasterController.missionController
    property var    _geoFenceController:    _planMasterController.geoFenceController
    property var    _rallyPointController:  _planMasterController.rallyPointController
    property var    _guidedController:      globals.guidedControllerFlyView
    property real   _margins:               ScreenTools.defaultFontPixelWidth / 2
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75
    property rect   _centerViewport:        Qt.rect(0, 0, width, height)
    property real   _rightPanelWidth:       ScreenTools.defaultFontPixelWidth * 22
    property bool   _isCheckListWindowVisible: false
    property var    _checkListWindow
    property real   _heading: _activeVehicle ? _activeVehicle.heading.rawValue : 0
    property bool   _useChecklist:              QGroundControl.settingsManager.appSettings.useChecklist.rawValue && QGroundControl.corePlugin.options.preFlightChecklistUrl.toString().length
    property bool   _enforceChecklist:          _useChecklist && QGroundControl.settingsManager.appSettings.enforceChecklist.rawValue
    property bool   _checklistFailed: _activeVehicle ? (_useChecklist ? (_enforceChecklist ? (_activeVehicle.checkListState !== Vehicle.CheckListPassed ? true : false) : false) : false) : false
    property bool   _isDisarmed: _activeVehicle ? _activeVehicle.armed === false : true

    //Create the pre-flight checklist after vehicle created and params loaded. Then the checklist stays in memory and its state is preserved
    Connections {
        target:             QGroundControl.multiVehicleManager
        onParameterReadyVehicleAvailableChanged:  {
            if (QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable) {
                if (_checkListWindow)
                {
                    _checkListWindow.destroy()
                }
                _checkListWindow = preFlightChecklistPopup.createObject(mainWindow)
                _checkListWindow.title = qsTr("Pre-Flight Checklist")
                _checkListWindow.close()
            }
        }
    }

    QGCToolInsets {
        id:                     _totalToolInsets
        leftEdgeTopInset:       toolStrip.leftInset
        leftEdgeCenterInset:    toolStrip.leftInset
        leftEdgeBottomInset:    parentToolInsets.leftEdgeBottomInset
        rightEdgeTopInset:      parentToolInsets.rightEdgeTopInset
        rightEdgeCenterInset:   parentToolInsets.rightEdgeCenterInset
        rightEdgeBottomInset:   parentToolInsets.rightEdgeBottomInset
        topEdgeLeftInset:       parentToolInsets.topEdgeLeftInset
        topEdgeCenterInset:     parentToolInsets.topEdgeCenterInset
        topEdgeRightInset:      parentToolInsets.topEdgeRightInset
        bottomEdgeLeftInset:    parentToolInsets.bottomEdgeLeftInset
        bottomEdgeCenterInset:  mapScale.centerInset
        bottomEdgeRightInset:   0
    }

    FlyViewMissionCompleteDialog {
        missionController:      _missionController
        geoFenceController:     _geoFenceController
        rallyPointController:   _rallyPointController
    }

    Row {
        id:                 multiVehiclePanelSelector
        anchors.margins:    _toolsMargin
        anchors.top:        parent.top
        anchors.right:      parent.right
        width:              _rightPanelWidth
        spacing:            ScreenTools.defaultFontPixelWidth
        visible:            QGroundControl.multiVehicleManager.vehicles.count > 1 && QGroundControl.corePlugin.options.flyView.showMultiVehicleList

        property bool showSingleVehiclePanel:  !visible || singleVehicleRadio.checked

        QGCMapPalette { id: mapPal; lightColors: true }

        QGCRadioButton {
            id:             singleVehicleRadio
            text:           qsTr("Single")
            checked:        true
            textColor:      mapPal.text
        }

        QGCRadioButton {
            text:           qsTr("Multi-Vehicle")
            textColor:      mapPal.text
        }
    }

    MultiVehicleList {
        anchors.margins:    _toolsMargin
        anchors.top:        multiVehiclePanelSelector.bottom
        anchors.right:      parent.right
        width:              _rightPanelWidth
        height:             parent.height - y - _toolsMargin
        visible:            !multiVehiclePanelSelector.showSingleVehiclePanel
    }

    FlyViewInstrumentPanel {
        id:                         instrumentPanel
        anchors.margins:            _toolsMargin
        anchors.top:                multiVehiclePanelSelector.visible ? multiVehiclePanelSelector.bottom : parent.top
        anchors.right:              parent.right
        width:                      _rightPanelWidth
        spacing:                    _toolsMargin
        visible:                    QGroundControl.corePlugin.options.flyView.showInstrumentPanel && multiVehiclePanelSelector.showSingleVehiclePanel
        availableHeight:            parent.height - y - _toolsMargin

        property real rightInset: visible ? parent.width - x : 0
    }

    PhotoVideoControl {
        id:                     photoVideoControl
        anchors.margins:        _toolsMargin
        anchors.top:            instrumentPanel.bottom
        anchors.topMargin:      _toolsMargin * 4
        anchors.right:          parent.right
        anchors.verticalCenter: undefined
        width:                  _rightPanelWidth
        visible:                _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? false : true) : false //QGroundControl.settingsManager.flyViewSettings.showSimpleCameraControl.rawValue //replace with detection of the gimbal camera

    }

    /*

    PhotoVideoControl {
        id:                     photoVideoControl
        visible:                true
        anchors.margins:        _toolsMargin
        anchors.right:          parent.right
        width:                  _rightPanelWidth
        state:                  _verticalCenter ? "verticalCenter" : "topAnchor"
        states: [
            State {
                name: "verticalCenter"
                AnchorChanges {
                    target:                 photoVideoControl
                    anchors.top:            undefined
                    anchors.verticalCenter: _root.verticalCenter
                }
            },
            State {
                name: "topAnchor"
                AnchorChanges {
                    target:                 photoVideoControl
                    anchors.verticalCenter: undefined
                    anchors.top:            instrumentPanel.bottom
                }
            }
        ]

        property bool _verticalCenter: !QGroundControl.settingsManager.flyViewSettings.alternateInstrumentPanel.rawValue
    }
*/

    //Wind Inidicator, Super Volo integration
    WindIndicator {
        visible:    !QGroundControl.videoManager.fullScreene
        z:          QGroundControl.zOrderTopMost
    }

    FlyViewThrottleIndicator {
        id:                 throttlePanel
        anchors.top:                parent.top
        anchors.topMargin:          ScreenTools.defaultFontPixelHeight * 1.5
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          QGroundControl.zOrderWidgets
        visible:                    _activeVehicle ? ((((_activeVehicle.flightMode == "FBW A") || (_activeVehicle.flightMode == "FBW B")) && _activeVehicle.flying) ? true : false) : false

    }

    ChecklistWarningIndicator {
        id:                 checklistPanel
        anchors.top:                parent.top
        anchors.topMargin:          ScreenTools.defaultFontPixelHeight * 1.5
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          throttlePanel.z + 1
        visible:                    _checklistFailed && _isDisarmed

    }

    TelemetryValuesBar {
        id:                 telemetryPanel
        x:                  recalcXPosition()
        anchors.margins:    _toolsMargin
        mouseCursor:       mapMouseCursor

        // States for custom layout support
        states: [
            State {
                name: "bottom"
                when: telemetryPanel.bottomMode

                AnchorChanges {
                    target: telemetryPanel
                    anchors.top: undefined
                    anchors.bottom: parent.bottom
                    anchors.right: undefined
                    anchors.verticalCenter: undefined
                }

                PropertyChanges {
                    target: telemetryPanel
                    x: recalcXPosition()
                }
            },

            State {
                name: "right-video"
                when: !telemetryPanel.bottomMode && photoVideoControl.visible

                AnchorChanges {
                    target: telemetryPanel
                    anchors.top: photoVideoControl.bottom
                    anchors.bottom: undefined
                    anchors.right: parent.right
                    anchors.verticalCenter: undefined
                }
            },

            State {
                name: "right-novideo"
                when: !telemetryPanel.bottomMode && !photoVideoControl.visible

                AnchorChanges {
                    target: telemetryPanel
                    anchors.top: undefined
                    anchors.bottom: undefined
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        ]

        function recalcXPosition() {
            // First try centered
            var halfRootWidth   = _root.width / 2
            var halfPanelWidth  = telemetryPanel.width / 2
            var leftX           = (halfRootWidth - halfPanelWidth) - _toolsMargin
            var rightX          = (halfRootWidth + halfPanelWidth) + _toolsMargin
            if (leftX >= parentToolInsets.leftEdgeBottomInset || rightX <= parentToolInsets.rightEdgeBottomInset ) {
                // It will fit in the horizontalCenter
                return halfRootWidth - halfPanelWidth
            } else {
                // Anchor to left edge
                return parentToolInsets.leftEdgeBottomInset + _toolsMargin
            }
        }
    }

    //-- Virtual Joystick
    Loader {
        id:                         virtualJoystickMultiTouch
        z:                          QGroundControl.zOrderTopMost + 1
        width:                      parent.width  - (_pipOverlay.width / 2)
        height:                     Math.min(parent.height * 0.25, ScreenTools.defaultFontPixelWidth * 16)
        visible:                    _virtualJoystickEnabled && !QGroundControl.videoManager.fullScreen && !(_activeVehicle ? _activeVehicle.usingHighLatencyLink : false)
        anchors.bottom:             parent.bottom
        anchors.bottomMargin:       parentToolInsets.leftEdgeBottomInset + ScreenTools.defaultFontPixelHeight * 2
        anchors.horizontalCenter:   parent.horizontalCenter
        source:                     "qrc:/qml/VirtualJoystick.qml"
        active:                     _virtualJoystickEnabled && !(_activeVehicle ? _activeVehicle.usingHighLatencyLink : false)

        property bool autoCenterThrottle: QGroundControl.settingsManager.appSettings.virtualJoystickAutoCenterThrottle.rawValue

        property bool _virtualJoystickEnabled: QGroundControl.settingsManager.appSettings.virtualJoystick.rawValue
    }

    FlyViewToolStrip {
        id:                     toolStrip
        anchors.leftMargin:     _toolsMargin + parentToolInsets.leftEdgeCenterInset
        anchors.topMargin:      _toolsMargin + parentToolInsets.topEdgeLeftInset
        anchors.left:           parent.left
        anchors.top:            parent.top
        z:                      QGroundControl.zOrderWidgets
        maxHeight:              parent.height - y - parentToolInsets.bottomEdgeLeftInset - _toolsMargin
        visible:                !QGroundControl.videoManager.fullScreen

        onDisplayPreFlightChecklist:
        {
            if (!_isCheckListWindowVisible && _checkListWindow)
            {
                _checkListWindow.show()
            }

        }
        onClearFlightPath:     _activeVehicle ? _activeVehicle.trajectoryPoints.clear() : 0;

        onCenterMap:           { mapControl.zoomLevel = 16; mapControl.animatedMapRecenter(mapControl.center, _activeVehicle.coordinate);}
        property real leftInset: x + width
    }

    MapFitFunctions {
        id:                         mapFitFunctions // The name for this id cannot be changed without breaking references outside of this code. Beware!
        map:                        mapControl
        usePlannedHomePosition:     false
        planMasterController:       _planMasterController
    }
    FlyViewAirspaceIndicator {
        anchors.top:                parent.top
        anchors.topMargin:          ScreenTools.defaultFontPixelHeight * 0.25
        anchors.horizontalCenter:   parent.horizontalCenter
        z:                          QGroundControl.zOrderWidgets
        show:                       mapControl.pipState.state !== mapControl.pipState.pipState
    }

    VehicleWarnings {
        anchors.centerIn:   parent
        z:                  QGroundControl.zOrderTopMost
    }

    MapScale {
        id:                 mapScale
        anchors.margins:    _toolsMargin
        anchors.left:       toolStrip.right
        anchors.top:        parent.top
        mapControl:         _mapControl
        buttonsOnLeft:      false
        visible:            !ScreenTools.isTinyScreen && QGroundControl.corePlugin.options.flyView.showMapScale && mapControl.pipState.state === mapControl.pipState.fullState

        property real centerInset: visible ? parent.height - y : 0
    }
//
//   Component {
//        id: preFlightChecklistPopup
//        FlyViewPreFlightChecklistPopup {
//            id: preflightRoot
//        }
//    }

    Component {
        id: preFlightChecklistPopup

        Window {
            id:         preFlightChecklistWindow
            width:      ScreenTools.defaultFontPixelWidth  * 100
            maximumWidth: width
            minimumWidth: width
            height:     ScreenTools.defaultFontPixelHeight * 40
            visible:    true
            color:      QGroundControl.globalPalette.window
            flags:      Qt.Dialog

            QGCFlickable {
                clip:               true
                anchors.fill:       parent
                contentHeight:      preflightRoot.height
                contentWidth:       preflightRoot.width

                FlyViewPreFlightChecklistPopup {
                            id: preflightRoot
                }

            }

            function showPopupDialogFromComponent(component, properties) {
                var dialog = popupDialogContainerComponent.createObject(preFlightChecklistWindow, { dialogComponent: component, dialogProperties: properties })
                dialog.open()
                return dialog
            }

            onVisibilityChanged: {
                _isCheckListWindowVisible = visible
            }

            onClosing: {
                _isCheckListWindowVisible = false
                visible = false
            }
        }

    }
}
