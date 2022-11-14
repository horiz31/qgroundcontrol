/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include <QLoggingCategory>

#include "LinkInterface.h"
#include "QGCMAVLink.h"
#include "QGC.h"
#include "QGCTemporaryFile.h"
#include "QGCToolbox.h"
#include "QGeoCoordinate"

class LinkManager;
class MultiVehicleManager;
class QGCApplication;

Q_DECLARE_LOGGING_CATEGORY(MAVLinkProtocolLog)

/**
 * @brief MAVLink micro air vehicle protocol reference implementation.
 *
 * MAVLink is a generic communication protocol for micro air vehicles.
 * for more information, please see the official website: https://mavlink.io
 **/
class MAVLinkProtocol : public QGCTool
{
    Q_OBJECT

public:
    MAVLinkProtocol(QGCApplication* app, QGCToolbox* toolbox);
    ~MAVLinkProtocol();

    /** @brief Get the human-friendly name of this protocol */
    QString getName();
    /** @brief Get the system id of this application */
    int getSystemId() const;
    /** @brief Get the component id of this application */
    int getComponentId();

    /** @brief Get protocol version check state */
    bool versionCheckEnabled() const {
        return m_enable_version_check;
    }
    /** @brief Get the protocol version */
    int getVersion() {
        return MAVLINK_VERSION;
    }
    /** @brief Get the currently configured protocol version */
    unsigned getCurrentVersion() const{
        return _current_version;
    }
    /**
     * Reset the counters for all metadata for this link.
     */
    virtual void resetMetadataForLink(LinkInterface *link);

    /// Suspend/Restart logging during replay.
    void suspendLogForReplay(bool suspend);

    /// Set protocol version
    void setVersion(unsigned version);

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);

public slots:
    /** @brief Receive bytes from a communication interface */
    void receiveBytes(LinkInterface* link, QByteArray b);

    /** @brief Log bytes sent from a communication interface */
    void logSentBytes(LinkInterface* link, QByteArray b);

    /** @brief Set the system id of this application */
    void setSystemId(int id);

    /** @brief Enable / disable version check */
    void enableVersionCheck(bool enabled);

    /** @brief Load protocol settings */
    void loadSettings();
    /** @brief Store protocol settings */
    void storeSettings();

    /// @brief Deletes any log files which are in the temp directory
    static void deleteTempLogFiles(void);

    /// Checks for lost log files
    void checkForLostLogFiles(void);

protected:
    bool        m_enable_version_check;                         ///< Enable checking of version match of MAV and QGC
    uint8_t     lastIndex[256][256];                            ///< Store the last received sequence ID for each system/componenet pair
    uint8_t     firstMessage[256][256];                         ///< First message flag
    uint64_t    totalReceiveCounter[MAVLINK_COMM_NUM_BUFFERS];  ///< The total number of successfully received messages
    uint64_t    totalLossCounter[MAVLINK_COMM_NUM_BUFFERS];     ///< Total messages lost during transmission.
    float       runningLossPercent[MAVLINK_COMM_NUM_BUFFERS];   ///< Loss rate

    mavlink_message_t _message;
    mavlink_status_t _status;

    bool        versionMismatchIgnore;
    int         systemId;
    unsigned    _current_version;
    int         _radio_version_mismatch_count;

signals:
    /// Heartbeat received on link
    void vehicleHeartbeatInfo(LinkInterface* link, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);

    /** @brief Message received and directly copied via signal */
    void messageReceived(LinkInterface* link, mavlink_message_t message);
    /** emitting every time the camera line of sight updated */
    void lineOfSightChanged(QList<QGeoCoordinate> test);
    /** @brief Emitted if version check is enabled / disabled */
    void versionCheckChanged(bool enabled);
    /** @brief Emitted if a message from the protocol should reach the user */
    void protocolStatusMessage(const QString& title, const QString& message);
    /** @brief Emitted if a new system ID was set */
    void systemIdChanged(int systemId);

    // NextVision
    /** emitting when the nv version changes, also used to indicate a nv gimbal is present */
    void nvPresentStatusChanged(float version);
    /** emitting every time the snapshot status changes */
    void snapShotStatusChanged(int status);
    /** emitting every time the nextvision mode changes */
    void nvModeChanged(QString mode);
    /** emitting every time the nextvision sensor changes */
    void nvSensorChanged(int sensor);
    /** emitting every time the nextvision fov changes */
    void nvFovChanged(float fov);
     /** emitting every time the nextvision recording status changes */
    void nvIsRecordingChanged(int isRecording);
     /** emitting every time the nextvision cpu temp changes */
    void nvCpuTempChanged(int cpuTemp);
     /** emitting every time the nextvision camera temp status changes */
    void nvCamTempChanged(int camTemp);
     /** emitting every time the nextvision ground crossing lat changes */
    void nvGrounndCrossingLatChanged(float groundCrossingLat);
    /** emitting every time the nextvision ground crossing lon changes */
    void nvGrounndCrossingLonChanged(float groundCrossingLon);
    /** emitting every time the nextvision ground crossing alt changes */
    void nvGrounndCrossingAltChanged(float groundCrossingAlt);
    /** emitting every time the nextvision slant range changes */
    void nvSlantRangeChanged(float slantRange);
    /** emitting every time the nextvision sd total capacity changes */
    void nvSdTotalCapacityChanged(float sdTotalCapacity);
    /** emitting every time the nextvision sd available capacity changes */
    void nvSdAvailableCapacityChanged(float sdAvailableCapacity);

    void mavlinkMessageStatus(int uasId, uint64_t totalSent, uint64_t totalReceived, uint64_t totalLoss, float lossPercent);

    /**
     * @brief Emitted if a new radio status packet received
     *
     * @param rxerrors receive errors
     * @param fixed count of error corrected packets
     * @param rssi local signal strength in dBm
     * @param remrssi remote signal strength in dBm
     * @param txbuf how full the tx buffer is as a percentage
     * @param noise background noise level
     * @param remnoise remote background noise level
     */
    void radioStatusChanged(LinkInterface* link, unsigned rxerrors, unsigned fixed, int rssi, int remrssi,
    unsigned txbuf, unsigned noise, unsigned remnoise);

    /// Emitted when a temporary telemetry log file is ready for saving
    void saveTelemetryLog(QString tempLogfile);

    /// Emitted when a telemetry log is started to save.
    void checkTelemetrySavePath(void);

private slots:
    void _vehicleCountChanged(void);

private:
    bool _closeLogFile(void);
    void _startLogging(void);
    void _stopLogging(void);

    bool _logSuspendError;      ///< true: Logging suspended due to error
    bool _logSuspendReplay;     ///< true: Logging suspended due to replay
    bool _vehicleWasArmed;      ///< true: Vehicle was armed during log sequence  

    QGCTemporaryFile    _tempLogFile;            ///< File to log to
    static const char*  _tempLogFileTemplate;    ///< Template for temporary log file
    static const char*  _logFileExtension;       ///< Extension for log files

    /* mavlink v2 ext system report struct */
    typedef struct
    {
        short           report_type;
        float           roll;
        float           pitch;
        float           fov;
        char            tracker_status;
        char            recording_status;
        char            sensor;
        char            polarity;
        char            mode;
        char            laser_status;
        short           tracker_roi_x;
        short           tracker_roi_y;
        float           single_yaw_cmd;
        char            snapshot_busy;
        float			cpu_temp;
        float 			camera_ver;
        int				trip2_ver;
        unsigned short	bit_status;
        char			status_flags;
        char 			camera_type;
        float			roll_rate;
        float			pitch_rate;
    }MavlinkExtV2SystemReportStruct;

    /* mavlink v2 ext LOS report struct */
    typedef struct
    {
        short           report_type;
        float           los_x;
        float           los_y;
        float           los_z;
        float           los_upper_left_corner_lat;
        float           los_upper_left_corner_lon;
        float           los_upper_right_corner_lat;
        float           los_upper_right_corner_lon;
        float           los_lower_right_corner_lat;
        float           los_lower_right_corner_lon;
        float           los_lower_left_corner_lat;
        float           los_lower_left_corner_lon;
        float           los_elevation;
        float           los_azimuth;
    }MavlinkExtV2LOSReportStruct;

    /* mavlink v2 ext Ground Crossing report struct */
    typedef struct
    {
        short           report_type;
        float           gnd_crossing_lat;
        float           gnd_crossing_lon;
        float           gnd_crossing_alt;
        float           slant_range;
    }MavlinkExtV2GndCrsReportStruct;

    const QMap<int, QString> nvExtModeMap {
        { 0,           "Stow" },
        { 1,           "Pilot" },
        { 2,           "Retract" },
        { 3,           "Retract Lock" },
        { 4,           "Observation" },
        { 5,           "GRR" },
        { 6,           "Hold" },
        { 7,           "PTC" },
        { 8,           "Local Pos" },
        { 9,           "Global Pos" },
        { 10,           "Track" },
        { 11,           "EPP" },
        { 12,           "BIT" },
        { 13,           "Nadir" },
    };

    /*
    int _nvCurrentMode;
    int _nvCurrentSensor;
    int _nvCurrentSnapshot;
    float _nvCurrentFov;
    int _nvCurrentIsRecording;
    float _nvCurrentCpuTemp;
    float _nvCurrentCamTemp;
    float _nvCurrentGroundCrossingLat;
    float _nvCurrentGroundCrossingLon;
    float _nvCurrentGroundCrossingAlt;
    float _nvCurrentSlantRange;
    float _nvSdTotalCapacity;
    float _nvSdAvailableCapacity;
*/

    /* Mavlink Extension Arguments for report command */
    typedef enum
    {
        MavExtReport_System = 0,
        MavExtReport_LOS,
        MavExtReport_GndCrs,
        MavExtReport_Unused1,
        MavExtReport_Unused2,
        MavExtReport_SnapShot,
        MavExtReport_SDCard
    }MavlinkExtSetReportArgs;
    LinkManager*            _linkMgr;
    MultiVehicleManager*    _multiVehicleManager;
};

