import QGroundControl.FlightDisplay 1.0
import QGroundControl 1.0

GuidedToolStripAction {

    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool _changeAirSpeed_enable: activeVehicle ? true : false

    text: "Set Airspeed"
    iconSource: "/InstrumentValueIcons/speedometer.svg"
    visible: _changeAirSpeed_enable
    enabled: _changeAirSpeed_enable
    actionID: _guidedController.actionChangeAirSpeed
}
