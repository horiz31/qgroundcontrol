import QtQuick                  2.12
import QtQuick.Controls         1.2

import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0

/// The EnterSwitch control implements a "press enter to activate" variant of SliderSwitch for us on PCs


Rectangle {
    id:             _root
    implicitWidth:  label.contentWidth + (_diameter * 2.5) + (_border * 2)
    implicitHeight: label.height * 1.75
    radius:         height /3

    Connections {
        target:  altitudeSlider
        function onExecuteAction() {
            //trigger the enterslider to press enter            
            _root.accept()

        }
    }

    SequentialAnimation on color {
        loops: Animation.Infinite
        running: true

        ColorAnimation {
            from: "darkgreen"
            to: "green"
            duration: 1000
            easing.type: Easing.InOutQuad
        }

        ColorAnimation {
            from: "green"
            to: "darkgreen"
            duration: 1000
            easing.type: Easing.InOutQuad
        }
    }

    //this needs to also accept a signal from confirmation, so that if enter is pressed after changing the radius, it is accepted
    //or we could run a function in guidedactioncontroller directly from the altitude controller
    //really this just fires the signal _root.accept() so we coudl do the same thing?

    signal accept   ///< Action confirmed

    property var altSlider
    property string confirmText                         ///< Text for button
    property alias  fontPointSize: label.font.pointSize ///< Point size for text

    property real _border:                      3
    property real _diameter:                    height - (_border * 2)
    property real _dragStartX:                  _border
    property real _dragStopX:                   _root.width - (_diameter + _border)


    Keys.onPressed: (event) => {

        const isEnterKey =
            event.key === Qt.Key_Return ||
            event.key === Qt.Key_Enter ||
            (typeof Qt.KeypadEnter !== "undefined" && event.key === Qt.KeypadEnter)

        // allow only if no Ctrl/Alt/Shift/Meta are held (use bitmask)
        const noCtrlAltShiftMeta =
            (event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.ShiftModifier | Qt.MetaModifier)) === 0

        if (isEnterKey && !event.isAutoRepeat && noCtrlAltShiftMeta) {
            event.accepted = true
            _root.accept()
        }
    }


    QGCPalette { id: qgcPal; colorGroupEnabled: true }


    QGCLabel {
        id:                         label
        width:                      parent.width - x
        anchors.verticalCenter:     parent.verticalCenter
        horizontalAlignment:        Text.AlignHCenter
        text:                       confirmText
        color:                      qgcPal.buttonText
    }


    QGCMouseArea {
        id:                 enterArea
        anchors.leftMargin: -ScreenTools.defaultFontPixelWidth * 15
        fillItem:           label
        preventStealing:    true

        onClicked: {
            _root.accept()
        }


    }
}

