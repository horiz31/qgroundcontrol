/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick 2.12

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0
import QtGraphicalEffects 1.0


Item {
    id:         _root
    visible:    QGroundControl.videoManager.hasVideo
    focus:      true
    property var _lastZoomDirection: ""
    property Item pipState: videoPipState

    // --- Arrow-key gimbal pan properties -------------------------------------------
    // Magnitude of each pan nudge, in normalized stick units (-1.0 .. 1.0).
    // sendGimbalCommand() takes (roll_yaw, pitch) already normalized.
    property real _panStep: 0.3
    // Flip these if a key pans the wrong way on your gimbal.
    property int  _panYawSign:   1
    property int  _panPitchSign: 1

    // track which arrows are currently held (for diagonals + clean stop)
    property bool _keyLeft:  false
    property bool _keyRight: false
    property bool _keyUp:    false
    property bool _keyDown:  false
    property bool _keyCtrl:  false

    QGCPipState {
        id:         videoPipState
        pipOverlay: _pipOverlay
        isDark:     true

        onWindowAboutToOpen: {
            QGroundControl.videoManager.stopDecoding()
            videoStartDelay.start()
        }

        onWindowAboutToClose: {
            QGroundControl.videoManager.stopDecoding()
            videoStartDelay.start()
        }

        onStateChanged: {
            if (pipState.state !== pipState.fullState) {
                QGroundControl.videoManager.fullScreen = false
            }
        }
    }

    Timer {
        id:           videoStartDelay
        interval:     2000;
        running:      false
        repeat:       false
        onTriggered:  QGroundControl.videoManager.startDecoding()
    }


    function _sendPan() {
        if (!_keyLeft && !_keyRight && !_keyUp && !_keyDown) {
            // nothing held -> stop the gimbal
            joystickManager.cameraManagement.sendGimbalCommand(0, 0)
            return
        }
        var step  = _keyCtrl ? _panStep * 3.0 : _panStep
        var yaw   = 0.0
        var pitch = 0.0
        if (_keyLeft)  yaw   -= step
        if (_keyRight) yaw   += step
        if (_keyUp)    pitch += step
        if (_keyDown)  pitch -= step
        console.log("sending pan command, yaw: " + yaw * _panYawSign + " pitch: " + pitch * _panPitchSign)
        joystickManager.cameraManagement.sendGimbalCommand(yaw * _panYawSign,
                                                           pitch * _panPitchSign)
    }

    Keys.onPressed: {
        switch (event.key) {
            case Qt.Key_Left:    _keyLeft  = true; break
            case Qt.Key_Right:   _keyRight = true; break
            case Qt.Key_Up:      _keyUp    = true; break
            case Qt.Key_Down:    _keyDown  = true; break
            case Qt.Key_Control: _keyCtrl  = true; break
            default: return            // let other keys propagate
        }
        event.accepted = true
        _sendPan()                     // also re-fires on auto-repeat -> keeps panning
    }

    Keys.onReleased: {
        if (event.isAutoRepeat)        // ignore the fake release from key auto-repeat
            return
        switch (event.key) {
            case Qt.Key_Left:    _keyLeft  = false; break
            case Qt.Key_Right:   _keyRight = false; break
            case Qt.Key_Up:      _keyUp    = false; break
            case Qt.Key_Down:    _keyDown  = false; break
            case Qt.Key_Control: _keyCtrl  = false; break
            default: return
        }
        event.accepted = true
        _sendPan()                     // recompute with updated speed/direction
    }

    //-- Video Streaming
    FlightDisplayViewVideo {
        id:             videoStreaming
        anchors.fill:   parent
        useSmallFont:   _root.pipState.state !== _root.pipState.fullState
        visible:        QGroundControl.videoManager.isGStreamer
    }
    //-- UVC Video (USB Camera or Video Device)
    Loader {
        id:             cameraLoader
        anchors.fill:   parent
        visible:        !QGroundControl.videoManager.isGStreamer
        source:         QGroundControl.videoManager.uvcEnabled ? "qrc:/qml/FlightDisplayViewUVC.qml" : "qrc:/qml/FlightDisplayViewDummy.qml"
    }

    QGCLabel {
        text: qsTr("Double-click to exit full screen")
        font.pointSize: ScreenTools.largeFontPointSize
        visible: false // QGroundControl.videoManager.fullScreen && flyViewVideoMouseArea.containsMouse
        anchors.centerIn: parent
    }

    function stopZoom()
    {
        //stop nextvision zoom
        joystickManager.cameraManagement.setSysZoomStopCommand()
    }

    Timer {
        id:           zoomTimer
        interval:     500;
        running:      false
        repeat:       false
        onTriggered:  stopZoom()
    }


    MouseArea {
        id: flyViewVideoMouseArea
        anchors.fill:       parent
        enabled:            pipState.state === pipState.fullState

        //remove double click because it doesn't work well with track mode
        /*
        onDoubleClicked: {
            console.log("double clicked");
            QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
        }*/

        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                if (zoomTimer.running && _lastZoomDirection === "out" )
                {
                    zoomTimer.stop()
                    joystickManager.cameraManagement.setSysZoomStopCommand()
                }
                else
                {
                    joystickManager.cameraManagement.setSysZoomInCommand();
                    _lastZoomDirection = "in"
                    if (zoomTimer.running)
                    {
                        zoomTimer.interval += 250;
                        zoomTimer.restart()
                    }
                    else
                    {
                        zoomTimer.interval = 500;
                        zoomTimer.restart()
                    }
                }
            }
            else
            {
                //zoom out
                if (zoomTimer.running && _lastZoomDirection === "in" )
                {
                    zoomTimer.stop()
                    joystickManager.cameraManagement.setSysZoomStopCommand()
                }
                else
                {
                    joystickManager.cameraManagement.setSysZoomOutCommand();
                    _lastZoomDirection = "out"
                    if (zoomTimer.running)
                    {
                        zoomTimer.interval += 250;
                        zoomTimer.restart()
                    }
                    else
                    {
                        zoomTimer.interval = 500;
                        zoomTimer.restart()
                    }
                }
            }
        }

        onClicked: {
            /* Calculating the position to track on */
            var videoWidth
            var videoHeight
            var videoMargin
            var xPos = mouseX
            videoHeight = height
            videoWidth = (videoHeight * 16.0 ) / 9.0
            videoMargin = (width - videoWidth) / 2.0
            if(mouseX < (videoMargin + 16))
                xPos = videoMargin + 16
            else if(mouseX > (videoMargin + videoWidth - 16) )
                xPos = (videoMargin + videoWidth - 16)
            xPos -= videoMargin
            var xScaled = (1280.0 * xPos) / videoWidth
            var yScaled = (720.0 * mouseY) / videoHeight
            /* Sending the Track On Position command to the TRIP */
            joystickManager.cameraManagement.trackOnPosition(xScaled,yScaled,QGroundControl.settingsManager.appSettings.nvVideoChannel.rawValue);
        }
    }

    Rectangle {
        id:                     _exitFullScreenVideo
        anchors.right :         parent.right
        anchors.top:            parent.top
        height:                 ScreenTools.isMobile ? ScreenTools.defaultFontPixelHeight*3.0 : ScreenTools.defaultFontPixelHeight*2.0
        width:                  height * 1.2
        color:                  Qt.rgba(0,0,0,0)
        anchors.margins:        ScreenTools.defaultFontPixelWidth * 0.75
        visible:                QGroundControl.videoManager.fullScreen

        Image {
            id:                 _imageExit
            width:              parent.width
            height:             parent.height
            sourceSize.height:  height
            source:             "/InstrumentValueIcons/close-outline.svg"
            fillMode:           Image.PreserveAspectFit
            anchors.verticalCenter:     parent.verticalCenter
            anchors.horizontalCenter:   parent.horizontalCenter
        }
        MouseArea {
            anchors.fill:   parent
            onClicked:      {
                QGroundControl.videoManager.fullScreen = false
            }
        }
        ColorOverlay {
            anchors.fill:       _imageExit
            source:             _imageExit
            color:              "white"
        }
    }

    /*
    Rectangle {
        id:                     _enterFullScreenVideo
        anchors.right :         parent.right
        anchors.bottom:         parent.bottom
        height:                 ScreenTools.isMobile ? ScreenTools.defaultFontPixelHeight*3.0 : ScreenTools.defaultFontPixelHeight*2.0
        width:                  height * 1.2
        color:                  Qt.rgba(0,0,0,0)
        anchors.margins:        ScreenTools.defaultFontPixelWidth * 0.75
        visible:                !QGroundControl.videoManager.fullScreen

        Image {
            id:                 _imageEnter
            width:              parent.width
            height:             parent.height
            sourceSize.height:  height
            source:             "/InstrumentValueIcons/fullscreen-outline.svg"
            fillMode:           Image.PreserveAspectFit
            anchors.verticalCenter:     parent.verticalCenter
            anchors.horizontalCenter:   parent.horizontalCenter
        }
        MouseArea {
            anchors.fill:   parent
            onClicked:      {
                QGroundControl.videoManager.fullScreen = true
            }
        }
        ColorOverlay {
            anchors.fill:       _imageEnter
            source:             _imageEnter
            color:              "white"
        }
    }
    */

    ProximityRadarVideoView{
        anchors.fill:   parent
        vehicle:        QGroundControl.multiVehicleManager.activeVehicle
    }

    ObstacleDistanceOverlayVideo {
        id: obstacleDistance
        showText: pipState.state === pipState.fullState
    }
}
