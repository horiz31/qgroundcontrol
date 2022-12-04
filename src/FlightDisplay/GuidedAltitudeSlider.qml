/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                  2.3
import QtQuick.Controls         1.2

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette               1.0

/// Altitude slider for guided change altitude command
Rectangle {
    id:                 _root

    readonly property real _maxAlt: 121.92  // 400 feet
    readonly property real _minAlt: 3

    property var mapRadiusIndicator

    property var  _activeVehicle:       QGroundControl.multiVehicleManager.activeVehicle
    property var  _flyViewSettings:     QGroundControl.settingsManager.flyViewSettings
    property real _vehicleAltitude:     _activeVehicle ? _activeVehicle.altitudeRelative.rawValue : 0
    property bool _fixedWing:           _activeVehicle ? _activeVehicle.fixedWing : false
    property real _sliderMaxAlt:        _flyViewSettings ? _flyViewSettings.guidedMaximumAltitude.rawValue : 0
    property real _sliderMinAlt:        _flyViewSettings ? _flyViewSettings.guidedMinimumAltitude.rawValue : 0
    property bool _flying:              _activeVehicle ? _activeVehicle.flying : false
    property bool _isClockwise:         true
    property bool _isDirectionVisible:  true
    property bool _isRadiusVisible:     true
    property bool _isRadiusInputVisible: true
    property bool _isRadiusFollowVehicle: false

    function reset() {
        altSlider.value = 0
        guidedRadiusField.text = _activeVehicle.guidedModeRadius.toString()
        guidedDir.currentIndex = 0

    }

    function setToMinimumTakeoff() {
        altField.setToMinimumTakeoff()
    }

    function setDirectionVisible(value)
    {
        if (value === true)
            _isDirectionVisible = true;
        else if (value === false)
            _isDirectionVisible = false
    }
    function setRadiusVisible(value)
    {
        if (value === true)
            _isRadiusVisible = true;
        else if (value === false)
            _isRadiusVisible = false
    }
    function setRadiusInputVisible(value)
    {
        if (value === true)
            _isRadiusInputVisible = true;
        else if (value === false)
            _isRadiusInputVisible = false
    }

    function setRadiusFollowVehicle(value)
    {
        if (value === true)
            _isRadiusFollowVehicle = true;
        else if (value === false)
            _isRadiusFollowVehicle = false
    }

    /// Returns the user specified change in altitude from the current vehicle altitude
    function getAltitudeChangeValue() {
        return altField.newAltitudeMeters - _vehicleAltitude
    }

    function isClockwise() {
        return _isClockwise;
        //return true if clockwise, false if counter
    }

    //this is grabed by the action controller prior to setting the command
    function guidedRadius()
    {
        return QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(Number(guidedRadiusField.text));
    }

    function log10(value) {
        if (value === 0) {
            return 0
        } else {
            return Math.log(value) / Math.LN10
        }
    }

    //add connection to the vehicle, on coordinate change, if __isRadiusFollowVehicle = true, update the center of the circle
    Connections {
        target: _activeVehicle
        onCoordinateChanged: {
            if(_isRadiusFollowVehicle && _root.mapRadiusIndicator){
              _root.mapRadiusIndicator.setCenter(_activeVehicle.coordinate)
            }
        }
    }

    Column {
        id:                 headerColumn
        anchors.margins:    _margins
        anchors.top:        parent.top
        anchors.left:       parent.left
        anchors.right:      parent.right

        QGCLabel {
            anchors.left:           parent.left
            anchors.right:          parent.right
            wrapMode:               Text.WordWrap
            horizontalAlignment:    Text.AlignHCenter
            text:                   qsTr("Direction:")
            visible:                _isDirectionVisible
        }
        QGCComboBox {
            id:             guidedDir
            model:          [ qsTr("CW"), qsTr("CCW")]
            anchors.left:           parent.left
            anchors.right:          parent.right
            visible:         _isDirectionVisible

            onActivated:
            {
                if (index === 0)
                {
                    _isClockwise = true;
                    _root.mapRadiusIndicator.setClockwise(true)
                }
                else
                {
                    _isClockwise = false;
                    _root.mapRadiusIndicator.setClockwise(false)
                }
            }
        }
        QGCLabel {
            anchors.left:           parent.left
            anchors.right:          parent.right
            wrapMode:               Text.WordWrap
            horizontalAlignment:    Text.AlignHCenter
            text:                   qsTr("Radius:")
            visible:                _isRadiusInputVisible
        }
        QGCTextField {
            id:             guidedRadiusField
            text:           _activeVehicle ? _activeVehicle.guidedModeRadius.toString() : ""
            visible:        _activeVehicle ? (_isRadiusInputVisible && _activeVehicle.supportsGuidedRadius) : false
            showUnits:      true
            unitsLabel:     QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
            anchors.left:   parent.left
            anchors.right:  parent.right
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator {bottom: 20; top: 10000;}
            onEditingFinished: {               
                if (_root.mapRadiusIndicator)
                    _root.mapRadiusIndicator.setRadius(Number(guidedRadiusField.text))
                altSlider.forceActiveFocus()
            }
            onAccepted: {                
            }
        }

        QGCLabel {
            anchors.left:           parent.left
            anchors.right:          parent.right
            wrapMode:               Text.WordWrap
            horizontalAlignment:    Text.AlignHCenter
            text:                   qsTr("New Alt:")
        }

        QGCLabel {
            id:                         altField
            anchors.horizontalCenter:   parent.horizontalCenter
            font.family:                ScreenTools.demiboldFontFamily
            font.pointSize:             ScreenTools.mediumFontPointSize
            text:                       newAltitudeAppUnits + " " + QGroundControl.unitsConversion.appSettingsVerticalDistanceUnitsString  //QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
            color:                      fontColor()
            property real   altGainRange:           Math.max(_sliderMaxAlt - _vehicleAltitude, 0)
            property real   altLossRange:           Math.max(_vehicleAltitude - _sliderMinAlt, 0)
            property real   altExp:                 Math.pow(altSlider.value, 3)
            property real   altLossGain:            altExp * (altSlider.value > 0 ? altGainRange : altLossRange)
            property real   newAltitudeMeters:      _vehicleAltitude + altLossGain
            property string newAltitudeAppUnits:    Math.round(QGroundControl.unitsConversion.metersToAppSettingsVerticalDistanceUnits(newAltitudeMeters).toFixed(0) / 10) * 10  //no need to do this in increments less than 10

            function setToMinimumTakeoff() {
                altSlider.value = Math.pow(_activeVehicle.minimumTakeoffAltitude() / altGainRange, 1.0/3.0)
            }

            function fontColor() {
                //console.log("altlossgain is" + altLossGain);
                if (altLossGain > 5) return qgcPal.colorGreen
                else if (altLossGain < -5) return qgcPal.colorOrange
                else return qgcPal.text
            }
        }
    }

    QGCSlider {
        id:                 altSlider
        anchors.margins:    _margins
        anchors.top:        headerColumn.bottom
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left
        anchors.right:      parent.right
        orientation:        Qt.Vertical
        minimumValue:       _flying ? -1 : 0
        maximumValue:       1
        zeroCentered:       true
        rotation:           180


        // We want slide up to be positive values
        transform: Rotation {
            origin.x:   altSlider.width  / 2
            origin.y:   altSlider.height / 2
            angle:      180
        }
    } 

}
