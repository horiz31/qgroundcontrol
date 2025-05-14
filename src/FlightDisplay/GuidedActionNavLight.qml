import QGroundControl.FlightDisplay 1.0
import QGroundControl 1.0

GuidedToolStripAction {

    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool _initialConnectComplete: activeVehicle ? activeVehicle.initialConnectComplete : false
    property bool _navlight_enable: _initialConnectComplete ? activeVehicle.hasNavLight : false
    property bool _navlight_on: _initialConnectComplete ? activeVehicle.navLightOn : false

    text: "Nav Lights"
    iconSource: _navlight_on ? "/InstrumentValueIcons/lightbulb_on.svg": "/InstrumentValueIcons/lightbulb_off.svg"
    visible: _navlight_enable
    enabled: _navlight_enable
    actionID: _guidedController.actionNavLight
}
