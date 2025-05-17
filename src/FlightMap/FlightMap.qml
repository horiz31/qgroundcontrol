/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2
import QtLocation       5.3
import QtPositioning    5.3
import QtQuick.Dialogs  1.2

import QGroundControl                       1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.Controls              1.0
import QGroundControl.FlightMap             1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.Vehicle               1.0
import QGroundControl.QGCPositionManager    1.0

Map {
    id: _map

    //-- Qt 5.9 has rotation gesture enabled by default. Here we limit the possible gestures.
    gesture.acceptedGestures:   MapGestureArea.PinchGesture | MapGestureArea.PanGesture | MapGestureArea.FlickGesture
    gesture.flickDeceleration:  3000
    plugin:                     Plugin { name: "QGroundControl" }

    // https://bugreports.qt.io/browse/QTBUG-82185
    opacity:                    0.99

    property string mapName:                        'defaultMap'
    property bool   isSatelliteMap:                 activeMapType.name.indexOf("Satellite") > -1 || activeMapType.name.indexOf("Hybrid") > -1
    property var    gcsPosition:                    QGroundControl.qgcPositionManger.gcsPosition
    property real   gcsHeading:                     QGroundControl.qgcPositionManger.gcsHeading
    property bool   allowGCSLocationCenter:         false   ///< true: map will center/zoom to gcs location one time
    property bool   allowVehicleLocationCenter:     false   ///< true: map will center/zoom to vehicle location one time
    //property bool   firstGCSPositionReceived:       false   ///< true: first gcs position update was responded to
    property bool   firstVehiclePositionReceived:   false   ///< true: first vehicle position update was responded to
    property bool   planView:                       false   ///< true: map being using for Plan view, items should be draggable

    readonly property real  maxZoomLevel: 20

    property var    _activeVehicle:             QGroundControl.multiVehicleManager.activeVehicle
    property var    _activeVehicleCoordinate:   _activeVehicle ? _activeVehicle.coordinate : QtPositioning.coordinate()

    function setVisibleRegion(region) {
        // TODO: Is this still necessary with Qt 5.11?
        // This works around a bug on Qt where if you set a visibleRegion and then the user moves or zooms the map
        // and then you set the same visibleRegion the map will not move/scale appropriately since it thinks there
        // is nothing to do.
        _map.visibleRegion = QtPositioning.rectangle(QtPositioning.coordinate(0, 0), QtPositioning.coordinate(0, 0))
        _map.visibleRegion = region
    }

    function _possiblyCenterToVehiclePosition() {
        if (!firstVehiclePositionReceived && allowVehicleLocationCenter && _activeVehicleCoordinate.isValid) {
            firstVehiclePositionReceived = true
            center = _activeVehicleCoordinate
            zoomLevel = QGroundControl.flightMapInitialZoom
        }
    }

    function centerToSpecifiedLocation() {
        mainWindow.showComponentDialog(specifyMapPositionDialog, qsTr("Specify Position"), mainWindow.showDialogDefaultWidth, StandardButton.Close)
    }

    Component {
        id: specifyMapPositionDialog
        EditPositionDialog {
            coordinate:             center
            onCoordinateChanged:    center = coordinate
        }
    }
    onZoomLevelChanged: {
         gridlineTimer.restart();
    }
    onCenterChanged: {
        gridlineTimer.restart();
    }
    onMapReadyChanged: {
        gridlineTimer.restart();
    }
    Component.onCompleted: {
        updateActiveMapType()
        _possiblyCenterToVehiclePosition()
        gridlineTimer.restart();
    }
    onHeightChanged: {
        gridlineTimer.restart();
    }
    onWidthChanged: {
        gridlineTimer.restart();
    }
    Connections {
        target:  QGroundControl.settingsManager.unitsSettings.geoCoordinateSystem
        function onRawValueChanged() { gridlineTimer.restart(); }
    }
    Connections {
        target: QGroundControl.settingsManager.appSettings.showMapGridlines
        function onRawValueChanged() { gridlineTimer.restart(); }
    }

    Timer {
        id: gridlineTimer
        interval: 100
        repeat: false
        running: false
        onTriggered: {
            if(QGroundControl.settingsManager.appSettings.showMapGridlines.rawValue)
            {
                if(_root.mapReady)
                {
                    var tl = toCoordinate(Qt.point(0,0))
                    var br = toCoordinate(Qt.point(width, height))
                    if(tl.isValid && br.isValid)
                    {
                        mapGridlineModel.updateGridlines(tl, br, zoomLevel)
                    }
                }
            }
            else
            {
                mapGridlineModel.clearGridlines()
            }
        }
    }
    MapItemView {
        model: mapGridlineModel
        delegate: MapPolyline{
            line.width: model.precision>1?2:model.precision<0?1:3
            //display lat and lon in black/white
            //display gzd in red
            //display 100km in green
            //display 10km and less in black/white
            line.color: (model.precision<0||model.precision>1)?(isSatelliteMap?"white":"black"):model.precision===0?"red":"green"
            path:       model.path
            z:          QGroundControl.zOrderTopMost
        }
    }

    // Center map to gcs location
    //onGcsPositionChanged: {
        //if (gcsPosition.isValid && allowGCSLocationCenter && !firstGCSPositionReceived && !firstVehiclePositionReceived) {
            //firstGCSPositionReceived = true
            //-- Only center on gsc if we have no vehicle (and we are supposed to do so)
            //var _activeVehicleCoordinate = _activeVehicle ? _activeVehicle.coordinate : QtPositioning.coordinate()
            //if(QGroundControl.settingsManager.flyViewSettings.keepMapCenteredOnVehicle.rawValue || !_activeVehicleCoordinate.isValid)
            //    center = gcsPosition
        //}
    //}

    function updateActiveMapType() {
        var settings =  QGroundControl.settingsManager.flightMapSettings
        var fullMapName = settings.mapProvider.value + " " + settings.mapType.value

        for (var i = 0; i < _map.supportedMapTypes.length; i++) {
            if (fullMapName === _map.supportedMapTypes[i].name) {
                _map.activeMapType = _map.supportedMapTypes[i]
                return
            }
        }
    }

    on_ActiveVehicleCoordinateChanged: _possiblyCenterToVehiclePosition()

    Connections {
        target:             QGroundControl.settingsManager.flightMapSettings.mapType
        function onRawValueChanged() { updateActiveMapType() }
    }

    Connections {
        target:             QGroundControl.settingsManager.flightMapSettings.mapProvider
        function onRawValueChanged() { updateActiveMapType() }
    }

    /// Ground Station location
    MapQuickItem {
        anchorPoint.x:  sourceItem.width / 2
        anchorPoint.y:  sourceItem.height / 2
        visible:        gcsPosition.isValid
        coordinate:     gcsPosition

        sourceItem: Image {
            id:             mapItemImage
            source:         isNaN(gcsHeading) ? "/res/QGCLogoFullecho" : "/res/QGCLogoArrow"
            mipmap:         true
            antialiasing:   true
            fillMode:       Image.PreserveAspectFit
            height:         ScreenTools.defaultFontPixelHeight * (isNaN(gcsHeading) ? 1.75 : 2.5 )
            sourceSize.height: height
            transform: Rotation {
                origin.x:       mapItemImage.width  / 2
                origin.y:       mapItemImage.height / 2
                angle:          isNaN(gcsHeading) ? 0 : gcsHeading
            }
        }
    }
} // Map
