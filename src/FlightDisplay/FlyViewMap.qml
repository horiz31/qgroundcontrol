/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtLocation                   5.3
import QtPositioning                5.3
import QtQuick.Dialogs              1.2
import QtQuick.Layouts              1.11

import QGroundControl               1.0
import QGroundControl.Airspace      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0
import
QGroundControl.FlightMap     1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0


FlightMap {
    id:                         _root
    allowGCSLocationCenter:     true
    allowVehicleLocationCenter: !_keepVehicleCentered
    planView:                   false
    zoomLevel:                  QGroundControl.flightMapZoom
    center:                     QGroundControl.flightMapPosition


    property Item pipState: _pipState
    QGCPipState {
        id:         _pipState
        pipOverlay: _pipOverlay
        isDark:     _isFullWindowItemDark
    }

    property var    altitudeSlider             //passed in so we can know if it is visible or not
    property var    rightPanelWidth
    property var    planMasterController
    property bool   pipMode:                    false   // true: map is shown in a small pip mode
    property var    toolInsets                          // Insets for the center viewport area

    property var    _activeVehicle:             QGroundControl.multiVehicleManager.activeVehicle
    property var    _planMasterController:      planMasterController
    property var    _geoFenceController:        planMasterController.geoFenceController
    property var    _rallyPointController:      planMasterController.rallyPointController
    property var    _activeVehicleCoordinate:   _activeVehicle ? _activeVehicle.coordinate : QtPositioning.coordinate()
    property real   _toolButtonTopMargin:       parent.height - mainWindow.height + (ScreenTools.defaultFontPixelHeight / 2)
    property real   _toolsMargin:               ScreenTools.defaultFontPixelWidth * 0.75
    property bool   _airspaceEnabled:           QGroundControl.airmapSupported ? (QGroundControl.settingsManager.airMapSettings.enableAirMap.rawValue && QGroundControl.airspaceManager.connected): false
    property var    _flyViewSettings:           QGroundControl.settingsManager.flyViewSettings
    property bool   _keepMapCenteredOnVehicle:  _flyViewSettings.keepMapCenteredOnVehicle.rawValue
    readonly property real  _hamburgerSize:     ScreenTools.defaultFontPixelHeight

    property bool   _disableVehicleTracking:    false
    property bool   _keepVehicleCentered:       pipMode ? true : false
    property bool   _saveZoomLevelSetting:      true

    property bool   _nextVisionGimbalAvailable:                 _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? false : true) : false
    property var    _videoSettings:             QGroundControl.settingsManager.videoSettings
    property var    _clickedCoordinate

    signal mouseCursorChanged(var coordinate)

    function updateAirspace(reset) {
        if(_airspaceEnabled) {
            var coordinateNW = _root.toCoordinate(Qt.point(0,0), false /* clipToViewPort */)
            var coordinateSE = _root.toCoordinate(Qt.point(width,height), false /* clipToViewPort */)
            if(coordinateNW.isValid && coordinateSE.isValid) {
                QGroundControl.airspaceManager.setROI(coordinateNW, coordinateSE, false /*planView*/, reset)
            }
        }
    }

    function _adjustMapZoomForPipMode() {
        _saveZoomLevelSetting = false
        if (pipMode) {
            if (QGroundControl.flightMapZoom > 3) {
                zoomLevel = QGroundControl.flightMapZoom - 3
            }
        } else {
            zoomLevel = QGroundControl.flightMapZoom
        }
        _saveZoomLevelSetting = true
    }

    Component {
        id: editPositionDialog

        EditPositionDialog {
            coordinate: _clickedCoordinate
            onCoordinateChanged:
            {
                if (coordinate !== mapMouseArea.clickCoord)
                {
                    console.log("coordinate changed")
                    mapClickIconItem.update(coordinate)
                    mapMouseArea.clickCoord = coordinate
                }
            }
        }
    }

   GPSUnitsController {
        id: gpsUnitsController
    }
    ATAKController {
        id: atakController
    }
    onPipModeChanged: _adjustMapZoomForPipMode()

    onVisibleChanged: {
        if (visible) {
            // Synchronize center position with Plan View
            center = QGroundControl.flightMapPosition
        }
    }

    onZoomLevelChanged: {
        if (_saveZoomLevelSetting) {
            QGroundControl.flightMapZoom = zoomLevel
            updateAirspace(false)
        }
    }
    onCenterChanged: {
        QGroundControl.flightMapPosition = center
        updateAirspace(false)
    }

    on_AirspaceEnabledChanged: {
        updateAirspace(true)
    }

    // We track whether the user has panned or not to correctly handle automatic map positioning
    Connections {
        target: gesture

        function onPanStarted() {       _disableVehicleTracking = true }
        function onFlickStarted() {     _disableVehicleTracking = true }
        function onPanFinished() {      panRecenterTimer.restart() }
        function onFlickFinished() {    panRecenterTimer.restart() }
    }

    function pointInRect(point, rect) {
        return point.x > rect.x &&
                point.x < rect.x + rect.width &&
                point.y > rect.y &&
                point.y < rect.y + rect.height;
    }

    property real _animatedLatitudeStart
    property real _animatedLatitudeStop
    property real _animatedLongitudeStart
    property real _animatedLongitudeStop
    property real animatedLatitude
    property real animatedLongitude

    onAnimatedLatitudeChanged: _root.center = QtPositioning.coordinate(animatedLatitude, animatedLongitude)
    onAnimatedLongitudeChanged: _root.center = QtPositioning.coordinate(animatedLatitude, animatedLongitude)

    NumberAnimation on animatedLatitude { id: animateLat; from: _animatedLatitudeStart; to: _animatedLatitudeStop; duration: 1000 }
    NumberAnimation on animatedLongitude { id: animateLong; from: _animatedLongitudeStart; to: _animatedLongitudeStop; duration: 1000 }

    function animatedMapRecenter(fromCoord, toCoord) {
        _animatedLatitudeStart = fromCoord.latitude
        _animatedLongitudeStart = fromCoord.longitude
        _animatedLatitudeStop = toCoord.latitude
        _animatedLongitudeStop = toCoord.longitude
        animateLat.start()
        animateLong.start()
    }

    function _insetRect() {
        return Qt.rect(toolInsets.leftEdgeCenterInset,
                       toolInsets.topEdgeCenterInset,
                       _root.width - toolInsets.leftEdgeCenterInset - toolInsets.rightEdgeCenterInset,
                       _root.height - toolInsets.topEdgeCenterInset - toolInsets.bottomEdgeCenterInset)
    }

    function recenterNeeded() {
        var vehiclePoint = _root.fromCoordinate(_activeVehicleCoordinate, false /* clipToViewport */)
        var insetRect = _insetRect()
        return !pointInRect(vehiclePoint, insetRect)
    }

    function updateMapToVehiclePosition() {
        if (animateLat.running || animateLong.running) {
            return
        }
        // We let FlightMap handle first vehicle position
        if (!_keepMapCenteredOnVehicle && firstVehiclePositionReceived && _activeVehicleCoordinate.isValid && !_disableVehicleTracking) {
            if (_keepVehicleCentered) {
                _root.center = _activeVehicleCoordinate
            } else {
                if (firstVehiclePositionReceived && recenterNeeded()) {
                    // Move the map such that the vehicle is centered within the inset area
                    var vehiclePoint = _root.fromCoordinate(_activeVehicleCoordinate, false /* clipToViewport */)
                    var insetRect = _insetRect()
                    var centerInsetPoint = Qt.point(insetRect.x + insetRect.width / 2, insetRect.y + insetRect.height / 2)
                    var centerOffset = Qt.point((_root.width / 2) - centerInsetPoint.x, (_root.height / 2) - centerInsetPoint.y)
                    var vehicleOffsetPoint = Qt.point(vehiclePoint.x + centerOffset.x, vehiclePoint.y + centerOffset.y)
                    var vehicleOffsetCoord = _root.toCoordinate(vehicleOffsetPoint, false /* clipToViewport */)
                    animatedMapRecenter(_root.center, vehicleOffsetCoord)
                }
            }
        }
    }

    on_ActiveVehicleCoordinateChanged: {
        if (_keepMapCenteredOnVehicle && _activeVehicleCoordinate.isValid && !_disableVehicleTracking) {
            _root.center = _activeVehicleCoordinate
        }
    }

    Timer {
        id:         panRecenterTimer
        interval:   10000
        running:    false
        onTriggered: {
                //_disableVehicleTracking = false
                //updateMapToVehiclePosition()
        }
    }

    Timer {
        interval:       500
        running:        true
        repeat:         true
        onTriggered:    updateMapToVehiclePosition()
    }

    QGCMapPalette { id: mapPal; lightColors: isSatelliteMap }

    Connections {
        target:                 _missionController
        ignoreUnknownSignals:   true
        function onNewItemsFromVehicle() {
            var visualItems = _missionController.visualItems
            if (visualItems && visualItems.count !== 1) {
                mapFitFunctions.fitMapViewportToMissionItems()
                firstVehiclePositionReceived = true
            }
        }
    }

    MapFitFunctions {
        id:                         mapFitFunctions // The name for this id cannot be changed without breaking references outside of this code. Beware!
        map:                        _root
        usePlannedHomePosition:     false
        planMasterController:       _planMasterController
    }

    ObstacleDistanceOverlayMap {
        id: obstacleDistance
        showText: !pipMode
    }

    // Add trajectory lines to the map
    MapPolyline {
        id:         trajectoryPolyline
        line.width: 3
        line.color: "red"
        z:          QGroundControl.zOrderTrajectoryLines
        visible:    !pipMode

        Connections {
            target:                 QGroundControl.multiVehicleManager
            function onActiveVehicleChanged(activeVehicle) {
                trajectoryPolyline.path = _activeVehicle ? _activeVehicle.trajectoryPoints.list() : []
            }
        }

        Connections {
            target:                 _activeVehicle ? _activeVehicle.trajectoryPoints : null
            onPointAdded:           trajectoryPolyline.addCoordinate(coordinate)
            onUpdateLastPoint:      trajectoryPolyline.replaceCoordinate(trajectoryPolyline.pathLength() - 1, coordinate)
            onPointsCleared:        trajectoryPolyline.path = []
        }
    }

    // Add the vehicles to the map
    MapItemView {
        model: QGroundControl.multiVehicleManager.vehicles
        delegate: VehicleMapItem {
            vehicle:        object
            coordinate:     object.coordinate
            map:            _root
            size:           pipMode ? ScreenTools.defaultFontPixelHeight * 2 : ScreenTools.defaultFontPixelHeight * 5
            z:              QGroundControl.zOrderVehicles
        }
    }
    // Add distance sensor view
    MapItemView{
        model: QGroundControl.multiVehicleManager.vehicles
        delegate: ProximityRadarMapView {
            vehicle:        object
            coordinate:     object.coordinate
            map:            _root
            z:              QGroundControl.zOrderVehicles
        }
    }
    // Add ADSB vehicles to the map
    MapItemView {
        model: QGroundControl.adsbVehicleManager.adsbVehicles
        delegate: VehicleMapItem {
            coordinate:     object.coordinate
            altitude:       object.altitude
            callsign:       object.callsign
            heading:        object.heading
            alert:          object.alert
            map:            _root
            z:              QGroundControl.zOrderVehicles
        }
    }

    // Add the items associated with each vehicles flight plan to the map
    Repeater {
        model: QGroundControl.multiVehicleManager.vehicles

        PlanMapItems {
            map:                    _root
            largeMapView:           !pipMode
            planMasterController:   masterController
            vehicle:                _vehicle

            property var _vehicle: object

            PlanMasterController {
                id: masterController
                Component.onCompleted: startStaticActiveVehicle(object)
            }
        }
    }

    MapItemView {
        model: pipMode ? undefined : _missionController.directionArrows

        delegate: MapLineArrow {
            fromCoord:      object ? object.coordinate1 : undefined
            toCoord:        object ? object.coordinate2 : undefined
            arrowPosition:  2
            z:              QGroundControl.zOrderWaypointLines
        }
    }

    // Allow custom builds to add map items
    CustomMapItems {
        map:            _root
        largeMapView:   !pipMode
    }

    GeoFenceMapVisuals {
        map:                    _root
        myGeoFenceController:   _geoFenceController
        interactive:            false
        planView:               false
        homePosition:           _activeVehicle && _activeVehicle.homePosition.isValid ? _activeVehicle.homePosition :  QtPositioning.coordinate()
    }

    // Rally points on map
    MapItemView {
        model: _rallyPointController.points

        delegate: MapQuickItem {
            id:             itemIndicator
            anchorPoint.x:  sourceItem.anchorPointX
            anchorPoint.y:  sourceItem.anchorPointY
            coordinate:     object.coordinate
            z:              QGroundControl.zOrderMapItems

            sourceItem: MissionItemIndexLabel {
                id:         itemIndexLabel
                label:      qsTr("R", "rally point map item label")
            }
        }
    }

    // Camera trigger points
    MapItemView {
        model: _activeVehicle ? _activeVehicle.cameraTriggerPoints : 0

        delegate: CameraTriggerIndicator {
            coordinate:     object.coordinate
            z:              QGroundControl.zOrderTopMost
        }
    }

    // NextVision Target visuals
    MapQuickItem {
        id:             nextVisionTargetIndicator
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        coordinate:     _activeVehicle ? _activeVehicle.nvTargetCoordinate : QtPositioning.coordinate()       
        visible:        targetVisible()

        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("+", "Gimbal Target")
        }
        function targetVisible()
        {
            if (!_activeVehicle)
                return false
            if ((isNaN(_activeVehicle.nvGimbal.groundCrossingLat.value) || (_activeVehicle.nvGimbal.groundCrossingLat.value === 400.0)))
                return false
            return _videoSettings.targetOverlay.value
        }

    }


    // Map Click Location visuals
    MapQuickItem {
        id:             mapClickIconItem
        visible:        false
        z:              QGroundControl.zOrderMapItems
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("Choose Action")
        }

        Connections {
            target: QGroundControl.multiVehicleManager
            function onActiveVehicleChanged(activeVehicle) {
                if (!activeVehicle) {
                    mapClickIconItem.visible = false
                }
            }
        }

        function show(coord) {
            mapClickIconItem.coordinate = coord
            mapClickIconItem.visible = true
        }

        function hide() {
            mapClickIconItem.visible = false
        }
        function update(coord) {
            mapClickIconItem.coordinate = coord
        }
    }

    // GoTo Location visuals
    MapQuickItem {
        id:             gotoLocationItem        
        visible:        _activeVehicle ? inGotoFlightMode : false
        z:              QGroundControl.zOrderMapItems
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("Guided Point", "Guided mode point")
        }

        property bool inGotoFlightMode: _activeVehicle ? _activeVehicle.flightMode === _activeVehicle.gotoFlightMode : false

        onInGotoFlightModeChanged: {
            if (inGotoFlightMode && !gotoLocationItem.visible)
            {
                gotoLocationItem.visible = true
            }
            else if (!inGotoFlightMode && gotoLocationItem.visible) {
                // Hide goto indicator when vehicle falls out of guided mode                
                gotoLocationItem.visible = false
            }            
        }

        Connections {
            target: QGroundControl.multiVehicleManager
            function onActiveVehicleChanged(activeVehicle)
            {
                if (!activeVehicle) {                   
                    gotoLocationItem.visible = false
                }
            }

        }
        Connections {
            target: _activeVehicle
            function onGuidedModeCoordinateChanged(guidedCoordinate) {
                gotoLocationItem.coordinate = guidedCoordinate
            }
        }

        function show(coord) {
            gotoLocationItem.coordinate = coord
            gotoLocationItem.visible = true
        }

        function update(coord) {
            gotoLocationItem.coordinate = coord
        }

        function hide() {           
        }

        function actionConfirmed() {
            // We leave the indicator visible. The handling for onInGuidedModeChanged will hide it.
        }

        function actionCancelled() {
            //update the coordinate back to what the vehicle is tracking as it's guided coordinate
            //visibility of if this is shown or not is handled by whether we are in guided mode or not
            gotoLocationItem.coordinate = _activeVehicle.guidedModeCoordinate
            //hide()
        }
    }


    // Guided Mode PLANNING visuals, this is shown during goto and pause planning to indicate the guided mode radius and direction in a grey circle
    QGCMapCircleVisuals {
        id:             guidedPlanMapCircle
        mapControl:     parent
        mapCircle:      _guidedPlanCircle
        visible:        false
        borderColor:    qgcPal.colorGrey

        centerDragHandleVisible: false
        interactive:             false

        property alias center:              _guidedPlanCircle.center
        property alias clockwiseRotation:   _guidedPlanCircle.clockwiseRotation
        property alias radius:              _guidedPlanCircle.radius.rawValue
        readonly property real defaultRadius: _activeVehicle ? _activeVehicle.guidedModeRadius : 150


        function setCenter(coord) {
            guidedPlanMapCircle.center = coord
        }
        function setRadius(radius)
        {
            guidedPlanMapCircle.radius = radius
        }
        function setClockwise(clockwise)
        {
            guidedPlanMapCircle.clockwiseRotation = clockwise
        }
        function show()
        {
            guidedPlanMapCircle.visible = true
        }

        function hide()
        {
            guidedPlanMapCircle.visible = false
        }
        function actionCancelled()
        {
            guidedPlanMapCircle.visible = false
        }
        function actionConfirmed()
        {
            guidedPlanMapCircle.visible = false
        }

        function lockOnVehicle()
        {
            if (_activeVehicle)
                guidedPlanMapCircle.center = _activeVehicle.coordinate
        }

        Component.onCompleted: globals.guidedControllerFlyView.guidedPlanMapCircle = guidedPlanMapCircle

        QGCMapCircle {
            id:                 _guidedPlanCircle
            interactive:        true
            radius.rawValue:    _activeVehicle ? _activeVehicle.guidedModePlannedRadius : 150
            showRotation:       true
            clockwiseRotation:  true
        }
    }

    // Guided Mode Radius visuals, these should show any time the vehicle is in guided mode, and indicate the radius and location
    QGCMapCircleVisuals {
        id:             guidedMapCircle
        mapControl:     parent
        mapCircle:      _guidedCircle
        visible:        _activeVehicle ? inGotoFlightMode : false
        borderColor:    qgcPal.colorGreen
        centerDragHandleVisible: false
        interactive:             false

        property alias center:              _guidedCircle.center
        property alias clockwiseRotation:   _guidedCircle.clockwiseRotation
        property alias radius:              _guidedCircle.radius.rawValue
        readonly property real defaultRadius: _activeVehicle ? _activeVehicle.guidedModeRadius : 150
        property bool inGotoFlightMode: _activeVehicle ? _activeVehicle.flightMode === _activeVehicle.gotoFlightMode : false

        /*
        onInGotoFlightModeChanged: {
            if (!inGotoFlightMode && guidedMapCircle.visible) {
                // Hide  when vehicle falls out of guided mode
                guidedMapCircle.visible = false
            }
            else if (inGotoFlightMode)
            {
                _guidedCircle.radius.rawValue = _activeVehicle.guidedModeRadius
                guidedMapCircle.visible = true;
                guidedMapCircle.borderColor = qgcPal.colorGreen
            }
        }
        */

        Connections {
            target: _activeVehicle
            function onGuidedModeRadiusChanged() {
                guidedMapCircle.radius = _activeVehicle.guidedModeRadius
            }
            function onGuidedModeCoordinateChanged(guidedCoordinate) {
                guidedMapCircle.center = guidedCoordinate
            }
            function onGuidedModeisClockwiseChanged(isClockwise) {
                guidedMapCircle.clockwiseRotation = isClockwise
            }

        }      

        QGCMapCircle {
            id:                 _guidedCircle
            interactive:        true
            radius.rawValue:    _activeVehicle ? _activeVehicle.guidedModeRadius : 150
            showRotation:       true
            clockwiseRotation:  true
        }
    }

    // Orbit editing visuals
    QGCMapCircleVisuals {
        id:             orbitMapCircle
        mapControl:     parent
        mapCircle:      _mapCircle
        visible:        false

        property alias center:              _mapCircle.center
        property alias clockwiseRotation:   _mapCircle.clockwiseRotation
        readonly property real defaultRadius: 30

        Connections {
            target: QGroundControl.multiVehicleManager
            function onActiveVehicleChanged(activeVehicle) {
                if (!activeVehicle) {
                    orbitMapCircle.visible = false
                }
            }
        }

        function show(coord) {
            _mapCircle.radius.rawValue = defaultRadius
            orbitMapCircle.center = coord
            orbitMapCircle.visible = true
        }

        function hide() {
            orbitMapCircle.visible = false
        }

        function actionConfirmed() {
            // Live orbit status is handled by telemetry so we hide here and telemetry will show again.
            hide()
        }

        function actionCancelled() {
            hide()
        }

        function radius() {
            return _mapCircle.radius.rawValue
        }

        Component.onCompleted: globals.guidedControllerFlyView.orbitMapCircle = orbitMapCircle

        QGCMapCircle {
            id:                 _mapCircle
            interactive:        true
            radius.rawValue:    30
            showRotation:       true
            clockwiseRotation:  true
        }
    }

    // ROI Location visuals
    MapQuickItem {
        id:             roiLocationItem
        visible:        _activeVehicle && _activeVehicle.isROIEnabled
        z:              QGroundControl.zOrderMapItems
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("ROI", "Make this a Region Of Interest")
        }

        //-- Visibilty controlled by actual state
        function show(coord) {
            roiLocationItem.coordinate = coord
        }

        function hide() {
        }

        function actionConfirmed() {
        }

        function actionCancelled() {
        }
    }

    // NextVision PTC Location visuals
    MapQuickItem {
        id:             nvPTCLocationItem
        visible:        false
        z:              QGroundControl.zOrderMapItems
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("PTC", "Point Camera to Location")
        }

        Connections {
            target: _activeVehicle
            onNvModeChanged: {
                if(nvMode !== "PTC"){
                  nvPTCLocationItem.visible = false
                }
            }
        }
        //-- Visibilty controlled by actual state
        function show(coord) {
            nvPTCLocationItem.coordinate = coord
            nvPTCLocationItem.visible = true
        }

        function hide() {
        }

        function actionConfirmed() {
        }

        function actionCancelled() {
        }
    }

    // Orbit telemetry visuals
    QGCMapCircleVisuals {
        id:             orbitTelemetryCircle
        mapControl:     parent
        mapCircle:      _activeVehicle ? _activeVehicle.orbitMapCircle : null
        visible:        _activeVehicle ? _activeVehicle.orbitActive : false
    }

    MapQuickItem {
        id:             orbitCenterIndicator
        anchorPoint.x:  sourceItem.anchorPointX
        anchorPoint.y:  sourceItem.anchorPointY
        coordinate:     _activeVehicle ? _activeVehicle.orbitMapCircle.center : QtPositioning.coordinate()
        visible:        orbitTelemetryCircle.visible

        sourceItem: MissionItemIndexLabel {
            checked:    true
            index:      -1
            label:      qsTr("Orbit", "Orbit waypoint")
        }
    }

    // Handle map clicks, opens a right side dialog
    MouseArea {
        id: mapMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        property var clickCoord

        onPressed:
        {
            if (mouse.button == Qt.RightButton)
                mouseAction(mouse)
        }

        onPressAndHold: {
           mouseAction(mouse);
        }

        onPositionChanged:
        {
            var currentLocation = _root.toCoordinate(Qt.point(mouse.x, mouse.y), false /* clipToViewPort */)
            //console.log(currentLocation.latitude + ", " + currentLocation.longitude)
            mouseCursorChanged(currentLocation)
        }

        function mouseAction(mouse)
        {
            orbitMapCircle.hide()
           // gotoLocationItem.hide()
            clickCoord = _root.toCoordinate(Qt.point(mouse.x, mouse.y), false /* clipToViewPort */)
            //show the clicked location on the map
            mapClickIconItem.show(clickCoord)
            //open side dialog
            mainWindow.showComponentDialog(
            mapClickActionDialogComponent,
            qsTr("Map Click Action"),
            mainWindow.showDialogDefaultWidth,
            StandardButton.Close)

        }
        Component {
            id: mapClickActionDialogComponent

            QGCViewDialog {
                property var activeVehicleCopy: _activeVehicle
                onActiveVehicleCopyChanged:
                    if (!activeVehicleCopy) {
                        hideDialog()
                        mapClickIconItem.hide()
                    }
                function reject() {
                    mapClickIconItem.hide()
                    hideDialog()
                }

                QGCFlickable {
                    anchors.fill:   parent
                    contentHeight:  column.height

                    ColumnLayout {
                        id:                 column
                        anchors.margins:    _margins
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        spacing:            ScreenTools.defaultFontPixelHeight

                        //todo, this button should only show when the aircraft is in certain conditions
                        //add sections: Position Info (lat/lon and mgrs display, copy to clipboard), Navigation Actions (guided, roi, orbit), ATAK Actions
                        QGCLabel {
                            Layout.fillWidth:       true
                            text:                   qsTr("Camera Actions:")
                            font.family:            ScreenTools.demiboldFontFamily
                            visible:                !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                        }

                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:               qsTr("Point at location")
                            visible:            !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                            onClicked: {
                                mapClickIconItem.hide()
                                hideDialog()
                                nvPTCLocationItem.show(mapMouseArea.clickCoord)
                                if(_activeVehicle)
                                    joystickManager.cameraManagement.pointToCoordinate(mapMouseArea.clickCoord.latitude, mapMouseArea.clickCoord.longitude)

                            }
                        }
                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:               qsTr("Point at location, then Hold")
                            visible:            !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                            onClicked: {
                                mapClickIconItem.hide()
                                hideDialog()
                                nvPTCLocationItem.show(mapMouseArea.clickCoord)
                                if(_activeVehicle)
                                    joystickManager.cameraManagement.pointToCoordinateAndHold(mapMouseArea.clickCoord.latitude, mapMouseArea.clickCoord.longitude)

                            }
                        }
                        Rectangle {
                            Layout.fillWidth:   true
                            color:              qgcPal.text
                            height:             1
                            visible:            QGroundControl.videoManager.hasVideo && !QGroundControl.videoManager.fullScreen
                        }

                        QGCLabel {
                            Layout.fillWidth:       true
                            text:                   qsTr("Navigation Actions:")
                            font.family:    ScreenTools.demiboldFontFamily
                            //horizontalAlignment:    Text.AlignHCenter
                            visible:                globals.guidedControllerFlyView.showGotoLocation | globals.guidedControllerFlyView.showOrbit | globals.guidedControllerFlyView.showROI
                        }
                        RowLayout {
                            QGCButton {
                                Layout.fillWidth:   true
                                backRadius:         4
                                text:               qsTr("Go to location")
                                visible:            globals.guidedControllerFlyView.showGotoLocation
                                onClicked: {
                                    mapClickIconItem.hide()
                                    gotoLocationItem.show(mapMouseArea.clickCoord)
                                    guidedPlanMapCircle.setCenter(mapMouseArea.clickCoord)
                                    guidedPlanMapCircle.setRadius(_activeVehicle.guidedModeRadius)
                                    guidedPlanMapCircle.setClockwise(true)
                                    hideDialog()
                                    globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionGoto, mapMouseArea.clickCoord, gotoLocationItem, guidedPlanMapCircle)

                                }
                            }
                            QGCColoredImage {
                                 id:                     hamburger
                                 //anchors.margins:        _margin
                                 //anchors.right:          parent.right
                                 //anchors.verticalCenter: topRowLayout.verticalCenter
                                 width:                  _hamburgerSize
                                 height:                 _hamburgerSize
                                 sourceSize.height:      _hamburgerSize
                                 source:                 "qrc:/qmlimages/Hamburger.svg"
                                 visible:                true
                                 color:                  qgcPal.text

                                 QGCMouseArea {
                                     fillItem:   hamburger
                                     onClicked: {
                                         //currentItemScope.focus = true
                                         _clickedCoordinate = mapClickIconItem.coordinate
                                         mainWindow.showComponentDialog(editPositionDialog, qsTr("Edit Guided Position"), mainWindow.showDialogDefaultWidth, StandardButton.Close)

                                         //if needs expand, then use the popup menu below, but for now going directly to Edit Position
                                         //hamburgerMenu.popup()
                                     }

                                     QGCMenu {
                                         id: hamburgerMenu

                                         QGCMenuItem {
                                             text:           qsTr("Edit position...")
                                             visible:        true
                                             onTriggered:
                                             {
                                                 _clickedCoordinate = mapClickIconItem.coordinate
                                                 mainWindow.showComponentDialog(editPositionDialog, qsTr("Edit Position"), mainWindow.showDialogDefaultWidth, StandardButton.Close)
                                             }
                                         }


                                     }
                                 }
                             }
                        }

                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:               qsTr("Go to and point camera at location")
                            visible:            globals.guidedControllerFlyView.showGotoLocation && !QGroundControl.videoManager.fullScreen && _nextVisionGimbalAvailable
                            onClicked: {
                                mapClickIconItem.hide()
                                gotoLocationItem.show(mapMouseArea.clickCoord)
                                guidedPlanMapCircle.setCenter(mapMouseArea.clickCoord)
                                guidedPlanMapCircle.setRadius(_activeVehicle.guidedModeRadius)
                                guidedPlanMapCircle.setClockwise(true)

                                hideDialog()
                                if(_activeVehicle)
                                    joystickManager.cameraManagement.pointToCoordinate(mapMouseArea.clickCoord.latitude, mapMouseArea.clickCoord.longitude)

                                globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionGoto, mapMouseArea.clickCoord, gotoLocationItem, guidedPlanMapCircle)


                            }
                        }
                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:           qsTr("Orbit at location")
                            visible:        globals.guidedControllerFlyView.showOrbit

                            onClicked: {
                                mapClickIconItem.hide()
                                orbitMapCircle.show(mapMouseArea.clickCoord)
                                hideDialog()
                                globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionOrbit, mapMouseArea.clickCoord, orbitMapCircle)
                            }
                        }

                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:           qsTr("ROI at location")
                            visible:        globals.guidedControllerFlyView.showROI

                            onClicked: {
                                mapClickIconItem.hide()
                                roiLocationItem.show(mapMouseArea.clickCoord)
                                hideDialog()
                                globals.guidedControllerFlyView.confirmAction(globals.guidedControllerFlyView.actionROI, mapMouseArea.clickCoord, roiLocationItem)
                            }
                        }
                        Rectangle {
                            Layout.fillWidth:   true
                            color:              qgcPal.text
                            height:             1
                            visible:            globals.guidedControllerFlyView.showGotoLocation | globals.guidedControllerFlyView.showOrbit | globals.guidedControllerFlyView.showROI
                        }
                        QGCLabel {
                            Layout.fillWidth:       true
                            text:                   qsTr("Position Information:")
                            font.family:    ScreenTools.demiboldFontFamily
                            visible:                true
                        }

                        QGCLabel {
                            Layout.fillWidth:       true

                            text:                   mapMouseArea.clickCoord ? qsTr("Lat,Lon: ") + mapMouseArea.clickCoord.latitude.toFixed(7) + ", " + mapMouseArea.clickCoord.longitude.toFixed(7) : ""
                            //horizontalAlignment:    Text.AlignHCenter
                            visible:                true
                        }

                        QGCLabel {

                            Layout.fillWidth:       true
                            text:                   mapMouseArea.clickCoord ? qsTr("MGRS: ") + gpsUnitsController.convertToMGRS(mapMouseArea.clickCoord)  : ""
                            //horizontalAlignment:    Text.AlignHCenter
                            visible:                true
                        }

                        QGCLabel {

                            Layout.fillWidth:       true
                            text:                   (mapMouseArea.clickCoord && _activeVehicle) ? qsTr("Range: ") + mapMouseArea.clickCoord.distanceTo(_activeVehicle.coordinate).toFixed(1) + " " + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString  : ""
                            visible:                true
                        }

                        QGCLabel {
                            Layout.topMargin: 1
                            Layout.fillWidth:       true
                            text:                   (mapMouseArea.clickCoord && _activeVehicle) ? qsTr("Bearing: ") + _activeVehicle.coordinate.azimuthTo(mapMouseArea.clickCoord).toFixed(1) + " °" : ""
                            visible:                true
                        }
                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:               qsTr("Copy Coordinates to Clipboard")
                            visible:            true
                            onClicked: {
                                textEdit.text = mapMouseArea.clickCoord.latitude.toFixed(7) + ", " + mapMouseArea.clickCoord.longitude.toFixed(7)
                                textEdit.selectAll()
                                textEdit.copy()
                                mapClickIconItem.hide()
                                hideDialog()
                            }
                        }
                        TextEdit {
                               id: textEdit
                               visible: false
                        }

                        Rectangle {
                            Layout.fillWidth:   true
                            color:              qgcPal.text
                            height:             1
                        }


                        QGCLabel {
                            Layout.fillWidth:       true
                            text:                   qsTr("ATAK Target Actions:")
                            font.family:    ScreenTools.demiboldFontFamily
                            visible:                true
                        }

                        QGCButton {
                            Layout.fillWidth:   true
                            backRadius:         4
                            text:               qsTr("Create ATAK Target")
                            visible:            true
                            onClicked:{
                                 hideDialog()
                                 mapClickIconItem.hide()
                                 mainWindow.showPopupDialogFromComponent(atakDialogComponent)
                          }
                        }
                    }
                }
            }
        }
    }

    MapClickActionDialog {
        id: mapClickActionDialog
        missionController:      _missionController
    }
    // Airspace overlap support
    MapItemView {
        model:              _airspaceEnabled && QGroundControl.settingsManager.airMapSettings.enableAirspace && QGroundControl.airspaceManager.airspaceVisible ? QGroundControl.airspaceManager.airspaces.circles : []
        delegate: MapCircle {
            center:         object.center
            radius:         object.radius
            color:          object.color
            border.color:   object.lineColor
            border.width:   object.lineWidth
        }
    }

    MapItemView {
        model:              _airspaceEnabled && QGroundControl.settingsManager.airMapSettings.enableAirspace && QGroundControl.airspaceManager.airspaceVisible ? QGroundControl.airspaceManager.airspaces.polygons : []
        delegate: MapPolygon {
            path:           object.polygon
            color:          object.color
            border.color:   object.lineColor
            border.width:   object.lineWidth
        }
    }

    MapScale {
        id:                 mapScale
        anchors.margins:    _toolsMargin
        anchors.left:       parent.left
        anchors.top:        parent.top
        mapControl:         _root
        buttonsOnLeft:      false
        visible:            !ScreenTools.isTinyScreen && QGroundControl.corePlugin.options.flyView.showMapScale && mapControl.pipState.state === mapControl.pipState.windowState

        property real centerInset: visible ? parent.height - y : 0
    }

    Component {
        id: atakDialogComponent


        QGCPopupDialog {
            title:      qsTr("Create ATAK Target")
            buttons:    StandardButton.Close
            ColumnLayout {
                id: atakCol
                Layout.fillWidth:   true
                QGCLabel {
                    text:           qsTr("Location: " + mapMouseArea.clickCoord.latitude.toFixed(7) + ", " + mapMouseArea.clickCoord.longitude.toFixed(7))
                }
                Item {
                    Layout.fillWidth:   true
                    height: ScreenTools.defaultFontPixelWidth * 1
                }
                GridLayout {
                    columnSpacing:      ScreenTools.defaultFontPixelWidth * 2
                    columns: 2


                    QGCLabel {
                        text:           qsTr("Target Type:")
                    }
                    QGCComboBox {
                        id:             atakCombo
                        model:          atakController.cotTypes
                        currentIndex:   atakController.cotType
                        sizeToContents: true
                        onActivated: {
                            atakController.cotType = index;
                        }
                    }
                    QGCLabel {
                        text:           qsTr("Target Name (UID):")
                    }
                    QGCTextField {  //probably needs to be a factbox
                        id:    atakUid
                        placeholderText:    qsTr("Optional")
                    }
                    QGCLabel {
                        text:           qsTr("Time Until Invalid:")
                    }
                    QGCComboBox {
                        model:         atakController.staleMinuteList
                        currentIndex:  atakController.staleMinutes
                        sizeToContents: true
                        onActivated: {
                            atakController.staleMinutes = index;
                        }
                    }


                }
                Item {
                    Layout.fillWidth:   true
                    height: ScreenTools.defaultFontPixelWidth * 1
                }
                GridLayout {
                    Layout.fillWidth:   true
                    Layout.alignment:   Qt.AlignHCenter
                    columnSpacing:      ScreenTools.defaultFontPixelWidth * 2
                    columns: 2

                    QGCButton {
                        text:       qsTr("Cancel")
                        onClicked: {
                          hideDialog();
                        }
                    }
                    QGCButton {
                        text:       qsTr("Send to ATAK")
                        onClicked: {
                           atakController.send(mapMouseArea.clickCoord, atakUid.text)
                           hideDialog();
                        }
                    }
                }
            }
        }
    }

}
