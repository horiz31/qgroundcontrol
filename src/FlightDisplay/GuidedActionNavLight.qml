import QGroundControl.FlightDisplay 1.0
import QGroundControl 1.0

GuidedToolStripAction {

    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool _initialConnectComplete: activeVehicle ? activeVehicle.initialConnectComplete : false
    property bool _navlight_enable: _initialConnectComplete ? activeVehicle.hasNavLight : false

    text: "Nav Lights"
    iconSource: "/InstrumentValueIcons/light-bulb.svg" //TODO get a more appropriate icon
    visible: _navlight_enable
    enabled: _navlight_enable
    actionID: _guidedController.actionNavLight
}
