/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleGimbalFactGroup.h"
#include "Vehicle.h"

#include <QtMath>


const char* VehicleGimbalFactGroup::_modeFactName =                      "mode";
const char* VehicleGimbalFactGroup::_groundCrossingLatFactName =         "groundCrossingLat";
const char* VehicleGimbalFactGroup::_groundCrossingLonFactName =         "groundCrossingLon";
const char* VehicleGimbalFactGroup::_groundCrossingAltFactName =         "groundCrossingAlt";
const char* VehicleGimbalFactGroup::_slantRangeFactName =                "slantRange";
const char* VehicleGimbalFactGroup::_fovFactName =                       "fov";
const char* VehicleGimbalFactGroup::_activeSensorFactName =              "activeSensor";
const char* VehicleGimbalFactGroup::_isRecordingFactName =               "isRecording";
const char* VehicleGimbalFactGroup::_isSnapshotFactName =                "isSnapshot";
const char* VehicleGimbalFactGroup::_cpuTemperatureFactName =            "cpuTemperature";
const char* VehicleGimbalFactGroup::_cameraTemperatureFactName =         "cameraTemperature";
const char* VehicleGimbalFactGroup::_sdCapacityFactName =                "sdCapacity";
const char* VehicleGimbalFactGroup::_sdAvailableFactName =               "sdAvailable";



VehicleGimbalFactGroup::VehicleGimbalFactGroup(QObject* parent)
    : FactGroup(1000, ":/json/Vehicle/NvGimbalFact.json", parent)
    , _modeFact                  (0, _modeFactName,                 FactMetaData::valueTypeString)
    , _groundCrossingLatFact     (0, _groundCrossingLatFactName,    FactMetaData::valueTypeDouble)
    , _groundCrossingLonFact     (0, _groundCrossingLonFactName,    FactMetaData::valueTypeDouble)
    , _groundCrossingAltFact     (0, _groundCrossingAltFactName,    FactMetaData::valueTypeDouble)
    , _slantRangeFact            (0, _slantRangeFactName,           FactMetaData::valueTypeDouble)
    , _fovFact                   (0, _fovFactName,                  FactMetaData::valueTypeDouble)
    , _activeSensorFact          (0, _activeSensorFactName,         FactMetaData::valueTypeUint8)
    , _isRecordingFact           (0, _isRecordingFactName,          FactMetaData::valueTypeUint8)
    , _isSnapshotFact            (0, _isSnapshotFactName,           FactMetaData::valueTypeUint8)
    , _cpuTemperatureFact        (0, _cpuTemperatureFactName,       FactMetaData::valueTypeDouble)
    , _cameraTemperatureFact     (0, _cameraTemperatureFactName,    FactMetaData::valueTypeDouble)
    , _sdCapacityFact            (0, _sdCapacityFactName,           FactMetaData::valueTypeDouble)
    , _sdAvailableFact           (0, _sdAvailableFactName,          FactMetaData::valueTypeDouble)

{
    _addFact(&_modeFact,                _modeFactName);
    _addFact(&_groundCrossingLatFact,   _groundCrossingLatFactName);
    _addFact(&_groundCrossingLonFact,   _groundCrossingLonFactName);
    _addFact(&_groundCrossingAltFact,   _groundCrossingAltFactName);
    _addFact(&_slantRangeFact,          _slantRangeFactName);
    _addFact(&_fovFact,                 _fovFactName);
    _addFact(&_activeSensorFact,        _activeSensorFactName);
    _addFact(&_isRecordingFact,         _isRecordingFactName);
    _addFact(&_isSnapshotFact,          _isSnapshotFactName);
    _addFact(&_cpuTemperatureFact,      _cpuTemperatureFactName);
    _addFact(&_cameraTemperatureFact,   _cameraTemperatureFactName);
    _addFact(&_sdCapacityFact,          _sdCapacityFactName);
    _addFact(&_sdAvailableFact,         _sdAvailableFactName);


    // Start out as not available "--.--"
    _modeFact.setRawValue               ("");
    _groundCrossingLatFact.setRawValue  (qQNaN());
    _groundCrossingLonFact.setRawValue  (qQNaN());
    _groundCrossingAltFact.setRawValue  (qQNaN());
    _slantRangeFact.setRawValue         (qQNaN());
    _fovFact.setRawValue                (qQNaN());
    _activeSensorFact.setRawValue       (qQNaN());
    _isRecordingFact.setRawValue        (qQNaN());
    _isSnapshotFact.setRawValue         (qQNaN());
    _cpuTemperatureFact.setRawValue     (qQNaN());
    _cameraTemperatureFact.setRawValue  (qQNaN());
    _sdCapacityFact.setRawValue         (qQNaN());
    _sdAvailableFact.setRawValue        (qQNaN());

}



