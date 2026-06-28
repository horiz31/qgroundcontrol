import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Vehicle       1.0

ToolStripAction {
    property var  activeVehicle:           QGroundControl.multiVehicleManager.activeVehicle
    property bool _initialConnectComplete: activeVehicle ? activeVehicle.initialConnectComplete : false
    property bool _navlight_enable:        _initialConnectComplete ? activeVehicle.hasNavLight : false
    property bool _navlight_on:            _initialConnectComplete ? activeVehicle.navLightOn : false

    property color iconColor: _navlight_on ? "green" : "red"

    text:       "Nav Lights"
    iconSource: "/InstrumentValueIcons/lightbulb_on.svg"
    visible:    _navlight_enable
    enabled:    _navlight_enable

    onTriggered: {
        if (activeVehicle) {
            activeVehicle.sendNavLightAction(_navlight_on ? Vehicle.NavLight_Off : Vehicle.NavLight_On)
        }
    }
}
