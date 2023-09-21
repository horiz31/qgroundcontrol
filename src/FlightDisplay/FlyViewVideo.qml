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

    property Item pipState: videoPipState
    QGCPipState {
        id:         videoPipState
        pipOverlay: _pipOverlay
        isDark:     true

        onWindowAboutToOpen: {
            QGroundControl.videoManager.stopVideo()
            videoStartDelay.start()
        }

        onWindowAboutToClose: {
            QGroundControl.videoManager.stopVideo()
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
        onTriggered:  QGroundControl.videoManager.startVideo()
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

    MouseArea {
        id: flyViewVideoMouseArea
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill:       parent
        enabled:            pipState.state === pipState.fullState
        property var isPressed: false
        onDoubleClicked: {
            QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
        }
        /*
        onPositionChanged: {
            var xScaledFromCenter
            var yScaledFromCenter
            if (isPressed)
            {
                var result = scaleValueCentered(mouse.x, mouse.y);
                joystickManager.cameraManagement.sendGimbalCommand(result.x,result.y);
                console.log("Sending Gimbal Command Roll/Pitch (1)", result.x, result.y);
                //update roll/pan based on how far we are from center
            }

        }
        onPressed:
        {
            if (mouse.button != Qt.LeftButton)
                return
            isPressed = true
            mouse.accepted = true

            var result = scaleValueCentered(mouse.x, mouse.y);
            joystickManager.cameraManagement.sendGimbalCommand(result.x,result.y);
            console.log("Sending Gimbal Command Roll/Pitch (2)", result.x, result.y);
            //start roll/pan based on how far we are from center
        }
        onReleased:
        {
            if (mouse.button != Qt.LeftButton)
                return
             isPressed = false
             mouse.accepted = true
            console.log("Sending Gimbal Command Roll/Pitch (3) 0.0,0.0");
            joystickManager.cameraManagement.sendGimbalCommand(0.0,0.0);
            //stop roll/pan
        }
        */

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

            /*
              //below is TBD, when I get left click to change pointing location
              /*
            if (mouse.button == Qt.RightButton)
            {
                var result = scaleValue(mouseX, mouseY);             
                console.log("Sending Tracking Command");
                joystickManager.cameraManagement.trackOnPosition(result.xScaled,result.yScaled,QGroundControl.settingsManager.appSettings.nvVideoChannel.rawValue);
            }

            else
            {
                //left click, so we want to position the camera based on the click distance from center screen
                //var result2 = scaleValueCentered(mouseX, mouseY);
                //console.log("Sending Gimbal Command Roll/Pitch", result2.x, result2.y);
                //joystickManager.cameraManagement.sendGimbalCommand(result2.x,result2.y);
            }
            */
        }
        onWheel: {

            //Send nextvision zoom in/out
             console.log("timer: starting camera zoom");
            if (wheel.angleDelta.y / 120 > 0 && !zoomStopTimer.running)
               joystickManager.cameraManagement.setSysZoomInCommand();
            else if (!zoomStopTimer.running)
               joystickManager.cameraManagement.setSysZoomOutCommand();

            //start a timer that stops zoom after 0.5
            zoomStopTimer.start();

           }
        Timer {
            id:             zoomStopTimer
            interval:       500
            repeat:         true

            onTriggered: {
                joystickManager.cameraManagement.setSysZoomStopCommand();
                console.log("timer: stopping camera zoom");
                this.stop();
            }

        }

        function scaleValue(x, y){
            var videoWidth
            var videoHeight
            var videoMargin

            var xPos = x
            videoHeight = height
            videoWidth = (videoHeight * 16.0 ) / 9.0
            videoMargin = (width - videoWidth) / 2.0
            if(x < (videoMargin + 16))
                xPos = videoMargin + 16
            else if(x > (videoMargin + videoWidth - 16) )
                xPos = (videoMargin + videoWidth - 16)
            xPos -= videoMargin
            var xScaled = (1280.0 * xPos) / videoWidth
            var yScaled = (720.0 * y) / videoHeight

            return {
                x: xScaled,
                y: yScaled
            }
        }
        function scaleValueCentered(x, y){
            var videoWidth
            var videoHeight
            var videoMargin

            var xPos = x
            videoHeight = height
            videoWidth = (videoHeight * 16.0 ) / 9.0
            videoMargin = (width - videoWidth) / 2.0
            if(x < (videoMargin + 16))
                xPos = videoMargin + 16
            else if(x > (videoMargin + videoWidth - 16) )
                xPos = (videoMargin + videoWidth - 16)
            xPos -= videoMargin
            var xScaled = (1280.0 * xPos) / videoWidth
            var yScaled = (720.0 * y) / videoHeight

            var xScaledFromCenter = (xScaled - 1280.0/2.0)/(1280/2.0)
            var yScaledFromCenter = (yScaled - 720.0/2.0)/(1280/2.0)

            return {
                x: xScaledFromCenter,
                y: yScaledFromCenter
            }
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

    ProximityRadarVideoView{
        anchors.fill:   parent
        vehicle:        QGroundControl.multiVehicleManager.activeVehicle
    }

    ObstacleDistanceOverlayVideo {
        id: obstacleDistance
        showText: pipState.state === pipState.fullState
    }
}
