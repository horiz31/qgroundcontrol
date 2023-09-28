/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include "Joystick.h"
#include "QGC.h"
#include "AutoPilotPlugin.h"
#include "UAS.h"
#include "QGCApplication.h"
#include "VideoManager.h"
#include "QGCCameraManager.h"
#include "QGCCameraControl.h"
#include "SettingsManager.h"

#include <QSettings>

#define NV_HAT_INC 983.0f

QGC_LOGGING_CATEGORY(JoystickLog,       "JoystickLog")
QGC_LOGGING_CATEGORY(JoystickValuesLog, "JoystickValuesLog")

const char* Joystick::_settingsGroup =                  "Joysticks";
const char* Joystick::_calibratedSettingsKey =          "Calibrated4"; // Increment number to force recalibration
const char* Joystick::_buttonActionNameKey =            "ButtonActionName%1";
const char* Joystick::_buttonActionRepeatKey =          "ButtonActionRepeat%1";
const char* Joystick::_throttleModeSettingsKey =        "ThrottleMode";
const char* Joystick::_negativeThrustSettingsKey =      "NegativeThrust";
const char* Joystick::_exponentialSettingsKey =         "Exponential";
const char* Joystick::_accumulatorSettingsKey =         "Accumulator";
const char* Joystick::_deadbandSettingsKey =            "Deadband";
const char* Joystick::_circleCorrectionSettingsKey =    "Circle_Correction";
const char* Joystick::_axisFrequencySettingsKey =       "AxisFrequency";
const char* Joystick::_minThrottleFBWSettingsKey =      "MinFBWThrottle";
const char* Joystick::_buttonFrequencySettingsKey =     "ButtonFrequency";
const char* Joystick::_txModeSettingsKey =              nullptr;
const char* Joystick::_fixedWingTXModeSettingsKey =     "TXMode_FixedWing";
const char* Joystick::_multiRotorTXModeSettingsKey =    "TXMode_MultiRotor";
const char* Joystick::_roverTXModeSettingsKey =         "TXMode_Rover";
const char* Joystick::_vtolTXModeSettingsKey =          "TXMode_VTOL";
const char* Joystick::_submarineTXModeSettingsKey =     "TXMode_Submarine";

const char* Joystick::_buttonActionNone =               QT_TR_NOOP("No Action");
const char* Joystick::_buttonActionArm =                QT_TR_NOOP("Arm");
const char* Joystick::_buttonActionDisarm =             QT_TR_NOOP("Disarm");
const char* Joystick::_buttonActionToggleArm =          QT_TR_NOOP("Toggle Arm");
const char* Joystick::_buttonActionVTOLFixedWing =      QT_TR_NOOP("VTOL: Fixed Wing");
const char* Joystick::_buttonActionVTOLMultiRotor =     QT_TR_NOOP("VTOL: Multi-Rotor");
const char* Joystick::_buttonActionContinuousZoomIn =   QT_TR_NOOP("Continuous Zoom In");
const char* Joystick::_buttonActionContinuousZoomOut =  QT_TR_NOOP("Continuous Zoom Out");
const char* Joystick::_buttonActionStepZoomIn =         QT_TR_NOOP("Step Zoom In");
const char* Joystick::_buttonActionStepZoomOut =        QT_TR_NOOP("Step Zoom Out");
const char* Joystick::_buttonActionNextStream =         QT_TR_NOOP("Next Video Stream");
const char* Joystick::_buttonActionPreviousStream =     QT_TR_NOOP("Previous Video Stream");
const char* Joystick::_buttonActionNextCamera =         QT_TR_NOOP("Next Camera");
const char* Joystick::_buttonActionPreviousCamera =     QT_TR_NOOP("Previous Camera");
const char* Joystick::_buttonActionTriggerCamera =      QT_TR_NOOP("Trigger Camera");
const char* Joystick::_buttonActionStartVideoRecord =   QT_TR_NOOP("Start Recording Video");
const char* Joystick::_buttonActionStopVideoRecord =    QT_TR_NOOP("Stop Recording Video");
const char* Joystick::_buttonActionToggleVideoRecord =  QT_TR_NOOP("Toggle Recording Video");
const char* Joystick::_buttonActionGimbalDown =         QT_TR_NOOP("Gimbal Down");
const char* Joystick::_buttonActionGimbalUp =           QT_TR_NOOP("Gimbal Up");
const char* Joystick::_buttonActionGimbalLeft =         QT_TR_NOOP("Gimbal Left");
const char* Joystick::_buttonActionGimbalRight =        QT_TR_NOOP("Gimbal Right");
const char* Joystick::_buttonActionGimbalCenter =       QT_TR_NOOP("Gimbal Center");
const char* Joystick::_buttonActionEmergencyStop =      QT_TR_NOOP("Emergency Stop");
const char* Joystick::_buttonActionEngineRunup =        QT_TR_NOOP("Hold for Engine Runup");

const char* Joystick::_rgFunctionSettingsKey[Joystick::maxFunction] = {
    "RollAxis",
    "PitchAxis",
    "YawAxis",
    "ThrottleAxis",
    "GimbalPitchAxis",
    "GimbalYawAxis"
};

int Joystick::_transmitterMode = 2;

/* NextVision Added configuration keys for Camera Joystick
* ------------------------------------------------------------------------------------------------------*/
const char* Joystick::_buttonCamActionNameKey =         "ButtonCamActionName%1";
const char* Joystick::_buttonCamActionRepeatKey =       "ButtonCamActionRepeat%1";
const char* Joystick::_camPitchRollAxleKey =            "CameraPitchRollAxle";

const char* Joystick::_buttonActionZoomIn =             QT_TR_NOOP("Zoom In");
const char* Joystick::_buttonActionZoomOut =            QT_TR_NOOP("Zoom Out");
const char* Joystick::_buttonActionDayIR =              QT_TR_NOOP("Day / IR");
const char* Joystick::_buttonActionWHBH =               QT_TR_NOOP("White Hot / Black Hot");
const char* Joystick::_buttonActionCM =                 QT_TR_NOOP("Color / B&W");
const char* Joystick::_buttonActionNUC =                QT_TR_NOOP("NUC");
const char* Joystick::_buttonActionSnap =               QT_TR_NOOP("Image Capture");
const char* Joystick::_buttonActionRec =                QT_TR_NOOP("Record");
const char* Joystick::_buttonActionSY =                 QT_TR_NOOP("Single Yaw");
const char* Joystick::_buttonActionOBS =                QT_TR_NOOP("Observation");
const char* Joystick::_buttonActionGRR =                QT_TR_NOOP("GRR");
const char* Joystick::_buttonActionStow =               QT_TR_NOOP("Stow");
const char* Joystick::_buttonActionPilot =              QT_TR_NOOP("Pilot");
const char* Joystick::_buttonActionRetract =            QT_TR_NOOP("Retract");
const char* Joystick::_buttonActionHoldCord =           QT_TR_NOOP("Hold Coordinate");
const char* Joystick::_buttonActionOverrideStick =      QT_TR_NOOP("Override Stick");
/* ------------------------------------------------------------------------------------------------------*/

const float Joystick::_defaultAxisFrequencyHz   = 25.0f;
const float Joystick::_defaultButtonFrequencyHz = 5.0f;
const float Joystick::_minAxisFrequencyHz       = 0.25f;
const float Joystick::_maxAxisFrequencyHz       = 200.0f;
const float Joystick::_minButtonFrequencyHz     = 0.25f;
const float Joystick::_maxButtonFrequencyHz     = 50.0f;




AssignedButtonAction::AssignedButtonAction(QObject* parent, const QString name)
    : QObject(parent)
    , action(name)
{
}

AssignableButtonAction::AssignableButtonAction(QObject* parent, QString action_, bool canRepeat_)
    : QObject(parent)
    , _action(action_)
    , _repeat(canRepeat_)
{
}

Joystick::Joystick(const QString& name, int axisCount, int buttonCount, int hatCount, MultiVehicleManager* multiVehicleManager)
    : _name(name)
    , _axisCount(axisCount)
    , _buttonCount(buttonCount)
    , _hatCount(hatCount)
    , _hatButtonCount(4 * hatCount)
    , _totalButtonCount(_buttonCount+_hatButtonCount)
    , _multiVehicleManager(multiVehicleManager)
{
    _rgAxisValues   = new int[static_cast<size_t>(_axisCount)];
    _rgCalibration  = new Calibration_t[static_cast<size_t>(_axisCount)];
    _rgButtonValues = new uint8_t[static_cast<size_t>(_totalButtonCount)];
    for (int i = 0; i < _axisCount; i++) {
        _rgAxisValues[i] = 0;
    }
    for (int i = 0; i < _totalButtonCount; i++) {
        _rgButtonValues[i] = BUTTON_UP;
        _buttonActionArray.append(nullptr);
    }

    /* NextVision Added Code For Camera Joystick
     ------------------------------------------------------------------------------------------------------*/
    for (int i = 0; i < _totalButtonCount; i++) {
        _buttonCamActionArray.append(nullptr);
    }
    _buildCamActionList();

    _joystickPitchRollEnableOption =  qgcApp()->toolbox()->settingsManager()->appSettings()->camJoystickPitchRollEnableOption()->rawValue().toInt();
    _camJoystickDZ = qgcApp()->toolbox()->settingsManager()->appSettings()->camJoystickDZ()->rawValue().toInt();
    _camJoystickGain = qgcApp()->toolbox()->settingsManager()->appSettings()->camJoystickGain()->rawValue().toInt();
    _camJoystickRollInvert = qgcApp()->toolbox()->settingsManager()->appSettings()->camJoystickRollInvert()->rawValue().toBool();
    _camJoystickPitchInvert = qgcApp()->toolbox()->settingsManager()->appSettings()->camJoystickPitchInvert()->rawValue().toBool();

    /* ------------------------------------------------------------------------------------------------------*/

    _buildActionList(_multiVehicleManager->activeVehicle());
    _updateTXModeSettingsKey(_multiVehicleManager->activeVehicle());
    _loadSettings();
    connect(_multiVehicleManager, &MultiVehicleManager::activeVehicleChanged, this, &Joystick::_activeVehicleChanged);

    _customMavCommands = JoystickMavCommand::load("JoystickMavCommands.json");
}

void Joystick::stop()
{
    _exitThread = true;
    wait();
}

Joystick::~Joystick()
{
    if (!_exitThread) {
        qWarning() << "Joystick thread still running!";
    }
    delete[] _rgAxisValues;
    delete[] _rgCalibration;
    delete[] _rgButtonValues;
    _assignableButtonActions.clearAndDeleteContents();
    _assignableCamButtonActions.clearAndDeleteContents();       /* NextVision */
    for (int button = 0; button < _totalButtonCount; button++) {
        if(_buttonActionArray[button]) {
            _buttonActionArray[button]->deleteLater();
        }

        /* NextVision Added Code For Camera Joystick
         ------------------------------------------------------------------------------------------------------*/
        if(_buttonCamActionArray[button]) {
            _buttonCamActionArray[button]->deleteLater();
        }
        /* ------------------------------------------------------------------------------------------------------*/
    }
}

void Joystick::_setDefaultCalibration(void) {
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.beginGroup(_name);
    _calibrated = settings.value(_calibratedSettingsKey, false).toBool();

    // Only set default calibrations if we do not have a calibration for this gamecontroller
    if(_calibrated) return;

    for(int axis = 0; axis < _axisCount; axis++) {
        Joystick::Calibration_t calibration;
        _rgCalibration[axis] = calibration;
    }

    _rgCalibration[1].reversed = true;
    _rgCalibration[3].reversed = true;

    // Default TX Mode 2 axis assignments for gamecontrollers
    _rgFunctionAxis[rollFunction]       = 2;
    _rgFunctionAxis[pitchFunction]      = 3;
    _rgFunctionAxis[yawFunction]        = 0;
    _rgFunctionAxis[throttleFunction]   = 1;

    _rgFunctionAxis[gimbalPitchFunction]= 4;
    _rgFunctionAxis[gimbalYawFunction]  = 5;

    _exponential        = 0;
    _accumulator        = false;
    _deadband           = false;
    _axisFrequencyHz    = _defaultAxisFrequencyHz;
    _buttonFrequencyHz  = _defaultButtonFrequencyHz;
    _throttleMode       = ThrottleModeDownZero;
    _calibrated         = true;
    _circleCorrection   = false;
    _runupEnabled       = false;
    _rollPitchEnabled   = true;

    _saveSettings();
}

void Joystick::_updateTXModeSettingsKey(Vehicle* activeVehicle)
{
    if(activeVehicle) {
        if(activeVehicle->fixedWing()) {
            _txModeSettingsKey = _fixedWingTXModeSettingsKey;
        } else if(activeVehicle->multiRotor()) {
            _txModeSettingsKey = _multiRotorTXModeSettingsKey;
        } else if(activeVehicle->rover()) {
            _txModeSettingsKey = _roverTXModeSettingsKey;
        } else if(activeVehicle->vtol()) {
            _txModeSettingsKey = _vtolTXModeSettingsKey;
        } else if(activeVehicle->sub()) {
            _txModeSettingsKey = _submarineTXModeSettingsKey;
        } else {
            _txModeSettingsKey = nullptr;
            qWarning() << "No valid joystick TXmode settings key for selected vehicle";
            return;
        }
    } else {
        _txModeSettingsKey = nullptr;
    }
}

void Joystick::_activeVehicleChanged(Vehicle* activeVehicle)
{
    _updateTXModeSettingsKey(activeVehicle);
    if(activeVehicle) {
        QSettings settings;
        settings.beginGroup(_settingsGroup);
        int mode = settings.value(_txModeSettingsKey, activeVehicle->firmwarePlugin()->defaultJoystickTXMode()).toInt();
        setTXMode(mode);
    }
}

void Joystick::_loadSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    Vehicle* activeVehicle = _multiVehicleManager->activeVehicle();

    if(_txModeSettingsKey && activeVehicle)
        _transmitterMode = settings.value(_txModeSettingsKey, activeVehicle->firmwarePlugin()->defaultJoystickTXMode()).toInt();

    settings.beginGroup(_name);

    bool badSettings = false;
    bool convertOk;

    qCDebug(JoystickLog) << "_loadSettings " << _name;

    _calibrated         = settings.value(_calibratedSettingsKey,        false).toBool();
    _exponential        = settings.value(_exponentialSettingsKey,       0).toFloat();
    _accumulator        = settings.value(_accumulatorSettingsKey,       false).toBool();
    _deadband           = settings.value(_deadbandSettingsKey,          false).toBool();
    _axisFrequencyHz    = settings.value(_axisFrequencySettingsKey,     _defaultAxisFrequencyHz).toFloat();
    _minThrottleFBW     = settings.value(_minThrottleFBWSettingsKey,    25.0).toFloat();
    _buttonFrequencyHz  = settings.value(_buttonFrequencySettingsKey,   _defaultButtonFrequencyHz).toFloat();
    _circleCorrection   = settings.value(_circleCorrectionSettingsKey,  false).toBool();
    _negativeThrust     = settings.value(_negativeThrustSettingsKey,    false).toBool();


    _throttleMode   = static_cast<ThrottleMode_t>(settings.value(_throttleModeSettingsKey, ThrottleModeDownZero).toInt(&convertOk));
    badSettings |= !convertOk;

    qCDebug(JoystickLog) << "_loadSettings calibrated:txmode:throttlemode:exponential:deadband:badsettings" << _calibrated << _transmitterMode << _throttleMode << _exponential << _deadband << badSettings;

    QString minTpl      ("Axis%1Min");
    QString maxTpl      ("Axis%1Max");
    QString trimTpl     ("Axis%1Trim");
    QString revTpl      ("Axis%1Rev");
    QString deadbndTpl  ("Axis%1Deadbnd");

    for (int axis = 0; axis < _axisCount; axis++) {
        Calibration_t* calibration = &_rgCalibration[axis];

        calibration->center = settings.value(trimTpl.arg(axis), 0).toInt(&convertOk);
        badSettings |= !convertOk;

        calibration->min = settings.value(minTpl.arg(axis), -32768).toInt(&convertOk);
        badSettings |= !convertOk;

        calibration->max = settings.value(maxTpl.arg(axis), 32767).toInt(&convertOk);
        badSettings |= !convertOk;

        calibration->deadband = settings.value(deadbndTpl.arg(axis), 0).toInt(&convertOk);
        badSettings |= !convertOk;

        calibration->reversed = settings.value(revTpl.arg(axis), false).toBool();

        qCDebug(JoystickLog) << "_loadSettings axis:min:max:trim:reversed:deadband:badsettings" << axis << calibration->min << calibration->max << calibration->center << calibration->reversed << calibration->deadband << badSettings;
    }

    int workingAxis = 0;
    for (int function = 0; function < maxFunction; function++) {
        int functionAxis;
        functionAxis = settings.value(_rgFunctionSettingsKey[function], -1).toInt(&convertOk);
        badSettings |= !convertOk || (functionAxis >= _axisCount);
        if(functionAxis >= 0) {
            workingAxis++;
        }
        if(functionAxis < _axisCount) {
            _rgFunctionAxis[function] = functionAxis;
        }
        qCDebug(JoystickLog) << "_loadSettings function:axis:badsettings" << function << functionAxis << badSettings;
    }
    badSettings |= workingAxis < 4;

    // FunctionAxis mappings are always stored in TX mode 2
    // Remap to stored TX mode in settings
    _remapAxes(2, _transmitterMode, _rgFunctionAxis);

    for (int button = 0; button < _totalButtonCount; button++) {
        QString a = settings.value(QString(_buttonActionNameKey).arg(button), QString()).toString();
        if(!a.isEmpty() && a != _buttonActionNone) {
            if(_buttonActionArray[button]) {
                _buttonActionArray[button]->deleteLater();
            }
            AssignedButtonAction* ap = new AssignedButtonAction(this, a);
            ap->repeat = settings.value(QString(_buttonActionRepeatKey).arg(button), false).toBool();
            _buttonActionArray[button] = ap;
            _buttonActionArray[button]->buttonTime.start();
            qCDebug(JoystickLog) << "_loadSettings button:action" << button << _buttonActionArray[button]->action << _buttonActionArray[button]->repeat;
        }
    }

    /* NextVision Added Code For Camera Joystick */
    /* ------------------------------------------------------------------------------------------------------*/
    for (int button = 0; button < _totalButtonCount; button++) {
        QString a = settings.value(QString(_buttonCamActionNameKey).arg(button), QString()).toString();
        if(!a.isEmpty() && a != _buttonActionNone) {
            if(_buttonCamActionArray[button]) {
                _buttonCamActionArray[button]->deleteLater();
            }
            AssignedButtonAction* ap = new AssignedButtonAction(this, a);
            ap->repeat = false;
            _buttonCamActionArray[button] = ap;
            _buttonCamActionArray[button]->buttonTime.start();
            qCDebug(JoystickLog) << "_loadSettings Cam button:action" << button << _buttonCamActionArray[button]->action << _buttonCamActionArray[button]->repeat;
        }
    }

    /* read the current camera PitchRoll Axle value */
    _camPitchRollAxle     = settings.value(_camPitchRollAxleKey, 0).toInt();
    /* ------------------------------------------------------------------------------------------------------*/

    if (badSettings) {
        _calibrated = false;
        settings.setValue(_calibratedSettingsKey, false);
    }
}

void Joystick::_saveButtonSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.beginGroup(_name);
    for (int button = 0; button < _totalButtonCount; button++) {
        if(_buttonActionArray[button]) {
            settings.setValue(QString(_buttonActionNameKey).arg(button),        _buttonActionArray[button]->action);
            settings.setValue(QString(_buttonActionRepeatKey).arg(button),      _buttonActionArray[button]->repeat);
            qCDebug(JoystickLog) << "_saveButtonSettings button:action" << button <<  _buttonActionArray[button]->action << _buttonActionArray[button]->repeat;
        }

        /* NextVision Added Code For Camera Joystick */
        /* ------------------------------------------------------------------------------------------------------*/
        if(_buttonCamActionArray[button]) {
            settings.setValue(QString(_buttonCamActionNameKey).arg(button),        _buttonCamActionArray[button]->action);
            settings.setValue(QString(_buttonCamActionRepeatKey).arg(button),      false);
        }
        /* ------------------------------------------------------------------------------------------------------*/
    }
}

void Joystick::_saveSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);

    // Transmitter mode is static
    // Save the mode we are using
    if(_txModeSettingsKey)
        settings.setValue(_txModeSettingsKey,       _transmitterMode);

    settings.beginGroup(_name);
    settings.setValue(_calibratedSettingsKey,       _calibrated);
    settings.setValue(_exponentialSettingsKey,      _exponential);
    settings.setValue(_accumulatorSettingsKey,      _accumulator);
    settings.setValue(_deadbandSettingsKey,         _deadband);
    settings.setValue(_axisFrequencySettingsKey,    _axisFrequencyHz);
    settings.setValue(_minThrottleFBWSettingsKey,   _minThrottleFBW);
    settings.setValue(_buttonFrequencySettingsKey,  _buttonFrequencyHz);
    settings.setValue(_throttleModeSettingsKey,     _throttleMode);
    settings.setValue(_negativeThrustSettingsKey,   _negativeThrust);
    settings.setValue(_circleCorrectionSettingsKey, _circleCorrection);
    settings.setValue(_camPitchRollAxleKey,         _camPitchRollAxle);         /* NextVision */

    qCDebug(JoystickLog) << "_saveSettings calibrated:throttlemode:deadband:txmode" << _calibrated << _throttleMode << _deadband << _circleCorrection << _transmitterMode;

    QString minTpl      ("Axis%1Min");
    QString maxTpl      ("Axis%1Max");
    QString trimTpl     ("Axis%1Trim");
    QString revTpl      ("Axis%1Rev");
    QString deadbndTpl  ("Axis%1Deadbnd");

    for (int axis = 0; axis < _axisCount; axis++) {
        Calibration_t* calibration = &_rgCalibration[axis];
        settings.setValue(trimTpl.arg(axis), calibration->center);
        settings.setValue(minTpl.arg(axis), calibration->min);
        settings.setValue(maxTpl.arg(axis), calibration->max);
        settings.setValue(revTpl.arg(axis), calibration->reversed);
        settings.setValue(deadbndTpl.arg(axis), calibration->deadband);
       /*
        qCDebug(JoystickLog) << "_saveSettings name:axis:min:max:trim:reversed:deadband"
                                << _name
                                << axis
                                << calibration->min
                                << calibration->max
                                << calibration->center
                                << calibration->reversed
                                << calibration->deadband;
                                */
    }

    // Always save function Axis mappings in TX Mode 2
    // Write mode 2 mappings without changing mapping currently in use
    int temp[maxFunction];
    _remapAxes(_transmitterMode, 2, temp);
    for (int function = 0; function < maxFunction; function++) {
        settings.setValue(_rgFunctionSettingsKey[function], temp[function]);
        //qCDebug(JoystickLog) << "_saveSettings name:function:axis" << _name << function << _rgFunctionSettingsKey[function];
    }
    _saveButtonSettings();
}

// Relative mappings of axis functions between different TX modes
int Joystick::_mapFunctionMode(int mode, int function) {
    static const int mapping[][6] = {
        { yawFunction, pitchFunction, rollFunction, throttleFunction, gimbalPitchFunction, gimbalYawFunction },
        { yawFunction, throttleFunction, rollFunction, pitchFunction, gimbalPitchFunction, gimbalYawFunction },
        { rollFunction, pitchFunction, yawFunction, throttleFunction, gimbalPitchFunction, gimbalYawFunction },
        { rollFunction, throttleFunction, yawFunction, pitchFunction, gimbalPitchFunction, gimbalYawFunction }};
    return mapping[mode-1][function];
}

// Remap current axis functions from current TX mode to new TX mode
void Joystick::_remapAxes(int currentMode, int newMode, int (&newMapping)[maxFunction]) {
    int temp[maxFunction];
    for(int function = 0; function < maxFunction; function++) {
        temp[_mapFunctionMode(newMode, function)] = _rgFunctionAxis[_mapFunctionMode(currentMode, function)];
    }
    for(int function = 0; function < maxFunction; function++) {
        newMapping[function] = temp[function];
    }
}

void Joystick::setTXMode(int mode) {
    if(mode > 0 && mode <= 4) {
        _remapAxes(_transmitterMode, mode, _rgFunctionAxis);
        _transmitterMode = mode;
        _saveSettings();
    } else {
        qCWarning(JoystickLog) << "Invalid mode:" << mode;
    }
}

/// Adjust the raw axis value to the -1:1 range given calibration information
float Joystick::_adjustRange(int value, Calibration_t calibration, bool withDeadbands)
{
    float valueNormalized;
    float axisLength;
    float axisBasis;

    if (value > calibration.center) {
        axisBasis = 1.0f;
        valueNormalized = value - calibration.center;
        axisLength =  calibration.max - calibration.center;
    } else {
        axisBasis = -1.0f;
        valueNormalized = calibration.center - value;
        axisLength =  calibration.center - calibration.min;
    }

    float axisPercent;

    if (withDeadbands) {
        if (valueNormalized>calibration.deadband) {
            axisPercent = (valueNormalized - calibration.deadband) / (axisLength - calibration.deadband);
        } else if (valueNormalized<-calibration.deadband) {
            axisPercent = (valueNormalized + calibration.deadband) / (axisLength - calibration.deadband);
        } else {
            axisPercent = 0.f;
        }
    }
    else {
        axisPercent = valueNormalized / axisLength;
    }

    float correctedValue = axisBasis * axisPercent;

    if (calibration.reversed) {
        correctedValue *= -1.0f;
    }

#if 0
    qCDebug(JoystickLog) << "_adjustRange corrected:value:min:max:center:reversed:deadband:basis:normalized:length"
                            << correctedValue
                            << value
                            << calibration.min
                            << calibration.max
                            << calibration.center
                            << calibration.reversed
                            << calibration.deadband
                            << axisBasis
                            << valueNormalized
                            << axisLength;
#endif

    return std::max(-1.0f, std::min(correctedValue, 1.0f));
}


void Joystick::run()
{
    float roll_yaw = 0, pitch = 0;
    Calibration_t   roll_caib;
    Calibration_t   pitch_caib;
    float roll_yaw_DZ = 0, pitch_DZ = 0;

    //-- Joystick thread
    _open();
    //-- Reset timers
    _axisTime.start();
    _camTimeDivider = 0;
    for (int buttonIndex = 0; buttonIndex < _totalButtonCount; buttonIndex++) {
        if(_buttonActionArray[buttonIndex]) {
            _buttonActionArray[buttonIndex]->buttonTime.start();
        }
    }
    while (!_exitThread) {
        _update();
        if ( _is_same_joystick )  //when the camera and vehicle are using the same joystick
        {
            _handleButtons();
            _handleAxis();

            if ( _activeVehicle->joystickCamEnabled() )
            {
                if( _camTimeDivider++ >= 3 )
                {

                    switch ( _camPitchRollAxle )
                    {
                        case 0:
                        {
                            _handleCamHat(&roll_yaw,&pitch);
                            roll_caib.max = 32768;
                            roll_caib.min = -32768;
                            pitch_caib.max = 32768;
                            pitch_caib.min = -32768;
                        }
                        break;

                        case 1:
                        {
                            roll_yaw = _getAxis(0);
                            pitch = _getAxis(1);
                            /* apply inversion */
                            if(_camJoystickRollInvert)
                                roll_yaw *= -1;
                            if(_camJoystickPitchInvert)
                                pitch *= -1;
                            roll_caib = getCalibration(0);
                            pitch_caib = getCalibration(1);
                        }
                        break;

                        case 2:
                        {
                            roll_yaw = _getAxis(2);
                            pitch = _getAxis(3);
                            /* apply inversion */
                            if(_camJoystickRollInvert)
                                roll_yaw *= -1;
                            if(_camJoystickPitchInvert)
                                pitch *= -1;
                            roll_caib = getCalibration(2);
                            pitch_caib = getCalibration(3);
                        }
                        break;

                        default:
                            QGC::SLEEP::msleep(20);
                        continue;
                    }

                    _camTimeDivider = 0;

                    if ( _camPitchRollAxle != 0 )
                    {
                        /* calculate the Dead Zone */
                        roll_yaw_DZ  = (_camJoystickDZ * roll_caib.max) / 100;
                        pitch_DZ  = (_camJoystickDZ * pitch_caib.max) / 100;

                        /* check deadzone - roll */
                        if ( roll_yaw > -roll_yaw_DZ && roll_yaw < roll_yaw_DZ )
                            roll_yaw = 0;

                        /* check deadzone - pitch */
                        if ( pitch > -pitch_DZ && pitch < pitch_DZ )
                            pitch = 0;
                    }

                    /* Apply Gain */
                    roll_yaw *= ((float)_camJoystickGain / (float)100.0);
                    pitch *= ((float)_camJoystickGain / (float)100.0);

                    /* Apply limiting */
                    if ( roll_yaw > roll_caib.max )
                        roll_yaw = roll_caib.max;
                    if ( roll_yaw < roll_caib.min )
                        roll_yaw = roll_caib.min;
                    if ( pitch > pitch_caib.max )
                        pitch = pitch_caib.max;
                    if ( pitch < pitch_caib.min )
                        pitch = pitch_caib.min;

                    emit manualControlCam(roll_yaw,pitch,_rgButtonValues);
                    emit manualControlCamQml(roll_yaw,pitch);
                }
            }
        }
        else if ( !_is_cam_joystick )
        {
            _handleButtons();
            _handleAxis();
        }
        else if ( _is_cam_joystick )
        {
            if ( _activeVehicle->joystickCamEnabled() )
            {
                if( _camTimeDivider++ >= 3 )
                {
                    //-- Update axis
                    for (int axisIndex = 0; axisIndex < _axisCount; axisIndex++) {
                        int newAxisValue = _getAxis(axisIndex);
                        // Calibration code requires signal to be emitted even if value hasn't changed
                        _rgAxisValues[axisIndex] = newAxisValue;
                        //   qDebug() << "newAxisValue[" << axisIndex <<"] = " << newAxisValue;
                    }

                    //-- Update button states
                    for (int buttonIndex = 0; buttonIndex < _buttonCount; buttonIndex++) {
                        bool newButtonValue = _getButton(buttonIndex);
                        if (newButtonValue && _rgButtonValues[buttonIndex] == BUTTON_UP)
                            _rgButtonValues[buttonIndex] = BUTTON_DOWN;
                        else if (!newButtonValue && _rgButtonValues[buttonIndex] != BUTTON_UP)
                            _rgButtonValues[buttonIndex] = BUTTON_UP;
                    }
                    //-- Update hat - append hat buttons to the end of the normal button list
                    for (int hatButtonIndex = 0; hatButtonIndex < 4; hatButtonIndex++) {
                        int rgButtonValueIndex = hatButtonIndex + _buttonCount;
                        bool newButtonValue = _getHat(0, hatButtonIndex);
                        if (newButtonValue && _rgButtonValues[rgButtonValueIndex] == BUTTON_UP)
                            _rgButtonValues[rgButtonValueIndex] = BUTTON_DOWN;
                         else if (!newButtonValue && _rgButtonValues[rgButtonValueIndex] != BUTTON_UP)
                            _rgButtonValues[rgButtonValueIndex] = BUTTON_UP;
                    }

                    switch ( _camPitchRollAxle )
                    {
                        case 0:
                        {
                            _handleCamHat(&roll_yaw,&pitch);
                            roll_caib.max = 32768;
                            roll_caib.min = -32768;
                            pitch_caib.max = 32768;
                            pitch_caib.min = -32768;
                        }
                        break;

                        case 1:
                        {
                            roll_yaw = _getAxis(0);
                            pitch = _getAxis(1);
                            /* apply inversion */
                            if(_camJoystickRollInvert)
                                roll_yaw *= -1;
                            if(_camJoystickPitchInvert)
                                pitch *= -1;
                            roll_caib = getCalibration(0);
                            pitch_caib = getCalibration(1);
                        }
                        break;

                        case 2:
                        {
                            roll_yaw = _getAxis(2);
                            pitch = _getAxis(3);
                            /* apply inversion */
                            if(_camJoystickRollInvert)
                                roll_yaw *= -1;
                            if(_camJoystickPitchInvert)
                                pitch *= -1;
                            roll_caib = getCalibration(2);
                            pitch_caib = getCalibration(3);
                        }
                        break;

                        default:
                            QGC::SLEEP::msleep(20);
                        continue;
                    }

                    _camTimeDivider = 0;

                    if ( _camPitchRollAxle != 0 )
                    {
                        /* calculate the Dead Zone */
                        roll_yaw_DZ  = (_camJoystickDZ * roll_caib.max) / 100;
                        pitch_DZ  = (_camJoystickDZ * pitch_caib.max) / 100;

                        /* check deadzone - roll */
                        if ( roll_yaw > -roll_yaw_DZ && roll_yaw < roll_yaw_DZ )
                            roll_yaw = 0;

                        /* check deadzone - pitch */
                        if ( pitch > -pitch_DZ && pitch < pitch_DZ )
                            pitch = 0;
                    }

                    /* Apply Gain */
                    roll_yaw *= ((float)_camJoystickGain / (float)100.0);
                    pitch *= ((float)_camJoystickGain / (float)100.0);

                    /* Apply limiting */
                    if ( roll_yaw > roll_caib.max )
                        roll_yaw = roll_caib.max;
                    if ( roll_yaw < roll_caib.min )
                        roll_yaw = roll_caib.min;
                    if ( pitch > pitch_caib.max )
                        pitch = pitch_caib.max;
                    if ( pitch < pitch_caib.min )
                        pitch = pitch_caib.min;

                    emit manualControlCam(roll_yaw,pitch,_rgButtonValues);
                    emit manualControlCamQml(roll_yaw,pitch);
                }
            }
        }
    QGC::SLEEP::msleep(20);
    }
    _close();
}

void Joystick::_handleButtons()
{
    int lastBbuttonValues[256];
    //-- Update button states
    for (int buttonIndex = 0; buttonIndex < _buttonCount; buttonIndex++) {
        bool newButtonValue = _getButton(buttonIndex);
        if(buttonIndex < 256)
            lastBbuttonValues[buttonIndex] = _rgButtonValues[buttonIndex];
        if (newButtonValue && _rgButtonValues[buttonIndex] == BUTTON_UP) {
            _rgButtonValues[buttonIndex] = BUTTON_DOWN;
            emit rawButtonPressedChanged(buttonIndex, newButtonValue);
        } else if (!newButtonValue && _rgButtonValues[buttonIndex] != BUTTON_UP) {
            _rgButtonValues[buttonIndex] = BUTTON_UP;
            emit rawButtonPressedChanged(buttonIndex, newButtonValue);
        }
    }
    //-- Update hat - append hat buttons to the end of the normal button list
    int numHatButtons = 4;
    for (int hatIndex = 0; hatIndex < _hatCount; hatIndex++) {
        for (int hatButtonIndex = 0; hatButtonIndex<numHatButtons; hatButtonIndex++) {
            // Create new index value that includes the normal button list
            int rgButtonValueIndex = hatIndex*numHatButtons + hatButtonIndex + _buttonCount;
            // Get hat value from joystick
            bool newButtonValue = _getHat(hatIndex, hatButtonIndex);
            if (newButtonValue && _rgButtonValues[rgButtonValueIndex] == BUTTON_UP) {
                _rgButtonValues[rgButtonValueIndex] = BUTTON_DOWN;
                emit rawButtonPressedChanged(rgButtonValueIndex, newButtonValue);
            } else if (!newButtonValue && _rgButtonValues[rgButtonValueIndex] != BUTTON_UP) {
                _rgButtonValues[rgButtonValueIndex] = BUTTON_UP;
                emit rawButtonPressedChanged(rgButtonValueIndex, newButtonValue);
            }
        }
    }
    //-- Process button press/release
    for (int buttonIndex = 0; buttonIndex < _totalButtonCount; buttonIndex++) {
        if(_rgButtonValues[buttonIndex] == BUTTON_DOWN || _rgButtonValues[buttonIndex] == BUTTON_REPEAT) {
            if(_buttonActionArray[buttonIndex]) {
                QString buttonAction = _buttonActionArray[buttonIndex]->action;
                if(buttonAction.isEmpty() || buttonAction == _buttonActionNone)
                    continue;
                if(!_buttonActionArray[buttonIndex]->repeat) {
                    //-- This button just went down
                    if(_rgButtonValues[buttonIndex] == BUTTON_DOWN) {
                        // Check for a multi-button action
                        QList<int> rgButtons = { buttonIndex };
                        bool executeButtonAction = true;
                        for (int multiIndex = 0; multiIndex < _totalButtonCount; multiIndex++) {
                            if (multiIndex != buttonIndex) {
                                if (_buttonActionArray[multiIndex] && _buttonActionArray[multiIndex]->action == buttonAction) {
                                    // We found a multi-button action
                                    if (_rgButtonValues[multiIndex] == BUTTON_DOWN || _rgButtonValues[multiIndex] == BUTTON_REPEAT) {
                                        // So far so good
                                        rgButtons.append(multiIndex);
                                        continue;
                                    } else {
                                        // We are missing a press we need
                                        executeButtonAction = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (executeButtonAction) {
                            qCDebug(JoystickLog) << "Action triggered" << rgButtons << buttonAction;
                            _executeButtonAction(buttonAction, true);
                        }
                    }
                } else {
                    //-- Process repeat buttons
                    int buttonDelay = static_cast<int>(1000.0f / _buttonFrequencyHz);
                    if(_buttonActionArray[buttonIndex]->buttonTime.elapsed() > buttonDelay) {
                        _buttonActionArray[buttonIndex]->buttonTime.start();
                        qCDebug(JoystickLog) << "Repeat button triggered" << buttonIndex << buttonAction;
                        _executeButtonAction(buttonAction, true);
                    }
                }
            }
            //-- Flag it as processed
            _rgButtonValues[buttonIndex] = BUTTON_REPEAT;
        } else if(_rgButtonValues[buttonIndex] == BUTTON_UP) {
            //-- Button up transition
            if(buttonIndex < 256) {
                if(lastBbuttonValues[buttonIndex] == BUTTON_DOWN || lastBbuttonValues[buttonIndex] == BUTTON_REPEAT) {
                    if(_buttonActionArray[buttonIndex]) {
                        QString buttonAction = _buttonActionArray[buttonIndex]->action;
                        if(buttonAction.isEmpty() || buttonAction == _buttonActionNone)
                            continue;
                        qCDebug(JoystickLog) << "Button up" << buttonIndex << buttonAction;
                        _executeButtonAction(buttonAction, false);
                    }
                }
            }
        }
    }
}

void Joystick::_handleCamHat(float *roll_yaw, float *pitch)
{
    bool up = _getHat(0, 0);
    bool down = _getHat(0, 1);
    bool left = _getHat(0, 2);
    bool right = _getHat(0, 3);

    /* calculate the Axle value */
    if ( up )
        *pitch -= NV_HAT_INC;
    else if ( down )
        *pitch += NV_HAT_INC;
    else
        *pitch = 0;

    if ( left )
        *roll_yaw -= NV_HAT_INC;
    else if ( right )
        *roll_yaw += NV_HAT_INC;
    else
        *roll_yaw = 0;

    /* rol_yaw / pitch limiting */
    if ( *roll_yaw > 32768 )
        *roll_yaw = 32768;
    else if ( *roll_yaw < -32768 )
        *roll_yaw = -32768;

    if ( *pitch > 32768 )
        *pitch = 32768;
    else if ( *pitch < -32768 )
        *pitch = -32768;
}
/*------------------------------------------------------------------------------------------------------*/

void Joystick::_handleAxis()
{
    //-- Get frequency
    int axisDelay = static_cast<int>(1000.0f / _axisFrequencyHz);
    //-- Check elapsed time since last run
    if(_axisTime.elapsed() > axisDelay) {
        _axisTime.start();
        //-- Update axis
        for (int axisIndex = 0; axisIndex < _axisCount; axisIndex++) {
            int newAxisValue = _getAxis(axisIndex);
            // Calibration code requires signal to be emitted even if value hasn't changed
            _rgAxisValues[axisIndex] = newAxisValue;
            emit rawAxisValueChanged(axisIndex, newAxisValue);
        }
        if (_activeVehicle->joystickEnabled() && !_calibrationMode && _calibrated) {
            int     axis = _rgFunctionAxis[rollFunction];
            float   roll = _adjustRange(_rgAxisValues[axis],    _rgCalibration[axis], _deadband);

                    axis = _rgFunctionAxis[pitchFunction];
            float   pitch = _adjustRange(_rgAxisValues[axis],   _rgCalibration[axis], _deadband);

                    axis = _rgFunctionAxis[yawFunction];
            float   yaw = _adjustRange(_rgAxisValues[axis],     _rgCalibration[axis],_deadband);

                    axis = _rgFunctionAxis[throttleFunction];
            float   throttle = _adjustRange(_rgAxisValues[axis],_rgCalibration[axis], _throttleMode==ThrottleModeDownZero?false:_deadband);

            float   gimbalPitch = 0.0f;
            float   gimbalYaw   = 0.0f;

            if(_axisCount > 4) {
                axis = _rgFunctionAxis[gimbalPitchFunction];
                gimbalPitch = _adjustRange(_rgAxisValues[axis], _rgCalibration[axis],_deadband);
            }

            if(_axisCount > 5) {
                axis = _rgFunctionAxis[gimbalYawFunction];
                gimbalYaw = _adjustRange(_rgAxisValues[axis],   _rgCalibration[axis],_deadband);
            }

            if (_accumulator) {
                //static float throttle_accu = 0.f;
                _throttle_accu += throttle * (40 / 3000.f); //for throttle to change from min to max it will take 3000ms (40ms is a loop time)
                _throttle_accu = std::max(static_cast<float>(-1.f), std::min(_throttle_accu, static_cast<float>(1.f)));
                //qDebug() << "throttle is " << _throttle_accu;
                float mimimumThrottle = _minThrottleFBW * 1.0e-2;
                //qDebug() << "min throttle is " << mimimumThrottle;
                _throttle_accu = std::max(mimimumThrottle, _throttle_accu);
                throttle = _throttle_accu;
            }

            if (_circleCorrection) {
                float roll_limited      = std::max(static_cast<float>(-M_PI_4), std::min(roll,      static_cast<float>(M_PI_4)));
                float pitch_limited     = std::max(static_cast<float>(-M_PI_4), std::min(pitch,     static_cast<float>(M_PI_4)));
                float yaw_limited       = std::max(static_cast<float>(-M_PI_4), std::min(yaw,       static_cast<float>(M_PI_4)));
                float throttle_limited  = std::max(static_cast<float>(-M_PI_4), std::min(throttle,  static_cast<float>(M_PI_4)));

                // Map from unit circle to linear range and limit
                roll =      std::max(-1.0f, std::min(tanf(asinf(roll_limited)),     1.0f));
                pitch =     std::max(-1.0f, std::min(tanf(asinf(pitch_limited)),    1.0f));
                yaw =       std::max(-1.0f, std::min(tanf(asinf(yaw_limited)),      1.0f));
                throttle =  std::max(-1.0f, std::min(tanf(asinf(throttle_limited)), 1.0f));
            }

            if ( _exponential < -0.01f) {
                // Exponential (0% to -50% range like most RC radios)
                // _exponential is set by a slider in joystickConfigAdvanced.qml
                // Calculate new RPY with exponential applied
                roll =  -_exponential*powf(roll, 3) + (1+_exponential)*roll;
                pitch = -_exponential*powf(pitch,3) + (1+_exponential)*pitch;
                yaw =   -_exponential*powf(yaw,  3) + (1+_exponential)*yaw;
            }

            // Adjust throttle to 0:1 range
            if (_throttleMode == ThrottleModeCenterZero && _activeVehicle->supportsThrottleModeCenterZero()) {
                if (!_activeVehicle->supportsNegativeThrust() || !_negativeThrust) {
                    throttle = std::max(0.0f, throttle);
                }
            } else {
                throttle = (throttle + 1.0f) / 2.0f;
            }
            qCDebug(JoystickValuesLog) << "name:roll:pitch:yaw:throttle:gimbalPitch:gimbalYaw" << name() << roll << -pitch << yaw << throttle << gimbalPitch << gimbalYaw;
            // NOTE: The buttonPressedBits going to MANUAL_CONTROL are currently used by ArduSub (and it only handles 16 bits)
            // Set up button bitmap
            quint64 buttonPressedBits = 0;  // Buttons pressed for manualControl signal
            for (int buttonIndex = 0; buttonIndex < _totalButtonCount; buttonIndex++) {
                quint64 buttonBit = static_cast<quint64>(1LL << buttonIndex);
                if (_rgButtonValues[buttonIndex] != BUTTON_UP) {
                    // Mark the button as pressed as long as its pressed
                    buttonPressedBits |= buttonBit;
                }
            }
            emit axisValues(roll, pitch, yaw, throttle);

            uint16_t shortButtons = static_cast<uint16_t>(buttonPressedBits & 0xFFFF);

            if (_joystickPitchRollEnableOption == 1 && !_rollPitchEnabled)
            {
                roll = 0;
                pitch = 0;
            }
            if (!_runupEnabled)
                _activeVehicle->sendJoystickDataThreadSafe(roll, pitch, yaw, throttle, shortButtons);
        }
    }
}

void Joystick::startPolling(Vehicle* vehicle)
{
    if (vehicle) {              /* NextVision */
        if ( _is_same_joystick || !_is_cam_joystick) {
            // If a vehicle is connected, disconnect it
            if (_activeVehicle ) {
                disconnect(this, &Joystick::setArmed,           _activeVehicle, &Vehicle::setArmedShowError);
                disconnect(this, &Joystick::setVtolInFwdFlight, _activeVehicle, &Vehicle::setVtolInFwdFlight);
                disconnect(this, &Joystick::setFlightMode,      _activeVehicle, &Vehicle::setFlightMode);
                disconnect(this, &Joystick::gimbalPitchStep,    _activeVehicle, &Vehicle::gimbalPitchStep);
                disconnect(this, &Joystick::gimbalYawStep,      _activeVehicle, &Vehicle::gimbalYawStep);
                disconnect(this, &Joystick::centerGimbal,       _activeVehicle, &Vehicle::centerGimbal);
                disconnect(this, &Joystick::gimbalControlValue, _activeVehicle, &Vehicle::gimbalControlValue);
            }
            // Always set up the new vehicle
            _activeVehicle = vehicle;
            // If joystick is not calibrated, disable it
            if ( !_calibrated ) {
                vehicle->setJoystickEnabled(false);
            }
            // Update qml in case of joystick transition
            emit calibratedChanged(_calibrated);

            // Build action list
            _buildActionList(vehicle);
            _buildCamActionList();
            // Only connect the new vehicle if it wants joystick data
            if (vehicle->joystickEnabled() ) {
                _pollingStartedForCalibration = false;
                connect(this, &Joystick::setArmed,           _activeVehicle, &Vehicle::setArmedShowError);
                connect(this, &Joystick::setVtolInFwdFlight, _activeVehicle, &Vehicle::setVtolInFwdFlight);
                connect(this, &Joystick::setFlightMode,      _activeVehicle, &Vehicle::setFlightMode);
                connect(this, &Joystick::gimbalPitchStep,    _activeVehicle, &Vehicle::gimbalPitchStep);
                connect(this, &Joystick::gimbalYawStep,      _activeVehicle, &Vehicle::gimbalYawStep);
                connect(this, &Joystick::centerGimbal,       _activeVehicle, &Vehicle::centerGimbal);
                connect(this, &Joystick::gimbalControlValue, _activeVehicle, &Vehicle::gimbalControlValue);
                // FIXME: ****
                //connect(this, &Joystick::buttonActionTriggered, uas, &UAS::triggerAction);
            }
        }
        else if ( _is_cam_joystick )
        {
            _buildCamActionList();
        }
    }
    if (!isRunning()) {
        _exitThread = false;
        start();
    }
}

void Joystick::stopPolling(void)
{
    if (isRunning()) {      /* NextVision */
        if (_activeVehicle && _activeVehicle->joystickEnabled() && ( _is_same_joystick || !_is_cam_joystick)) {
            disconnect(this, &Joystick::setArmed,           _activeVehicle, &Vehicle::setArmedShowError);
            disconnect(this, &Joystick::setVtolInFwdFlight, _activeVehicle, &Vehicle::setVtolInFwdFlight);
            disconnect(this, &Joystick::setFlightMode,      _activeVehicle, &Vehicle::setFlightMode);
            disconnect(this, &Joystick::gimbalPitchStep,    _activeVehicle, &Vehicle::gimbalPitchStep);
            disconnect(this, &Joystick::gimbalYawStep,      _activeVehicle, &Vehicle::gimbalYawStep);
            disconnect(this, &Joystick::centerGimbal,       _activeVehicle, &Vehicle::centerGimbal);
            disconnect(this, &Joystick::gimbalControlValue, _activeVehicle, &Vehicle::gimbalControlValue);
        }
        _exitThread = true;
    }
}

void Joystick::setCalibration(int axis, Calibration_t& calibration)
{
    if (!_validAxis(axis)) {
        return;
    }
    _calibrated = true;
    _rgCalibration[axis] = calibration;
    _saveSettings();
    emit calibratedChanged(_calibrated);
}

Joystick::Calibration_t Joystick::getCalibration(int axis)
{
    if (!_validAxis(axis)) {
        return Calibration_t();
    }
    return _rgCalibration[axis];
}

void Joystick::setFunctionAxis(AxisFunction_t function, int axis)
{
    if (!_validAxis(axis)) {
        return;
    }
    _calibrated = true;
    _rgFunctionAxis[function] = axis;
    _saveSettings();
    emit calibratedChanged(_calibrated);
}

void Joystick::setRunupEnabled(bool enabled)
{
    _runupEnabled = enabled;
}

int Joystick::getFunctionAxis(AxisFunction_t function)
{
    if (static_cast<int>(function) < 0 || function >= maxFunction) {
        qCWarning(JoystickLog) << "Invalid function" << function;
    }
    return _rgFunctionAxis[function];
}

void Joystick::setButtonRepeat(int button, bool repeat)
{
    if (!_validButton(button) || !_buttonActionArray[button]) {
        return;
    }
    _buttonActionArray[button]->repeat = repeat;
    _buttonActionArray[button]->buttonTime.start();
    //-- Save to settings
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.beginGroup(_name);
    settings.setValue(QString(_buttonActionRepeatKey).arg(button), _buttonActionArray[button]->repeat);
}

bool Joystick::getButtonRepeat(int button)
{
    if (!_validButton(button) || !_buttonActionArray[button]) {
        return false;
    }
    return _buttonActionArray[button]->repeat;
}

void Joystick::setButtonAction(int button, const QString& action)
{
    if (!_validButton(button)) {
        return;
    }
    qCWarning(JoystickLog) << "setButtonAction:" << button << action;
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.beginGroup(_name);
    if(action.isEmpty() || action == _buttonActionNone) {
        if(_buttonActionArray[button]) {
            _buttonActionArray[button]->deleteLater();
            _buttonActionArray[button] = nullptr;
            //-- Clear from settings
            settings.remove(QString(_buttonActionNameKey).arg(button));
            settings.remove(QString(_buttonActionRepeatKey).arg(button));
        }
    } else {
        if(!_buttonActionArray[button]) {
            _buttonActionArray[button] = new AssignedButtonAction(this, action);
        } else {
            _buttonActionArray[button]->action = action;
            //-- Make sure repeat is off if this action doesn't support repeats
            int idx = _findAssignableButtonAction(action);
            if(idx >= 0) {
                AssignableButtonAction* p = qobject_cast<AssignableButtonAction*>(_assignableButtonActions[idx]);
                if(!p->canRepeat()) {
                    _buttonActionArray[button]->repeat = false;
                }
            }
        }
        //-- Save to settings
        settings.setValue(QString(_buttonActionNameKey).arg(button),   _buttonActionArray[button]->action);
        settings.setValue(QString(_buttonActionRepeatKey).arg(button), _buttonActionArray[button]->repeat);
    }
    emit buttonActionsChanged();
}

QString Joystick::getButtonAction(int button)
{
    if (_validButton(button)) {
        if(_buttonActionArray[button]) {
            return _buttonActionArray[button]->action;
        }
    }
    return QString(_buttonActionNone);
}

QStringList Joystick::buttonActions()
{
    QStringList list;
    for (int button = 0; button < _totalButtonCount; button++) {
        list << getButtonAction(button);
    }
    return list;
}

/* NextVision Added Code For Camera Joystick */
/* ------------------------------------------------------------------------------------------------------*/
void Joystick::setButtonCamAction(int button, const QString& action)
{
    if (!_validButton(button)) {
        return;
    }
    qCWarning(JoystickLog) << "setButtonCamAction:" << button << action;
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.beginGroup(_name);
    if(action.isEmpty() || action == _buttonActionNone) {
        if(_buttonCamActionArray[button]) {
            _buttonCamActionArray[button]->deleteLater();
            _buttonCamActionArray[button] = nullptr;
            //-- Clear from settings
            settings.remove(QString(_buttonCamActionNameKey).arg(button));
            settings.remove(QString(_buttonCamActionRepeatKey).arg(button));
        }
    } else {
        if(!_buttonCamActionArray[button]) {
            _buttonCamActionArray[button] = new AssignedButtonAction(this, action);
        } else {
            _buttonCamActionArray[button]->action = action;
            _buttonCamActionArray[button]->repeat = false;
        }
        //-- Save to settings
        settings.setValue(QString(_buttonCamActionNameKey).arg(button),   _buttonCamActionArray[button]->action);
        settings.setValue(QString(_buttonCamActionRepeatKey).arg(button), false);
    }
    emit buttonCamActionsChanged();
}

QString Joystick::getButtonCamAction(int button)
{
    if (_validButton(button)) {
        if(_buttonCamActionArray[button]) {
            return _buttonCamActionArray[button]->action;
        }
    }
    return QString(_buttonActionNone);
}

QStringList Joystick::buttonCamActions()
{
    QStringList list;
    for (int button = 0; button < _totalButtonCount; button++) {
        list << getButtonCamAction(button);
    }
    return list;
}

void Joystick::setCamPitchRollAxle(int axle)
{
    _camPitchRollAxle = axle;
    _saveSettings();
    emit camPitchRollAxleChanged();
}

void Joystick::_buildCamActionList()
{
    if(_assignableCamButtonActions.count())
        _assignableCamButtonActions.clearAndDeleteContents();
    _availableCamActionTitles.clear();

    //-- Available Camera Actions
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionNone));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionZoomIn));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionZoomOut));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionDayIR));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionWHBH));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionCM));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionNUC));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionSnap));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionRec));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionSY));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionOBS));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionGRR));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionStow));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionPilot));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionRetract));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionHoldCord));
    _assignableCamButtonActions.append(new AssignableButtonAction(this, _buttonActionOverrideStick));

    for(int i = 0; i < _assignableCamButtonActions.count(); i++) {
        AssignableButtonAction* p = qobject_cast<AssignableButtonAction*>(_assignableCamButtonActions[i]);
        _availableCamActionTitles << p->action();
    }
    emit assignableCamActionsChanged();
}

void Joystick::setCamJoystickPitchRollEnableOption(int mode)
{
    //qDebug() << "setting pitch roll enable option to " << mode;
    _joystickPitchRollEnableOption = mode;
}

void Joystick::setCamJoystickDZ(int DZ)
{
    _camJoystickDZ = DZ;
}

void Joystick::setCamJoystickGain(int gain)
{
    _camJoystickGain = gain;
}

void Joystick::setCamJoystickRollInvert(bool value)
{
    _camJoystickRollInvert = value;
}

void Joystick::setCamJoystickPitchInvert(bool value)
{
    _camJoystickPitchInvert = value;
}

void Joystick::setRollPitchEnabled(bool value)
{    
    _rollPitchEnabled = value;
    //if I want to know about this from qml, emit a signal
    emit rollPitchEnabled(value);
}

/* ------------------------------------------------------------------------------------------------------*/

int Joystick::throttleMode()
{
    return _throttleMode;
}
void Joystick::setThrottleAccumulatorValue(float value)
{
    _throttle_accu = value;
}
void Joystick::setThrottleMode(int mode)
{
    if (mode < 0 || mode >= ThrottleModeMax) {
        qCWarning(JoystickLog) << "Invalid throttle mode" << mode;
        return;
    }
    _throttleMode = static_cast<ThrottleMode_t>(mode);
    if (_throttleMode == ThrottleModeDownZero) {
        setAccumulator(false);
    }
    _saveSettings();
    emit throttleModeChanged(_throttleMode);
}

bool Joystick::negativeThrust() const
{
    return _negativeThrust;
}

void Joystick::setNegativeThrust(bool allowNegative)
{
    if (_negativeThrust == allowNegative) {
        return;
    }
    _negativeThrust = allowNegative;
    _saveSettings();
    emit negativeThrustChanged(_negativeThrust);
}

float Joystick::exponential() const
{
    return _exponential;
}

void Joystick::setExponential(float expo)
{
    _exponential = expo;
    _saveSettings();
    emit exponentialChanged(_exponential);
}

bool Joystick::accumulator() const
{
    return _accumulator;
}

void Joystick::setAccumulator(bool accu)
{
    _accumulator = accu;
    _saveSettings();
    emit accumulatorChanged(_accumulator);
}

bool Joystick::deadband() const
{
    return _deadband;
}

void Joystick::setDeadband(bool deadband)
{
    _deadband = deadband;
    _saveSettings();
}

bool Joystick::circleCorrection() const
{
    return _circleCorrection;
}

void Joystick::setCircleCorrection(bool circleCorrection)
{
    _circleCorrection = circleCorrection;
    _saveSettings();
    emit circleCorrectionChanged(_circleCorrection);
}
void Joystick::setMinThrottleFBW(float val)
{
    qDebug() << "setting min throttle fbw to " << val;
    if (val < 0)
        val = 0.0f;
    if (val > 30)
        val = 30.0f;
    _minThrottleFBW = val;
    _saveSettings();
    emit minThrottleFBWChanged();
}
void Joystick::setAxisFrequency(float val)
{
    //-- Arbitrary limits
    val = qMin(_minAxisFrequencyHz, val);
    val = qMax(_maxAxisFrequencyHz, val);
    _axisFrequencyHz = val;
    _saveSettings();
    emit axisFrequencyHzChanged();
}

void Joystick::setButtonFrequency(float val)
{
    //-- Arbitrary limits
    val = qMin(_minButtonFrequencyHz, val);
    val = qMax(_maxButtonFrequencyHz, val);
    _buttonFrequencyHz = val;
    _saveSettings();
    emit buttonFrequencyHzChanged();
}

void Joystick::setCalibrationMode(bool calibrating)
{
    _calibrationMode = calibrating;
    if (calibrating && !isRunning()) {
        _pollingStartedForCalibration = true;
        startPolling(_multiVehicleManager->activeVehicle());
    }
    else if (_pollingStartedForCalibration) {
        stopPolling();
    }
}


void Joystick::_executeButtonAction(const QString& action, bool buttonDown)
{   

    if (!_activeVehicle || !_activeVehicle->joystickEnabled() || action == _buttonActionNone) {
        return;
    }
    if (action == _buttonActionArm) {
        if (buttonDown) emit setArmed(true);
    } else if (action == _buttonActionDisarm) {
        if (buttonDown) emit setArmed(false);
    } else if (action == _buttonActionToggleArm) {
        if (buttonDown) emit setArmed(!_activeVehicle->armed());
    } else if (action == _buttonActionVTOLFixedWing) {
        if (buttonDown) emit setVtolInFwdFlight(true);
    } else if (action == _buttonActionVTOLMultiRotor) {
        if (buttonDown) emit setVtolInFwdFlight(false);
    } else if (_activeVehicle->flightModes().contains(action) || _activeVehicle->extraJoystickFlightModes().contains(action)) {
        if (buttonDown)
        {
            //qDebug() << "setting flight mode from joystick" << action;
            emit setFlightMode(action);
        }
    } else if(action == _buttonActionContinuousZoomIn || action == _buttonActionContinuousZoomOut) {
        if (buttonDown) {
            emit startContinuousZoom(action == _buttonActionContinuousZoomIn ? 1 : -1);
        } else {
            emit stopContinuousZoom();
        }
    } else if(action == _buttonActionStepZoomIn || action == _buttonActionStepZoomOut) {
        if (buttonDown) emit stepZoom(action == _buttonActionStepZoomIn ? 1 : -1);
    } else if(action == _buttonActionNextStream || action == _buttonActionPreviousStream) {
        if (buttonDown) emit stepStream(action == _buttonActionNextStream ? 1 : -1);
    } else if(action == _buttonActionNextCamera || action == _buttonActionPreviousCamera) {
        if (buttonDown) emit stepCamera(action == _buttonActionNextCamera ? 1 : -1);
    } else if(action == _buttonActionTriggerCamera) {
        if (buttonDown) emit triggerCamera();
    } else if(action == _buttonActionStartVideoRecord) {
        if (buttonDown) emit startVideoRecord();
    } else if(action == _buttonActionStopVideoRecord) {
        if (buttonDown) emit stopVideoRecord();
    } else if(action == _buttonActionToggleVideoRecord) {
        if (buttonDown) emit toggleVideoRecord();
    } else if(action == _buttonActionGimbalUp) {
        if (buttonDown) _pitchStep(1);
    } else if(action == _buttonActionGimbalDown) {
        if (buttonDown) _pitchStep(-1);
    } else if(action == _buttonActionGimbalLeft) {
        if (buttonDown) _yawStep(-1);
    } else if(action == _buttonActionGimbalRight) {
        if (buttonDown) _yawStep(1);
    } else if(action == _buttonActionGimbalCenter) {
        if (buttonDown) {
            _localPitch = 0.0;
            _localYaw   = 0.0;
            emit gimbalControlValue(0.0, 0.0);
        }
    } else if(action == _buttonActionEmergencyStop) {
      if(buttonDown) emit emergencyStop();
    } else if (action == _buttonActionEngineRunup){
        if (buttonDown){
            //qDebug() << "Engine runup button pressed down";
            emit startEngineRunup(); //button down
        }
        else {
            //qDebug() << "Engine runup button released";
            emit stopEngineRunup(); //button back up
        }
    }
    else {
        qDebug() << "doing generic item.send";
        if (buttonDown && _activeVehicle) {
            for (auto& item : _customMavCommands) {
                if (action == item.name()) {
                    item.send(_activeVehicle);
                    return;
                }
            }
        }
        qCDebug(JoystickLog) << "_buttonAction unknown action:" << action;
    }
}

void Joystick::_pitchStep(int direction)
{
    _localPitch += static_cast<double>(direction);
    //-- Arbitrary range
    if(_localPitch < -90.0) _localPitch = -90.0;
    if(_localPitch >  35.0) _localPitch =  35.0;
    emit gimbalControlValue(_localPitch, _localYaw);
}

void Joystick::_yawStep(int direction)
{
    _localYaw += static_cast<double>(direction);
    if(_localYaw < -180.0) _localYaw = -180.0;
    if(_localYaw >  180.0) _localYaw =  180.0;
    emit gimbalControlValue(_localPitch, _localYaw);
}

bool Joystick::_validAxis(int axis) const
{
    if(axis >= 0 && axis < _axisCount) {
        return true;
    }
    qCWarning(JoystickLog) << "Invalid axis index" << axis;
    return false;
}

bool Joystick::_validButton(int button) const
{
    if(button >= 0 && button < _totalButtonCount)
        return true;
    qCWarning(JoystickLog) << "Invalid button index" << button;
    return false;
}

int Joystick::_findAssignableButtonAction(const QString& action)
{
    for(int i = 0; i < _assignableButtonActions.count(); i++) {
        AssignableButtonAction* p = qobject_cast<AssignableButtonAction*>(_assignableButtonActions[i]);
        if(p->action() == action)
            return i;
    }
    return -1;
}

void Joystick::_buildActionList(Vehicle* activeVehicle)
{
    if(_assignableButtonActions.count())
        _assignableButtonActions.clearAndDeleteContents();
    _availableActionTitles.clear();
    //-- Available Actions
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionNone));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionArm));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionDisarm));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionToggleArm));
    if (activeVehicle) {
        QStringList list = activeVehicle->flightModes();
        foreach(auto mode, list) {
            _assignableButtonActions.append(new AssignableButtonAction(this, mode));
        }
        list = activeVehicle->extraJoystickFlightModes();
        foreach(auto mode, list) {
            _assignableButtonActions.append(new AssignableButtonAction(this, mode));
        }
    }
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionVTOLFixedWing));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionVTOLMultiRotor));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionContinuousZoomIn, true));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionContinuousZoomOut, true));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionStepZoomIn,  true));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionStepZoomOut, true));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionNextStream));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionPreviousStream));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionNextCamera));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionPreviousCamera));
  //  _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionTriggerCamera));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionStartVideoRecord));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionStopVideoRecord));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionToggleVideoRecord));
   // _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionGimbalDown,    true));
   // _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionGimbalUp,      true));
   // _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionGimbalLeft,    true));
   // _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionGimbalRight,   true));
   // _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionGimbalCenter));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionEmergencyStop));
    _assignableButtonActions.append(new AssignableButtonAction(this, _buttonActionEngineRunup));
    for (auto& item : _customMavCommands)
        _assignableButtonActions.append(new AssignableButtonAction(this, item.name()));

    for(int i = 0; i < _assignableButtonActions.count(); i++) {
        AssignableButtonAction* p = qobject_cast<AssignableButtonAction*>(_assignableButtonActions[i]);
        _availableActionTitles << p->action();
    }
    emit assignableActionsChanged();
}
