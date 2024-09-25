/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                  2.4
import QtPositioning            5.2
import QtQuick.Layouts          1.2
import QtQuick.Controls         1.4
import QtQuick.Dialogs          1.2
import QtGraphicalEffects       1.0

import QGroundControl                   1.0
import QGroundControl.ScreenTools       1.0
import QGroundControl.Controls          1.0
import QGroundControl.Palette           1.0
import QGroundControl.Vehicle           1.0
import QGroundControl.Controllers       1.0
import QGroundControl.FactSystem        1.0
import QGroundControl.FactControls      1.0

Rectangle {
    id: nvMainPanel
    height:     mainLayout.height + (_margins * 1)
    //color:      "#80000000"
    color:      qgcPal.window
    radius:     _margins
    //border.width: 2
    //border.color: qgcPal.window
    //visible:    _nextVisionGimbalAvailable// && multiVehiclePanelSelector.showSingleVehiclePanel
    visible:    _activeVehicle// && multiVehiclePanelSelector.showSingleVehiclePanel
    z:      QGroundControl.zOrderTopMost
    MouseArea {
        anchors.fill:   parent
        propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
    }
    Connections {
        target: joystickManager.activeJoystick
        onRollPitchEnabled: {
            if (!value)
                border.color = qgcPal.colorBlue
            else
                border.color = qgcPal.window //"#80000000"
        }
    }
    Connections {
        target: QGroundControl.multiVehicleManager.activeVehicle
        onNvModeChanged: {
            _currentNvMode = _activeVehicle.nvGimbal.mode.value;
        }
    }
    property real   _margins:                                   ScreenTools.defaultFontPixelHeight / 2
    property var    _activeVehicle:                             QGroundControl.multiVehicleManager.activeVehicle
    property bool   _isArmed:                                   _activeVehicle ? (_activeVehicle.armed) : false
    property bool   _isFlying:                                  _activeVehicle ? (_activeVehicle.flying) : false

    // The following properties relate to a simple camera
    property var    _flyViewSettings:                           QGroundControl.settingsManager.flyViewSettings
    property bool   _simpleCameraAvailable:                     !_mavlinkCamera && _activeVehicle && _flyViewSettings.showSimpleCameraControl.rawValue
    property bool   _nextVisionGimbalAvailable:                 _activeVehicle ? (isNaN(_activeVehicle.nvGimbal.nvVersion.value) ? false : true) : false
    property bool   _onlySimpleCameraAvailable:                 !_anyVideoStreamAvailable && _simpleCameraAvailable
    property bool   _simpleCameraIsShootingInCurrentMode:       _onlySimpleCameraAvailable && !_simplePhotoCaptureIsIdle

    // The following properties relate to a simple video stream
    property bool   _videoStreamAvailable:                      _videoStreamManager.hasVideo
    property var    _videoStreamSettings:                       QGroundControl.settingsManager.videoSettings
    property var    _videoStreamManager:                        QGroundControl.videoManager
    property bool   _videoStreamAllowsPhotoWhileRecording:      true
    property bool   _videoStreamIsStreaming:                    _videoStreamManager.streaming
    property bool   _simplePhotoCaptureIsIdle:             true
    property bool   _videoStreamRecording:                      _videoStreamManager.recording
    property bool   _recordButtonBusy:                          _videoStreamManager.recordTransitionInProgress
    property bool   _videoStreamCanShoot:                       _videoStreamIsStreaming
    property bool   _videoStreamIsShootingInCurrentMode:        _videoStreamInPhotoMode ? !_simplePhotoCaptureIsIdle : _isRecording
    property bool   _videoStreamInPhotoMode:                    false

    // The following properties relate to a mavlink protocol camera
    property var    _mavlinkCameraManager:                      _activeVehicle ? _activeVehicle.cameraManager : null
    property int    _mavlinkCameraManagerCurCameraIndex:        _mavlinkCameraManager ? _mavlinkCameraManager.currentCamera : -1
    property bool   _noMavlinkCameras:                          _mavlinkCameraManager ? _mavlinkCameraManager.cameras.count === 0 : true
    property var    _mavlinkCamera:                             !_noMavlinkCameras ? (_mavlinkCameraManager.cameras.get(_mavlinkCameraManagerCurCameraIndex) && _mavlinkCameraManager.cameras.get(_mavlinkCameraManagerCurCameraIndex).paramComplete ? _mavlinkCameraManager.cameras.get(_mavlinkCameraManagerCurCameraIndex) : null) : null
    property bool   _multipleMavlinkCameras:                    _mavlinkCameraManager ? _mavlinkCameraManager.cameras.count > 1 : false
    property string _mavlinkCameraName:                         _mavlinkCamera && _multipleMavlinkCameras ? _mavlinkCamera.modelName : ""
    property bool   _noMavlinkCameraStreams:                    _mavlinkCamera ? _mavlinkCamera.streamLabels.length : true
    property bool   _multipleMavlinkCameraStreams:              _mavlinkCamera ? _mavlinkCamera.streamLabels.length > 1 : false
    property int    _mavlinCameraCurStreamIndex:                _mavlinkCamera ? _mavlinkCamera.currentStream : -1
    property bool   _mavlinkCameraHasThermalVideoStream:        _mavlinkCamera ? _mavlinkCamera.thermalStreamInstance : false
    property bool   _mavlinkCameraModeUndefined:                _mavlinkCamera ? _mavlinkCamera.cameraMode === QGCCameraControl.CAM_MODE_UNDEFINED : true
    property bool   _mavlinkCameraInVideoMode:                  _mavlinkCamera ? _mavlinkCamera.cameraMode === QGCCameraControl.CAM_MODE_VIDEO : false
    property bool   _mavlinkCameraInPhotoMode:                  _mavlinkCamera ? _mavlinkCamera.cameraMode === QGCCameraControl.CAM_MODE_PHOTO : false
    property bool   _mavlinkCameraElapsedMode:                  _mavlinkCamera && _mavlinkCamera.cameraMode === QGCCameraControl.CAM_MODE_PHOTO && _mavlinkCamera.photoMode === QGCCameraControl.PHOTO_CAPTURE_TIMELAPSE
    property bool   _mavlinkCameraHasModes:                     _mavlinkCamera && _mavlinkCamera.hasModes
    property bool   _mavlinkCameraVideoIsRecording:             _mavlinkCamera && _mavlinkCamera.videoStatus === QGCCameraControl.VIDEO_CAPTURE_STATUS_RUNNING
    property bool   _mavlinkCameraPhotoCaptureIsIdle:           _mavlinkCamera && (_mavlinkCamera.photoStatus === QGCCameraControl.PHOTO_CAPTURE_IDLE || _mavlinkCamera.photoStatus >= QGCCameraControl.PHOTO_CAPTURE_LAST)
    property bool   _mavlinkCameraStorageReady:                 _mavlinkCamera && _mavlinkCamera.storageStatus === QGCCameraControl.STORAGE_READY
    property bool   _mavlinkCameraBatteryReady:                 _mavlinkCamera && _mavlinkCamera.batteryRemaining >= 0
    property bool   _mavlinkCameraStorageSupported:             _mavlinkCamera && _mavlinkCamera.storageStatus !== QGCCameraControl.STORAGE_NOT_SUPPORTED
    property bool   _mavlinkCameraAllowsPhotoWhileRecording:    false
    property bool   _mavlinkCameraCanShoot:                     (!_mavlinkCameraModeUndefined && ((_mavlinkCameraStorageReady && _mavlinkCamera.storageFree > 0) || !_mavlinkCameraStorageSupported)) || _videoStreamManager.streaming
    property bool   _mavlinkCameraIsShooting:                   ((_mavlinkCameraInVideoMode && _mavlinkCameraVideoIsRecording) || (_mavlinkCameraInPhotoMode && !_mavlinkCameraPhotoCaptureIsIdle)) || _videoStreamManager.recording

    // The following settings and functions unify between a mavlink camera and a simple video stream for simple access

    property bool   _anyVideoStreamAvailable:                   _videoStreamManager.hasVideo || _nextVisionGimbalAvailable
    property string _cameraName:                                _mavlinkCamera ? _mavlinkCameraName : ""
    property bool   _showModeIndicator:                         _mavlinkCamera ? _mavlinkCameraHasModes : _videoStreamManager.hasVideo
    property bool   _modeIndicatorPhotoMode:                    _mavlinkCamera ? _mavlinkCameraInPhotoMode : _videoStreamInPhotoMode || _onlySimpleCameraAvailable
    property bool   _allowsPhotoWhileRecording:                  _mavlinkCamera ? _mavlinkCameraAllowsPhotoWhileRecording : _videoStreamAllowsPhotoWhileRecording
    property bool   _switchToPhotoModeAllowed:                  !_modeIndicatorPhotoMode && (_mavlinkCamera ? !_mavlinkCameraIsShooting : true)
    property bool   _switchToVideoModeAllowed:                  _modeIndicatorPhotoMode && (_mavlinkCamera ? !_mavlinkCameraIsShooting : true)
    property bool   _videoIsRecording:                          _mavlinkCamera ? _mavlinkCameraIsShooting : _videoStreamRecording
    property bool   _canShootInCurrentMode:                     _mavlinkCamera ? _mavlinkCameraCanShoot : _videoStreamCanShoot || _simpleCameraAvailable
    //property bool   _isShootingInCurrentMode:                   _mavlinkCamera ? _mavlinkCameraIsShooting : _videoStreamIsShootingInCurrentMode
    property bool   _isShootingInCurrentMode:                   _videoStreamIsShootingInCurrentMode
    property bool   _nvRecording:                               _activeVehicle? _activeVehicle.nvGimbal.isRecording.value === 1 : false
    property bool   _isRecording:                               _videoStreamRecording || _nvRecording
    property bool   _nvDayMode:                                  _activeVehicle ? _activeVehicle.nvGimbal.activeSensor.value === 0 : false
    property bool   _nvIRMode:                                  _activeVehicle ? _activeVehicle.nvGimbal.activeSensor.value === 1 : false
    property string _nvSnapShotStatus:                          _activeVehicle ? ((_activeVehicle.nvGimbal.isSnapshot.value === 0) ? qsTr("Idle") : qsTr("Busy")) : "Unknown"
    property bool   _remoteRecording:                           _videoStreamSettings.remoteRecording.rawValue === 1 ? true : false
    property bool   _autoRecording:                             _videoStreamSettings.recordOnFlying.rawValue
    property bool   _autoNuc:                                   _videoStreamSettings.autoNuc.rawValue
    property int    _nucPeriod:                                  _videoStreamSettings.nucPeriod.rawValue * 1000
    property string _currentNvMode:                             _activeVehicle ? _activeVehicle.nvGimbal.mode.value : "Observation"


    on_IsFlyingChanged: {
        if (_isFlying && _isArmed && _autoRecording && _nextVisionGimbalAvailable && !_videoStreamInPhotoMode)
        {
            //flying just changed and we are now flying, and autorecording is enabled, so if we aren't recording, get to it
            if (!_videoStreamManager.recording) {
                _videoStreamManager.startRecording()
                _activeVehicle.say("Recording Started");
                if (_nextVisionGimbalAvailable && !_nvRecording && _remoteRecording) //start remote recording (if enabled)
                {
                    joystickManager.cameraManagement.setSysRecOnCommand(0);
                    joystickManager.cameraManagement.setRecordChan1OnAfterDelay(1000);
                }
            }
        }
    }

    on_IsArmedChanged: {
        if (!_isArmed && _autoRecording && _nextVisionGimbalAvailable && !_videoStreamInPhotoMode)
        {
            //we have landed/disarmed and autorecording is enabled, if recording, stop
            if (_videoStreamManager.recording) {
                _videoStreamManager.stopRecording()
                if (_nextVisionGimbalAvailable && _nvRecording) {
                   joystickManager.cameraManagement.setSysRecOffCommand(0);
                   joystickManager.cameraManagement.setRecordChan1OffAfterDelay(1000);
                }

            }
        }
    }

    ListModel {
        id: irColorModel

        ListElement {
            text:       qsTr("Color")
        }
        ListElement {
            text:       qsTr("White Hot")
        }
        ListElement {
            text:       qsTr("Black Hot")
        }
        ListElement {
            text:       qsTr("Color Inverse")
        }
    }

    function setCameraMode(photoMode) {
        _videoStreamInPhotoMode = photoMode
        if (_mavlinkCamera) {
            if (_mavlinkCameraInPhotoMode) {
                _mavlinkCamera.setVideoMode()
            } else {
                _mavlinkCamera.setPhotoMode()
            }
        }
    }

    function toggleShooting() {
        console.log("toggleShooting", _anyVideoStreamAvailable)
        // handle local recording
        if (_anyVideoStreamAvailable)
        {
            if (_videoStreamInPhotoMode) //system is in PHOTO mode
            {
                _simplePhotoCaptureIsIdle = false
                _videoStreamManager.grabImage()
                simplePhotoCaptureTimer.start()
                if (_nextVisionGimbalAvailable & _nvSnapShotStatus === "Idle") {
                   console.log("nextvision snapshot");
                   joystickManager.cameraManagement.setSysSnapshotCommand(0);

                }

            }
            else   //system is in VIDEO MODE
            {
                if (_videoStreamManager.recording) {
                    console.log("stop local recording");
                    _videoStreamManager.stopRecording()
                    _activeVehicle.say("Recording Stopped");
                    //stop nextvision recording (if it is actually recording)
                    if (_nextVisionGimbalAvailable & _nvRecording) {
                       console.log("nextvision recording stop");
                       joystickManager.cameraManagement.setSysRecOffCommand(0);
                       joystickManager.cameraManagement.setRecordChan1OffAfterDelay(1000);
                    }

                } else {
                    console.log("start local recording");
                    _videoStreamManager.startRecording()
                    _activeVehicle.say("Recording Started");
                    if (_nextVisionGimbalAvailable & !_nvRecording & _remoteRecording) //start remote recording (if enabled)
                    {
                        console.log("nextvision recording start");
                        joystickManager.cameraManagement.setSysRecOnCommand(0);
                        //record channel 1 as well
                       joystickManager.cameraManagement.setRecordChan1OnAfterDelay(1000);
                    }
                    else
                    {
                        console.log("nextvision recording start skipped");
                    }
                }
            }
        }

    }

    Timer {
        id:             autoNucTimer
        interval:       _nucPeriod
        running:        _autoNuc
        repeat:         true
        onTriggered:    {

            if (_nextVisionGimbalAvailable && _nvIRMode )
            {
                 joystickManager.cameraManagement.setSysIrNUCCommand()
            }

        }
    }

    Timer {
        id:             simplePhotoCaptureTimer
        interval:       500
        onTriggered:    _simplePhotoCaptureIsIdle = true
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }


    ColumnLayout {
        id:                         mainLayout
        anchors.margins:            _margins
        anchors.top:                parent.top
        anchors.horizontalCenter:   parent.horizontalCenter
        spacing:                    ScreenTools.defaultFontPixelHeight / 2
        visible:                    _activeVehicle
        RowLayout{
            id: pauseSettingsAndTargetButtons
            Layout.alignment: Qt.AlignHCenter
            visible: _activeVehicle
            width: parent.width
            QGCColoredImage {
                Layout.alignment:   Qt.AlignLeft
                source:             "/res/target.svg"
                mipmap:             true
                height:             ScreenTools.defaultFontPixelHeight * 1.3
                width:              height
                sourceSize.height:  height
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit
                visible:            _nextVisionGimbalAvailable

                QGCMouseArea {
                    fillItem:   parent
                    onClicked:  {
                        _activeVehicle.showNvQuickPanel()
                    }
                }
            }

            // Photo/Video Mode Selector
            // IMPORTANT: This control supports both mavlink cameras and simple video streams. Do no reference anything here which is not
            // using the unified properties/functions.
            Rectangle {
                id:                 photoVidSwitch
                Layout.alignment:   Qt.AlignHCenter
                height:             ScreenTools.defaultFontPixelHeight * 2.0
                width:              height*2
                color:              qgcPal.windowShadeLight
                radius:             height * 0.5
                visible:            _nextVisionGimbalAvailable
                MouseArea {
                    anchors.fill:   parent
                    enabled:        true
                }

                //-- Video Mode
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width:                  parent.height
                    height:                 parent.height
                    color:                  _modeIndicatorPhotoMode ? qgcPal.windowShadeLight : qgcPal.window
                    radius:                 height * 0.5
                    anchors.left:           parent.left
                    border.color:           qgcPal.text
                    border.width:           _modeIndicatorPhotoMode ? 0 : 1

                    QGCColoredImage {
                        height:             parent.height * 0.5
                        width:              height
                        anchors.centerIn:   parent
                        source:             "/qmlimages/camera_video.svg"
                        fillMode:           Image.PreserveAspectFit
                        sourceSize.height:  height
                        color:              _modeIndicatorPhotoMode ? qgcPal.text : qgcPal.colorGreen
                        MouseArea {
                            anchors.fill:   parent
                            enabled:        _switchToVideoModeAllowed
                            onClicked:      setCameraMode(false)
                        }
                    }
                }
                //-- Photo Mode
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width:                  parent.height
                    height:                 parent.height
                    color:                  _modeIndicatorPhotoMode ? qgcPal.window : qgcPal.windowShadeLight
                    radius:                 height * 0.5
                    anchors.right:          parent.right
                    border.color:           qgcPal.text
                    border.width:           _modeIndicatorPhotoMode ? 1 : 0
                    QGCColoredImage {
                        height:             parent.height * 0.5
                        width:              height
                        anchors.centerIn:   parent
                        source:             "/qmlimages/camera_photo.svg"
                        fillMode:           Image.PreserveAspectFit
                        sourceSize.height:  height
                        color:              _modeIndicatorPhotoMode ? qgcPal.colorGreen : qgcPal.text
                        MouseArea {
                            anchors.fill:   parent
                            enabled:        _switchToPhotoModeAllowed
                            onClicked:      setCameraMode(true)
                        }
                    }
                }
            }

            QGCColoredImage {
                Layout.alignment:   Qt.AlignRight
                source:             "/res/gear-black.svg"
                mipmap:             true
                height:             ScreenTools.defaultFontPixelHeight * 1.3
                width:              height
                sourceSize.height:  height
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit
                visible:            ScreenTools.isMobile && _nextVisionGimbalAvailable

                QGCMouseArea {
                    fillItem:   parent
                    onClicked:  {
                        mainWindow.showPopupDialogFromComponent(settingsDialogComponent)
                    }
                }
            }
        }

        // Take Photo, Start/Stop Video button
        // IMPORTANT: This control supports both mavlink cameras and simple video streams. Do no reference anything here which is not
        // using the unified properties/functions.
        Rectangle {
            id: redRect
            Layout.alignment:   Qt.AlignHCenter
            //Layout.topMargin:   ScreenTools.defaultFontPixelWidth
            //Layout.bottomMargin:   ScreenTools.defaultFontPixelWidth
            color:              Qt.rgba(0,0,0,0)
            width:              ScreenTools.defaultFontPixelWidth * 8
            height:             width
            radius:             width * 0.5
            border.color:       qgcPal.buttonText
            border.width:       4
            visible:            _activeVehicle
            //visible:            _nextVisionGimbalAvailable

            SequentialAnimation {
                        id: anim

                        // Expand the button
                        PropertyAnimation {
                            target: redRect
                            property: "scale"
                            to: 1.2
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }

                        // Shrink back to normal
                        PropertyAnimation {
                            target: redRect
                            property: "scale"
                            to: 1.0
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

            Rectangle {
                anchors.centerIn:   parent
                width:              parent.width * (_isShootingInCurrentMode ? 0.5 : 0.75)  //   _isShootingInCurrentMode
                height:             width
                //changing the radius here to half the width makes it a circle
                radius:             _isShootingInCurrentMode ? 0 : width * 0.5  //   _isShootingInCurrentMode
                color:              _canShootInCurrentMode ? _recordButtonBusy ? qgcPal.colorOrange : qgcPal.colorRed : qgcPal.colorGrey
                //color:              _canShootInCurrentMode ? qgcPal.colorRed : qgcPal.colorGrey


            }

            MouseArea {
                anchors.fill:   parent
                enabled:        _canShootInCurrentMode// && !_recordButtonBusy
                onClicked:
                {
                    //change the color to yellow
                    anim.start()
                    toggleShooting()
                }
            }
        }

        //-- Status Information
        ColumnLayout {
            Layout.alignment:   Qt.AlignHCenter
            spacing:            0
            visible:            _activeVehicle
            width: parent.width
            //visible:            _nextVisionGimbalAvailable
            GridLayout {
                    id:     nvControlgridLayout
                    columns:            2
                    columnSpacing:      ScreenTools.defaultFontPixelWidth *0.25
                    rowSpacing:         ScreenTools.defaultFontPixelHeight*0.25
                    Layout.alignment: Qt.AlignHCenter
                    Layout.margins: _margins
                    visible:            _activeVehicle
                    //visible:            _nextVisionGimbalAvailable

                    QGCButton {
                        id:             grrButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        highlight:      _currentNvMode === "GRR"
                        text:           qsTr("GRR")
                        //leftPadding:    ScreenTools.defaultFontPixelWidth * 1
                        //rightPadding:   ScreenTools.defaultFontPixelWidth * 1
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animGrrButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: grrButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: grrButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animGrrButton.start()
                            joystickManager.cameraManagement.setSysModeGrrCommand()
                        }
                    }
                    QGCButton {
                        id:             pilotButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        highlight:      ((_currentNvMode === "Stow") || (_currentNvMode === "Local Pos")) ? true : false
                        text:           qsTr("PILOT")
                        //leftPadding:    ScreenTools.defaultFontPixelWidth * .5
                        //rightPadding:   ScreenTools.defaultFontPixelWidth * .5
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animPilotButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: pilotButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: pilotButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animPilotButton.start()
                            joystickManager.cameraManagement.setSysModePilotCommand()
                        }
                    }
                    QGCButton {
                        id:             obsButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        text:           qsTr("OBS")
                        highlight:      _currentNvMode === "Observation"
                        //leftPadding:    ScreenTools.defaultFontPixelWidth * 1
                        //rightPadding:   ScreenTools.defaultFontPixelWidth * 1
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animObsButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: obsButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: obsButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animObsButton.start()
                            joystickManager.cameraManagement.setSysModeObsCommand()
                        }
                    }
                    QGCButton {
                        id:             holdButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        text:           qsTr("HOLD")
                        highlight:      _currentNvMode === "Hold"
                        //leftPadding:    ScreenTools.defaultFontPixelWidth * .5
                        //rightPadding:   ScreenTools.defaultFontPixelWidth * .5
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animHoldButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: holdButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: holdButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animHoldButton.start()
                            joystickManager.cameraManagement.setSysModeHoldCommand()
                        }
                    }
                    QGCButton {
                        id:             dayButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        text:           qsTr("DAY")
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable && _nvIRMode
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animDayButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: dayButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: dayButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animDayButton.start()
                            joystickManager.cameraManagement.setSysSensorDayCommand()
                        }
                    }
                    QGCButton {
                        id:             irButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        text:           qsTr("IR")
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable && _nvDayMode
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animIrButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: irButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: irButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animIrButton.start()
                            joystickManager.cameraManagement.setSysSensorIrCommand()
                        }
                    }
                    QGCButton {
                        id:             nucButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        text:           qsTr("NUC")
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable && _nvIRMode
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animNucButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: nucButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: nucButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animNucButton.start()
                            joystickManager.cameraManagement.setSysIrNUCCommand()
                        }
                    }
                    QGCButton {
                        id:             resetCamButton
                        backRadius:     4
                        showBorder:     true
                        font.pointSize: ScreenTools.smallFontPointSize
                        pointSize:      ScreenTools.defaultFontPointSize
                        Layout.fillWidth: true
                        visible:        !_videoStreamInPhotoMode && _nextVisionGimbalAvailable && _nvDayMode
                        text:           qsTr("RST")
                        hoverEnabled:   false
                        SequentialAnimation {
                                    id: animResetButton
                                    // Expand the button
                                    PropertyAnimation {
                                        target: resetCamButton
                                        property: "scale"
                                        to: 1.2
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }

                                    // Shrink back to normal
                                    PropertyAnimation {
                                        target: resetCamButton
                                        property: "scale"
                                        to: 1.0
                                        duration: 200
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                        onClicked: {
                            animResetButton.start()
                            joystickManager.cameraManagement.setSysResetCommand()
                        }
                    }
            }
        }
    }

    Component {
        id: settingsDialogComponent

        QGCPopupDialog {
            id:         videoSettingsPopup
            title:      qsTr("Video Settings")
            buttons:    StandardButton.Close

            ColumnLayout {
                spacing: _margins

                GridLayout {
                    id:         gridLayout
                    columns:    2

                    QGCLabel {
                        text:               qsTr("Object Detection")
                        visible:            _nextVisionGimbalAvailable
                    }

                    FactComboBox {
                        id:                     objectDetection
                        Layout.fillWidth:       true
                        sizeToContents:         true
                        fact:                   _videoStreamSettings.objDetection
                        visible:                fact.visible && _nextVisionGimbalAvailable
                        indexModel:             false
                        onCurrentIndexChanged:  {
                            if (_videoStreamSettings.objDetection.value !== _videoStreamSettings.objDetection.enumValues[currentIndex])
                            {
                                if (_videoStreamSettings.objDetection.enumValues[currentIndex] === 4)
                                {
                                    joystickManager.cameraManagement.setSysObjDetOffCommand();
                                }
                                else
                                {
                                    console.log("setting obj detection to" + _videoStreamSettings.objDetection.enumValues[currentIndex]);
                                    joystickManager.cameraManagement.setSysObjDetSetNetTypeCommand(_videoStreamSettings.objDetection.enumValues[currentIndex])
                                    joystickManager.cameraManagement.setSysObjDetOnCommandAfterDelay(1000);  //turn on object detection after delay
                                }
                            }
                        }
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Map FOV Overlay")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.fovOverlay.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.fovOverlay.rawValue = checked ? 1 : 0
                    }

                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Map Target Overlay")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.targetOverlay.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.targetOverlay.rawValue = checked ? 1 : 0
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Remote Recording")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.remoteRecording.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.remoteRecording.rawValue = checked ? 1 : 0
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Pilot view on FBW")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.pilotViewOnFBW.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.pilotViewOnFBW.rawValue = checked ? true : false
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Downward view on Land")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.nadirViewOnLand.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.nadirViewOnLand.rawValue = checked ? true : false
                    }

                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Auto Record on Takeoff")
                        visible:            _nextVisionGimbalAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.recordOnFlying.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.recordOnFlying.rawValue = checked ? true : false
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Auto NUC Periodically")
                        visible:            _nextVisionGimbalAvailable
                    }
                    QGCSwitch {
                        id:                 _autoNucSwitch
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.autoNuc.rawValue
                        visible:            _nextVisionGimbalAvailable
                        onClicked:          _videoStreamSettings.autoNuc.rawValue = checked ? true : false
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Auto NUC Period")
                        visible:            _autoNucSwitch.checked
                    }
                    FactTextField {
                        fact:                   _videoStreamSettings.nucPeriod
                        visible:            _autoNucSwitch.checked
                    }


                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Video Mode")
                        visible:            _nextVisionGimbalAvailable
                    }

                    FactComboBox {
                        id:                     videoPlaybackMode
                        Layout.topMargin:       ScreenTools.defaultFontPixelHeight
                        Layout.fillWidth:       true
                        sizeToContents:         true
                        fact:                   _videoStreamSettings.videoPlaybackMode
                        visible:                fact.visible && _nextVisionGimbalAvailable
                        indexModel:             false
                        onCurrentIndexChanged:  {
                            if (_videoStreamSettings.videoPlaybackMode.value !== _videoStreamSettings.videoPlaybackMode.enumValues[currentIndex])
                            {
                                joystickManager.cameraManagement.setSysStreamModeCommand(_videoStreamSettings.videoPlaybackMode.enumValues[currentIndex],0);
                            }
                        }
                    }


                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("IR Color Mode")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "B&W"
                            onClicked: {
                                //joystickManager.cameraManagement.setSysIrPolarityToggleCommand()
                                joystickManager.cameraManagement.setSysIrBWPCommand()
                            }
                        }
                        QGCButton {
                            text: "Color"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrColorPCommand()
                            }
                        }
                    }

                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("IR Display Mode")
                        visible:            _nextVisionGimbalAvailable
                    }

                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "White Hot"
                            onClicked:
                            {
                                //joystickManager.cameraManagement.setSysIrBWPCommand()
                                joystickManager.cameraManagement.setSysIrPolarityWHCommand()

                            }
                        }
                        QGCButton {
                            text: "Black Hot"
                            onClicked: {
                                //joystickManager.cameraManagement.setSysIrBWPCommand()
                                joystickManager.cameraManagement.setSysIrPolarityBHCommand()
                            }
                        }
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("IR Level")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "Decrease"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrLevelDecrease()
                            }
                        }
                        QGCButton {
                            text: "Increase"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrLevelIncrease()
                            }
                        }
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("IR Gain")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "Decrease"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrGainDecrease()
                            }
                        }
                        QGCButton {
                            text: "Increase"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrGainIncrease()
                            }
                        }
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("IR Gain/Level")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "Reset"
                            onClicked: {
                                joystickManager.cameraManagement.setSysIrGainLevelReset()
                            }
                        }
                    }

                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Advanced Modes")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "Nadir"
                            onClicked: joystickManager.cameraManagement.setSysNadirCommand()
                        }
                        QGCButton {
                            text: "Stow"
                            onClicked: joystickManager.cameraManagement.setSysModeStowCommand()
                        }
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Video Grid Lines")
                        visible:            _anyVideoStreamAvailable
                    }

                    QGCSwitch {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        checked:            _videoStreamSettings.gridLines.rawValue
                        visible:            _anyVideoStreamAvailable
                        onClicked:          _videoStreamSettings.gridLines.rawValue = checked ? 1 : 0
                    }


                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Video Screen Fit")
                        visible:            _anyVideoStreamAvailable
                    }

                    FactComboBox {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        Layout.fillWidth:   true
                        sizeToContents:     true
                        fact:               _videoStreamSettings.videoFit
                        indexModel:         false
                        visible:            _anyVideoStreamAvailable
                    }
                    QGCLabel {
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        text:               qsTr("Video OSD")
                        visible:            _nextVisionGimbalAvailable
                    }
                    RowLayout{
                        Layout.topMargin:   ScreenTools.defaultFontPixelHeight
                        QGCButton {
                            text: "OSD On"
                            onClicked: {
                                 mainWindow.showPopupDialogFromComponent(changeOSDOnDialog)
                            }
                        }
                        QGCButton {
                            text: "OSD Off"
                            onClicked: {
                                 mainWindow.showPopupDialogFromComponent(changeOSDOffDialog)
                            }
                        }
                    }
                    Component {
                        id: changeOSDOnDialog
                        QGCPopupDialog {
                           title:      qsTr("Enable OSD?")
                            buttons:    StandardButton.Yes | StandardButton.Cancel

                            QGCLabel { text: qsTr("Warning: Changing OSD settings requires a full reboot of the Camera system. It will be offline for approximately 30 seconds. Are you sure you want to do this?")}

                            function accept() {
                                joystickManager.cameraManagement.setSysOSDOnCommand()
                                hideDialog()
                                videoSettingsPopup.hideDialog()
                            }
                        }

                    }
                    Component {
                        id: changeOSDOffDialog
                        QGCPopupDialog {
                           title:      qsTr("Disable OSD?")
                            buttons:    StandardButton.Yes | StandardButton.Cancel

                            QGCLabel { text: qsTr("Warning: Changing OSD settings requires a full reboot of the Camera system. It will be offline for approximately 30 seconds. Are you sure you want to do this?")}

                            function accept() {
                                joystickManager.cameraManagement.setSysOSDOffCommand()
                                hideDialog()
                                videoSettingsPopup.hideDialog()
                            }
                        }

                    }

                }
            }
        }
    }
}
