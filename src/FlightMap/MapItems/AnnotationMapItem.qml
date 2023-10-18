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
import QtQuick.Dialogs              1.2


/// Marker for displaying an Annotation on the map
MapQuickItem {
    property var    marker
    property var    map
    property double altitude:       Number.NaN                                      ///< NAN to not show
    property string displayName:    marker ? marker.displayName.value : ""                                              ///< marker callsign
    property real   size:           ScreenTools.defaultFontPixelHeight * 1.5        /// Size for icon, typically specified by the parent
    property string uid
    property string iconColor:          marker ? marker.iconColor : "#000000"
    property string type:           marker ? marker.type : "point"

    property var  _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool _inGotoFlightMode: _activeVehicle ? _activeVehicle.flightMode === _activeVehicle.gotoFlightMode : false
    property bool _isFlying: _activeVehicle ? _activeVehicle.armed && _activeVehicle.flying : false

    anchorPoint.x:  markerItem.width  / 2
    anchorPoint.y:  markerItem.height / 2
    visible:        coordinate.isValid

    property var    _map:           map

    sourceItem: Item {
        id:         markerItem
        width:      markerIcon.width
        height:     markerIcon.height
        opacity:    1.0


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



        QGCColoredImage {
            id:                 markerIcon
            height:             ScreenTools.defaultFontPixelHeight * 1.5
            width:              size
            source:             "/qmlimages/annotation.svg"
            sourceSize.height:  size
            fillMode:           Image.PreserveAspectFit
            mipmap:             true
            smooth:             true
            color:              iconColor
            QGCMouseArea {
                fillItem:   markerIcon
                onClicked: {
                    //currentItemScope.focus = true
                    //_clickedCoordinate = mapClickIconItem.coordinate
                    //mainWindow.showComponentDialog(editPositionDialog, qsTr("Edit Guided Position"), mainWindow.showDialogDefaultWidth, StandardButton.Close)

                    //if needs expand, then use the popup menu below, but for now going directly to Edit Position
                    hamburgerMenu.popup()
                }

                QGCMenu {
                    id: hamburgerMenu

                    QGCMenuItem {
                        text:           qsTr("Navigate To")
                        visible:        _isFlying
                        onTriggered:
                        {
                            mapClickIconItem.hide()
                            gotoLocationItem.show(coordinate)
                            guidedPlanMapCircle.setCenter(coordinate)
                            guidedPlanMapCircle.setRadius(_activeVehicle.guidedModeRadius)
                            guidedPlanMapCircle.setClockwise(true)
                            globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionGoto, coordinate, gotoLocationItem, guidedPlanMapCircle)

                                                   }
                    }
                    QGCMenuItem {
                        text:           qsTr("Point Camera At")
                        visible:            !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                        //visible: true
                        onTriggered:
                        {
                            mapClickIconItem.hide()
                            nvPTCLocationItem.show(coordinate)
                            if(_activeVehicle)
                                joystickManager.cameraManagement.pointToCoordinate(coordinate.latitude, coordinate.longitude)
                        }
                    }
                    QGCMenuItem {
                        text:           qsTr("Delete Annotation")
                        visible:        true
                        onTriggered:
                        {
                            mainWindow.showPopupDialogFromComponent(deleteMarkerDialog)
                        }
                    }


                }
            }

        }
        /*
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
            }
            QGCMouseArea {
                fillItem:   markerIcon
                onClicked: {
                    //currentItemScope.focus = true
                    //_clickedCoordinate = mapClickIconItem.coordinate
                    //mainWindow.showComponentDialog(editPositionDialog, qsTr("Edit Guided Position"), mainWindow.showDialogDefaultWidth, StandardButton.Close)

                    //if needs expand, then use the popup menu below, but for now going directly to Edit Position
                    hamburgerMenu.popup()
                }

                QGCMenu {
                    id: hamburgerMenu

                    QGCMenuItem {
                        text:           qsTr("Navigate To")
                        visible:        true
                        onTriggered:
                        {
                            mapClickIconItem.hide()
                            gotoLocationItem.show(coordinate)
                            guidedPlanMapCircle.setCenter(coordinate)
                            guidedPlanMapCircle.setRadius(_activeVehicle.guidedModeRadius)
                            guidedPlanMapCircle.setClockwise(true)
                            globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionGoto, coordinate, gotoLocationItem, guidedPlanMapCircle)

                                                   }
                    }
                    QGCMenuItem {
                        text:           qsTr("Point Camera At")
                        visible:            !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                        //visible: true
                        onTriggered:
                        {
                            mapClickIconItem.hide()
                            nvPTCLocationItem.show(coordinate)
                            if(_activeVehicle)
                                joystickManager.cameraManagement.pointToCoordinate(coordinate.latitude, coordinate.longitude)
                        }
                    }
                    QGCMenuItem {
                        text:           qsTr("Delete Annotation")
                        visible:        true //isLocal
                        onTriggered:
                        {
                            mainWindow.showPopupDialogFromComponent(deleteMarkerDialog)
                        }
                    }


                }
            }
        }
        */

        QGCMapLabel {
            id:                         markerLabel
            visible:                    _mainWindowIsMap
            anchors.top:                parent.bottom
            anchors.horizontalCenter:   parent.horizontalCenter
            map:                        _map
            text:                       markerLabelText
            font.pointSize:             ScreenTools.mediumFontPointSize
            //visible:                    true
            property string markerLabelText: visible ? displayName : ""

        }

        QGCMapLabel {
            id:                         markerAltLabel
            visible:                    _mainWindowIsMap && !isNaN(altitude)
            anchors.top:                markerLabel.bottom
            anchors.horizontalCenter:   parent.horizontalCenter
            map:                        _map
            text:                       markerLabelText
            font.pointSize:             ScreenTools.mediumFontPointSize
            //visible:                    true
            property string markerLabelText: visible ? "(" + altitude + " ft. AGL)" : ""

        }

        Component {
            id: deleteMarkerDialog
            QGCPopupDialog {
               title:      qsTr("Delete Annotation?")
                buttons:    StandardButton.Yes | StandardButton.Cancel

                QGCLabel { text: qsTr("Are you sure you want to delete annotation '%1'?").arg(displayName) }

                function accept() {
                    QGroundControl.annotationManager.deleteMarker(uid)
                    hideDialog()
                }
            }

        }
    }
}
