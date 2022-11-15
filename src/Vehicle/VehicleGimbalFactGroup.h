/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"

class VehicleGimbalFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleGimbalFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* mode                   READ mode               CONSTANT)
    Q_PROPERTY(Fact* groundCrossingLat      READ groundCrossingLat   CONSTANT)
    Q_PROPERTY(Fact* groundCrossingLon      READ groundCrossingLon   CONSTANT)
    Q_PROPERTY(Fact* groundCrossingAlt      READ groundCrossingAlt   CONSTANT)
    Q_PROPERTY(Fact* slantRange             READ slantRange          CONSTANT)
    Q_PROPERTY(Fact* fov                    READ fov                 CONSTANT)
    Q_PROPERTY(Fact* activeSensor           READ activeSensor        CONSTANT)
    Q_PROPERTY(Fact* isRecording            READ isRecording         CONSTANT)
    Q_PROPERTY(Fact* isSnapshot             READ isSnapshot          CONSTANT)
    Q_PROPERTY(Fact* cpuTemperature	        READ cpuTemperature      CONSTANT)
    Q_PROPERTY(Fact* cameraTemperature      READ cameraTemperature   CONSTANT)
    Q_PROPERTY(Fact* sdCapacity             READ sdCapacity          CONSTANT)
    Q_PROPERTY(Fact* sdAvailable		    READ sdAvailable         CONSTANT)
    Q_PROPERTY(Fact* nvVersion 		        READ nvVersion           CONSTANT)
    Q_PROPERTY(Fact* nvTripVersion 		    READ nvTripVersion           CONSTANT)


    Fact* mode                  () { return &_modeFact; }
    Fact* groundCrossingLat     () { return &_groundCrossingLatFact; }
    Fact* groundCrossingLon     () { return &_groundCrossingLonFact; }
    Fact* groundCrossingAlt     () { return &_groundCrossingAltFact; }
    Fact* slantRange            () { return &_slantRangeFact; }
    Fact* fov                   () { return &_fovFact; }
    Fact* activeSensor          () { return &_activeSensorFact; }
    Fact* isRecording           () { return &_isRecordingFact; }
    Fact* isSnapshot            () { return &_isSnapshotFact; }
    Fact* cpuTemperature        () { return &_cpuTemperatureFact; }
    Fact* cameraTemperature     () { return &_cameraTemperatureFact; }
    Fact* sdCapacity            () { return &_sdCapacityFact; }
    Fact* sdAvailable           () { return &_sdAvailableFact; }
    Fact* nvVersion             () { return &_nvVersionFact; }
    Fact* nvTripVersion         () { return &_nvTripVersionFact; }


    static const char* _modeFactName;
    static const char* _groundCrossingLatFactName;
    static const char* _groundCrossingLonFactName;
    static const char* _groundCrossingAltFactName;
    static const char* _slantRangeFactName;
    static const char* _fovFactName;
    static const char* _activeSensorFactName;
    static const char* _isRecordingFactName;
    static const char* _isSnapshotFactName;
    static const char* _cpuTemperatureFactName;
    static const char* _cameraTemperatureFactName;
    static const char* _sdCapacityFactName;
    static const char* _sdAvailableFactName;
    static const char* _nvVersionFactName;
    static const char* _nvTripVersionFactName;



private:

    Fact        _modeFact;
    Fact        _groundCrossingLatFact;
    Fact        _groundCrossingLonFact;
    Fact        _groundCrossingAltFact;
    Fact        _slantRangeFact;
    Fact        _fovFact;
    Fact        _activeSensorFact;
    Fact        _isRecordingFact;
    Fact        _isSnapshotFact;
    Fact        _cpuTemperatureFact;
    Fact        _cameraTemperatureFact;
    Fact        _sdCapacityFact;
    Fact        _sdAvailableFact;
    Fact        _nvVersionFact;
    Fact        _nvTripVersionFact;


};
