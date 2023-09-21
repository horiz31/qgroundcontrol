/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs  1.2
import QtQuick.Layouts  1.11

import QGroundControl               1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controllers   1.0
import QGroundControl.Palette       1.0

SetupPage {
    id:             doodleRadioPage
    pageComponent:  pageComponent

    Component {
        id: pageComponent

        Item {
            width:  availableWidth
            //height: Math.max(leftColumn.height, rightColumn.height)

            readonly property string  dialogTitle: qsTr("Channel Warning")

            function setupPageCompleted() {
                controller.start()

            }

            Component.onDestruction: {
               controller.stop();
            }

            Component {
                id: warningChannelMessage
                QGCViewMessage {
                    message: qsTr("Warning: This action will change the channel on the local radio and all other radios currently connected. This is an advanced action and may cause you to lose communication to a vehicle if not done properly. Please do not proceed unless you understand the implications.")
                    function accept() {
                        hideDialog()
                        controller.updateChannelClicked()
                    }
                    function reject() {
                        hideDialog()
                        controller.refresh()
                    }
                }
            }


            QGCPalette { id: qgcPal; colorGroupEnabled: doodleRadioPage.enabled }

            DoodleComponentController {
                id:             controller
                statusText:     statusText 
                clientListText: clientListText
            }

            property bool    _isDoodleConnected:             controller.isConnected
            property bool    _isDoodleChannelLoadComplete:   controller.isChannelLoadComplete
            property bool    _isDoodleConnectionError:       controller.isConnectionError
            property bool    _isDoodleChannelChangeOK:       controller.isChannelChangeOK
            property bool    _isIPEditable:                 controller.isIPEditable

            GridLayout {
                id:                         doodleGrid
                columns:  3

                Image {
                    source: "../qmlimages/Doodle.png"
                    Layout.columnSpan: 3
                    MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                controller.clickIP()
                            }
                        }
                }
                ProgressBar {
                    indeterminate: true
                    visible: (!_isDoodleConnected && !_isDoodleConnectionError) || (_isDoodleConnected && _isDoodleConnectionError)
                    width: doodleGrid.width
                    Layout.columnSpan: 3
                }
                QGCLabel {
                    text:                   qsTr("IP Address of Local Doodle Radio:")

                }
                FactTextField {
                    fact:                   QGroundControl.settingsManager.appSettings.doodleIP
                    enabled:                _isIPEditable

                }
                QGCButton {
                    text:          qsTr("Refresh")
                    onClicked:     controller.refresh()
                }
                QGCLabel {
                    text:       qsTr("Status:")
                    width:      parent.width
                    wrapMode:   Text.WordWrap
                }

                QGCLabel {
                    id:         statusText
                    width:      parent.width
                    wrapMode:   Text.WordWrap
                    Layout.columnSpan:      2
                }
                QGCLabel {
                    text:       qsTr("Mesh Members:")
                    Layout.alignment: Qt.AlignTop
                    visible:    true
                    width:      parent.width
                }

                QGCLabel {
                    id:         clientListText
                    Layout.alignment: Qt.AlignTop
                    visible:    true
                    Layout.columnSpan:      2
                }

                //Item { width: 1; height: _margins; Layout.columnSpan:      3}

                QGCLabel {
                    id:         settingsLabel
                    text:       qsTr("Settings")
                    visible:    true
                    Layout.columnSpan:      3
                }

                Rectangle {
                    //Layout.preferredWidth:  comboGrid.width + (_margins * 4)
                    Layout.minimumWidth: comboGrid.width + (_margins * 4)
                    Layout.preferredHeight: (comboGrid.height)  + (_margins * 2)
                    Layout.fillWidth:       true
                    color:                  qgcPal.windowShade
                    visible:                true
                    Layout.columnSpan:      3


                    Item {
                        id:                 comboGridItem
                        anchors.margins:    _margins
                        anchors.top:        parent.top
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        height:             comboGrid.height

                        GridLayout {
                            id:                         comboGrid
                            columns:                    3

                            QGCLabel {
                                text:                   qsTr("Retrieve RSSI From Doodle Radio")
                            }
                            FactCheckBox {
                                text:                   ""
                                enabled:                _isDoodleConnected
                                fact:                   QGroundControl.settingsManager.appSettings.enableDoodleRssi
                                Layout.columnSpan:      2
                            }

                            QGCLabel {
                                text:                   qsTr("Radio Channel")
                            }
                            QGCComboBox {
                                id: doodleChannel
                                enabled: _isDoodleConnected && _isDoodleChannelLoadComplete && _isDoodleChannelChangeOK
                                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 16
                                model: ["Ch 1, 2.412 GHz", "Ch 2, 2.417 GHz", "Ch 3, 2.422 GHz", "Ch 4, 2.427 GHz", "Ch 5, 2.432 GHz", "Ch 6, 2.437 GHz", "Ch 7, 2.442 GHz","Ch 8, 2.447 GHz","Ch 9, 2.452 GHz","Ch 10, 2.457 GHz","Ch 11, 2.462 GHz","Ch 12, 2.467 GHz","Ch 13, 2.472 GHz","Ch 14, 2.484 GHz"]
                                currentIndex: controller.channel - 1
                                onCurrentIndexChanged: {
                                    controller.setChannel(currentIndex + 1)
                                }
                            }

                            QGCButton {
                                id: updateChannelButton
                                text:               qsTr("Change Channel")
                                enabled:    _isDoodleConnected && _isDoodleChannelLoadComplete && _isDoodleChannelChangeOK
                                onClicked:  mainWindow.showComponentDialog(warningChannelMessage, dialogTitle, mainWindow.showDialogDefaultWidth, StandardButton.Ok | StandardButton.Cancel)
                            }

                            Item { width: 1; height: _margins; Layout.columnSpan:      1}

                            QGCLabel {
                                id:                 channelwarningLabel
                                font.pointSize:     ScreenTools.smallFontPointSize
                                Layout.columnSpan:  2
                                text:               qsTr("Note: Changing channel requires that all nodes in the\nmesh have an RSSI > 50%. If the button above is\ndisabled, move the associated radios closer to the local\nradio and click Refresh.")
                            }

                        }
                    }
                }

            } // Grid

        } // Item
    } // Component - pageComponent
} // SetupPage
