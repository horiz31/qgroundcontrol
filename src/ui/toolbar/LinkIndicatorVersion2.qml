/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.11
import QtQuick.Layouts  1.11

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

//-------------------------------------------------------------------------
//-- LinkIndicatorVersion2 Indicator
Item {
    id:             _root
    width:          linkRow.width * 1.1
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property bool showIndicator: true
    property var  _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property var _rgLinkNames:      _activeVehicle ? _activeVehicle.vehicleLinkManager.linkNames : [ ]
    property var _rgLinkStatus:     _activeVehicle ? _activeVehicle.vehicleLinkManager.linkStatuses : [ ]
    property var _rgLinkPriorityLinkName:     _activeVehicle ? _activeVehicle.vehicleLinkManager.primaryLinkName : ""

    property var _rgMenuItems:      [ ]

    Component {
        id: linkInfo

        Rectangle {
            width:  linkCol.width   + ScreenTools.defaultFontPixelWidth  * 3
            height: linkCol.height  + ScreenTools.defaultFontPixelHeight * 2
            radius: ScreenTools.defaultFontPixelHeight * 0.5
            color:  qgcPal.window
            border.color:   qgcPal.text

            Column {
                id:                 linkCol
                spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                width:              Math.max(linkGrid.width, linkLabel.width)
                anchors.margins:    ScreenTools.defaultFontPixelHeight
                anchors.centerIn:   parent

                QGCLabel {
                    id:             linkLabel
                    text:           _activeVehicle ? qsTr("Links Available") : qsTr("N/A", "No data available")
                    font.family:    ScreenTools.demiboldFontFamily
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GridLayout {
                    id:                 linkGrid
                    visible:            true
                    columns:            2
                    anchors.margins:    ScreenTools.defaultFontPixelHeight
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    anchors.horizontalCenter: parent.horizontalCenter

                    Repeater {
                        model:      _rgLinkNames
                        delegate:
                            RowLayout {
                                        Layout.fillWidth: true
                                        Layout.row: index

                                        QGCLabel {
                                            Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 20
                                            text: getModelText()
                                            color: getPrimaryLinkStatus()
                                            function getPrimaryLinkStatus()
                                            {
                                              if (_rgLinkStatus[index] === "Comm Lost")
                                                   return qgcPal.colorRed
                                                return qgcPal.buttonText
                                            }
                                            function getModelText()
                                            {
                                                if (_rgLinkStatus[index] === "Comm Lost")
                                                    return modelData + " (Comms Lost)"
                                                else
                                                    return modelData
                                            }
                                        }

                                        QGCButton {
                                            text: "Set Active"
                                            enabled: _rgLinkPriorityLinkName !== modelData
                                            onClicked: {
                                                _activeVehicle.vehicleLinkManager.primaryLinkName = modelData
                                            }
                                        }
                        }
                    }



                }
            }
        }
    }

    Row {
        id:             linkRow
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        spacing:        ScreenTools.defaultFontPixelWidth
        visible:        _activeVehicle ? true : false


        QGCColoredImage {
            width:              height
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            sourceSize.height:  height
            source:             (_rgLinkNames.length > 1) ? "/res/link_multiple.svg" : "/res/link.svg"
            fillMode:           Image.PreserveAspectFit
            opacity:            1
            color:              qgcPal.buttonText

        }

         QGCLabel {
             text: getText()
             font.pointSize:     ScreenTools.mediumFontPointSize
             anchors.verticalCenter: parent.verticalCenter
             color:              getPrimaryLinkStatus();
             function getText()
             {
                 if (_activeVehicle)
                 {
                     var i
                     for (i = 0; i < _rgLinkStatus.length; i++) {
                        if ((_rgLinkNames[i] === _activeVehicle.vehicleLinkManager.primaryLinkName) && (_rgLinkStatus[i] === "Comm Lost"))
                        return _activeVehicle.vehicleLinkManager.primaryLinkName + " (Comms Lost)"
                     }
                     return _activeVehicle.vehicleLinkManager.primaryLinkName
                 }
                 return "";
             }
             function getPrimaryLinkStatus()
             {
                 //if the selected (primary) link is comms lost, make it red
                 var i
                 for (i = 0; i < _rgLinkStatus.length; i++) {
                    if ((_rgLinkNames[i] === _activeVehicle.vehicleLinkManager.primaryLinkName) && (_rgLinkStatus[i] === "Comm Lost"))
                    return qgcPal.colorRed
                 }
                 return qgcPal.buttonText

             }

         }


    }

    MouseArea {
        anchors.fill:   parent
        enabled: _activeVehicle ? (_rgLinkNames.length > 1 ? true : false) : false
        onClicked: {
            mainWindow.showIndicatorPopup(_root, linkInfo)
        }
    }
}

