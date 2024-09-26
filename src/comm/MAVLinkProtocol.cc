/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include <inttypes.h>
#include <iostream>

#include <QDebug>
#include <QTime>
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QtEndian>
#include <QMetaType>
#include <QDir>
#include <QFileInfo>

#include "MAVLinkProtocol.h"
#include "UASInterface.h"
#include "UASInterface.h"
#include "UAS.h"
#include "LinkManager.h"
#include "QGCMAVLink.h"
#include "QGC.h"
#include "QGCApplication.h"
#include "QGCLoggingCategory.h"
#include "MultiVehicleManager.h"
#include "SettingsManager.h"
#include <NvExt/NvExt_Sys_Report.h>
#include <NvExt/NvExt_Los_Report.h>
#include <NvExt/NvExt_GndCrs_Report.h>
#include <NvExt/NvExt_SdCard_Report.h>
#include "QGCQGeoCoordinate.h"
#include "mavlink_types.h"

Q_DECLARE_METATYPE(mavlink_message_t)

QGC_LOGGING_CATEGORY(MAVLinkProtocolLog, "MAVLinkProtocolLog")

const char* MAVLinkProtocol::_tempLogFileTemplate   = "FlightDataXXXXXX";   ///< Template for temporary log file
const char* MAVLinkProtocol::_logFileExtension      = "mavlink";            ///< Extension for log files


/**
 * The default constructor will create a new MAVLink object sending heartbeats at
 * the MAVLINK_HEARTBEAT_DEFAULT_RATE to all connected links.
 */
MAVLinkProtocol::MAVLinkProtocol(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , m_enable_version_check(true)
    , _message({})
    , _status({})
    , versionMismatchIgnore(false)
    , systemId(255)
    , _current_version(100)
    , _radio_version_mismatch_count(0)
    , _logSuspendError(false)
    , _logSuspendReplay(false)
    , _vehicleWasArmed(false)
    , _tempLogFile(QString("%2.%3").arg(_tempLogFileTemplate).arg(_logFileExtension))
    , _linkMgr(nullptr)
    , _multiVehicleManager(nullptr)
{
    memset(totalReceiveCounter, 0, sizeof(totalReceiveCounter));
    memset(totalLossCounter,    0, sizeof(totalLossCounter));
    memset(runningLossPercent,  0, sizeof(runningLossPercent));
    memset(firstMessage,        1, sizeof(firstMessage));
    memset(&_status,            0, sizeof(_status));
    memset(&_message,           0, sizeof(_message));
}

MAVLinkProtocol::~MAVLinkProtocol()
{
    storeSettings();
    _closeLogFile();
}

void MAVLinkProtocol::setVersion(unsigned version)
{
    QList<SharedLinkInterfacePtr> sharedLinks = _linkMgr->links();

    for (int i = 0; i < sharedLinks.length(); i++) {
        mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(sharedLinks[i].get()->mavlinkChannel());

        // Set flags for version
        if (version < 200) {
            mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        } else {
            mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        }
    }

    _current_version = version;
}

void MAVLinkProtocol::setToolbox(QGCToolbox *toolbox)
{
   QGCTool::setToolbox(toolbox);

   _linkMgr =               _toolbox->linkManager();
   _multiVehicleManager =   _toolbox->multiVehicleManager();

   qRegisterMetaType<mavlink_message_t>("mavlink_message_t");

   loadSettings();

   // All the *Counter variables are not initialized here, as they should be initialized
   // on a per-link basis before those links are used. @see resetMetadataForLink().

   connect(this, &MAVLinkProtocol::protocolStatusMessage,   _app, &QGCApplication::criticalMessageBoxOnMainThread);
   connect(this, &MAVLinkProtocol::saveTelemetryLog,        _app, &QGCApplication::saveTelemetryLogOnMainThread);
   connect(this, &MAVLinkProtocol::checkTelemetrySavePath,  _app, &QGCApplication::checkTelemetrySavePathOnMainThread);

   connect(_multiVehicleManager, &MultiVehicleManager::vehicleAdded, this, &MAVLinkProtocol::_vehicleCountChanged);
   connect(_multiVehicleManager, &MultiVehicleManager::vehicleRemoved, this, &MAVLinkProtocol::_vehicleCountChanged);

   emit versionCheckChanged(m_enable_version_check);
}

void MAVLinkProtocol::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.beginGroup("QGC_MAVLINK_PROTOCOL");
    enableVersionCheck(settings.value("VERSION_CHECK_ENABLED", m_enable_version_check).toBool());

    // Only set system id if it was valid
    int temp = settings.value("GCS_SYSTEM_ID", systemId).toInt();
    if (temp > 0 && temp < 256)
    {
        systemId = temp;
    }
}

void MAVLinkProtocol::storeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("QGC_MAVLINK_PROTOCOL");
    settings.setValue("VERSION_CHECK_ENABLED", m_enable_version_check);
    settings.setValue("GCS_SYSTEM_ID", systemId);
    // Parameter interface settings
}

void MAVLinkProtocol::resetMetadataForLink(LinkInterface *link)
{
    int channel = link->mavlinkChannel();
    totalReceiveCounter[channel] = 0;
    totalLossCounter[channel]    = 0;
    runningLossPercent[channel]  = 0.0f;
    for(int i = 0; i < 256; i++) {
        firstMessage[channel][i] =  1;
    }
    link->setDecodedFirstMavlinkPacket(false);
}

/**
 * This method parses all outcoming bytes and log a MAVLink packet.
 * @param link The interface to read from
 * @see LinkInterface
 **/

void MAVLinkProtocol::logSentBytes(LinkInterface* link, QByteArray b){

    uint8_t bytes_time[sizeof(quint64)];

    Q_UNUSED(link);
    if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {

        quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);

        qToBigEndian(time,bytes_time);

        b.insert(0,QByteArray((const char*)bytes_time,sizeof(bytes_time)));

        int len = b.count();

        if(_tempLogFile.write(b) != len)
        {
            // If there's an error logging data, raise an alert and stop logging.
            emit protocolStatusMessage(tr("MAVLink Protocol"), tr("MAVLink Logging failed. Could not write to file %1, logging disabled.").arg(_tempLogFile.fileName()));
            _stopLogging();
            _logSuspendError = true;
        }
    }

}

/**
 * This method parses all incoming bytes and constructs a MAVLink packet.
 * It can handle multiple links in parallel, as each link has it's own buffer/
 * parsing state machine.
 * @param link The interface to read from
 * @see LinkInterface
 **/

void MAVLinkProtocol::receiveBytes(LinkInterface* link, QByteArray b)
{
    // Since receiveBytes signals cross threads we can end up with signals in the queue
    // that come through after the link is disconnected. For these we just drop the data
    // since the link is closed.
    SharedLinkInterfacePtr linkPtr = _linkMgr->sharedLinkInterfacePointerForLink(link, true);
    if (!linkPtr) {
        qCDebug(MAVLinkProtocolLog) << "receiveBytes: link gone!" << b.size() << " bytes arrived too late";
        return;
    }

    uint8_t mavlinkChannel = link->mavlinkChannel();

    for (int position = 0; position < b.size(); position++) {
        if (mavlink_parse_char(mavlinkChannel, static_cast<uint8_t>(b[position]), &_message, &_status)) {
            // Got a valid message
            if (!link->decodedFirstMavlinkPacket()) {
                link->setDecodedFirstMavlinkPacket(true);
                mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
                if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
                    qCDebug(MAVLinkProtocolLog) << "Switching outbound to mavlink 2.0 due to incoming mavlink 2.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
                    mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
                    // Set all links to v2
                    setVersion(200);
                }
            }

            //-----------------------------------------------------------------
            // MAVLink Status
            uint8_t lastSeq = lastIndex[_message.sysid][_message.compid];
            uint8_t expectedSeq = lastSeq + 1;
            // Increase receive counter
            totalReceiveCounter[mavlinkChannel]++;
            // Determine what the next expected sequence number is, accounting for
            // never having seen a message for this system/component pair.
            if(firstMessage[_message.sysid][_message.compid]) {
                firstMessage[_message.sysid][_message.compid] = 0;
                lastSeq     = _message.seq;
                expectedSeq = _message.seq;
            }
            // And if we didn't encounter that sequence number, record the error

            if (_message.seq != expectedSeq && _message.sysid != 255)  //NextVision TRIP sends as 255, next vision system reports don't use packet sequence, so don't calc lostMessages for that systemid as it will screw up the telemetry lost display
            {               
                int lostMessages = 0;
                //-- Account for overflow during packet loss
                if(_message.seq < expectedSeq) {
                    lostMessages = (_message.seq + 255) - expectedSeq;
                } else {
                    lostMessages = _message.seq - expectedSeq;
                }
                // Log how many were lost
                totalLossCounter[mavlinkChannel] += static_cast<uint64_t>(lostMessages);
            }


            // And update the last sequence number for this system/component pair
            lastIndex[_message.sysid][_message.compid] = _message.seq;;
            // Calculate new loss ratio
            uint64_t totalSent = totalReceiveCounter[mavlinkChannel] + totalLossCounter[mavlinkChannel];
            float receiveLossPercent = static_cast<float>(static_cast<double>(totalLossCounter[mavlinkChannel]) / static_cast<double>(totalSent));
            receiveLossPercent *= 100.0f;
            receiveLossPercent = (receiveLossPercent * 0.5f) + (runningLossPercent[mavlinkChannel] * 0.5f);
            runningLossPercent[mavlinkChannel] = receiveLossPercent;

            //qDebug() << foo << _message.seq << expectedSeq << lastSeq << totalLossCounter[mavlinkChannel] << totalReceiveCounter[mavlinkChannel] << "(" << _message.sysid << _message.compid << ")";

            //-----------------------------------------------------------------
            // MAVLink forwarding
            bool forwardingEnabled = _app->toolbox()->settingsManager()->appSettings()->forwardMavlink()->rawValue().toBool();
            if (forwardingEnabled) {
                SharedLinkInterfacePtr forwardingLink = _linkMgr->mavlinkForwardingLink();

                if (forwardingLink) {
                    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
                    int len = mavlink_msg_to_send_buffer(buf, &_message);
                    forwardingLink->writeBytesThreadSafe((const char*)buf, len);
                }
            }

            //-----------------------------------------------------------------
            // Log data
            if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {
                uint8_t buf[MAVLINK_MAX_PACKET_LEN+sizeof(quint64)];

                // Write the uint64 time in microseconds in big endian format before the message.
                // This timestamp is saved in UTC time. We are only saving in ms precision because
                // getting more than this isn't possible with Qt without a ton of extra code.
                quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);
                qToBigEndian(time, buf);

                // Then write the message to the buffer
                int len = mavlink_msg_to_send_buffer(buf + sizeof(quint64), &_message);

                // Determine how many bytes were written by adding the timestamp size to the message size
                len += sizeof(quint64);

                // Now write this timestamp/message pair to the log.
                QByteArray b(reinterpret_cast<const char*>(buf), len);
                if(_tempLogFile.write(b) != len)
                {
                    // If there's an error logging data, raise an alert and stop logging.
                    emit protocolStatusMessage(tr("MAVLink Protocol"), tr("MAVLink Logging failed. Could not write to file %1, logging disabled.").arg(_tempLogFile.fileName()));
                    _stopLogging();
                    _logSuspendError = true;
                }

                // Check for the vehicle arming going by. This is used to trigger log save.
                if (!_vehicleWasArmed && _message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                    mavlink_heartbeat_t state;
                    mavlink_msg_heartbeat_decode(&_message, &state);
                    if (state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY) {
                        _vehicleWasArmed = true;
                    }
                }
            }

            if (_message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {                 
                _startLogging();
                mavlink_heartbeat_t heartbeat;
                mavlink_msg_heartbeat_decode(&_message, &heartbeat);
                emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, heartbeat.autopilot, heartbeat.type);
            } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY) {
                _startLogging();
                mavlink_high_latency_t highLatency;
                mavlink_msg_high_latency_decode(&_message, &highLatency);
                // HIGH_LATENCY does not provide autopilot or type information, generic is our safest bet
                emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);
            } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY2) {
                _startLogging();
                mavlink_high_latency2_t highLatency2;
                mavlink_msg_high_latency2_decode(&_message, &highLatency2);
                emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, highLatency2.autopilot, highLatency2.type);
            }

            /* 248 = V2_EXTENSION  message id, used for NextVision gimbal */

            // NextVision
            if (_message.msgid == 248 ) {

               //First, take the message as GND_CRS_REPORT (the shortest one), for extracting the report_type
                mavlink_nvext_gnd_crs_report_t gnd_crs_report;
                mavlink_nvext_gnd_crs_report_decode(&_message,&gnd_crs_report);

                if(gnd_crs_report.report_type == MavExtReport_GndCrs)
                {
                    //qDebug() << "got ground crossing report and decoded: " << gnd_crs_report.gnd_crossing_lat << "," << gnd_crs_report.gnd_crossing_lon;
                    qgcApp()->toolbox()->joystickManager()->cameraManagement()->getAltAtCoord(gnd_crs_report.gnd_crossing_lat,gnd_crs_report.gnd_crossing_lon);

                    if (gnd_crs_report.gnd_crossing_lat != 400)  //400 means that the camera does not think the LOS crosses the ground
                    {
                        //probably smart to save copies of ground crossing lat/lon/
                        //get ground crossing lat
                        emit nvGrounndCrossingLatChanged(gnd_crs_report.gnd_crossing_lat);

                        //get ground crossing lon
                        emit nvGrounndCrossingLonChanged(gnd_crs_report.gnd_crossing_lon);

                        //get ground crossing alt
                        emit nvGrounndCrossingAltChanged(gnd_crs_report.gnd_crossing_alt);

                        //get slant range crossing
                        emit nvSlantRangeChanged(gnd_crs_report.slant_range);
                    }
                    else
                    {
                        //qDebug() << "ground crossing not detected by trip6";
                    }
                }
                /*
                if(gnd_crs_report.report_type == MavExtReport_GndCrs)
                {

                    qDebug() << "got ground crossing report and decoded: " << gnd_crs_report.gnd_crossing_lat << "," << gnd_crs_report.gnd_crossing_lon;

                    qgcApp()->toolbox()->joystickManager()->cameraManagement()->getAltAtCoord(gnd_crs_report.gnd_crossing_lat,gnd_crs_report.gnd_crossing_lon);
                    //get ground crossing lat
                    emit nvGrounndCrossingLatChanged(gnd_crs_report.gnd_crossing_lat);

                    //get ground crossing lon
                    emit nvGrounndCrossingLonChanged(gnd_crs_report.gnd_crossing_lon);

                    //get ground crossing alt
                    emit nvGrounndCrossingAltChanged(gnd_crs_report.gnd_crossing_alt);

                    //get slant range crossing
                    emit nvSlantRangeChanged(gnd_crs_report.slant_range);


                }*/
                else if(gnd_crs_report.report_type == MavExtReport_LOS)
                {
                    float los_upper_left_corner_lat,los_upper_left_corner_lon;
                    float los_upper_right_corner_lat,los_upper_right_corner_lon;
                    float los_lower_right_corner_lat,los_lower_right_corner_lon;
                    float los_lower_left_corner_lat,los_lower_left_corner_lon;
                    uint8_t buf[46];
                    memcpy(&buf, _MAV_PAYLOAD(&_message), std::min<size_t>(_message.len,46));
                    memcpy(&los_upper_left_corner_lat, &buf[14],4);
                    memcpy(&los_upper_left_corner_lon, &buf[18],4);
                    memcpy(&los_upper_right_corner_lat, &buf[22],4);
                    memcpy(&los_upper_right_corner_lon, &buf[26],4);
                    memcpy(&los_lower_right_corner_lat, &buf[30],4);
                    memcpy(&los_lower_right_corner_lon, &buf[34],4);
                    memcpy(&los_lower_left_corner_lat, &buf[38],4);
                    memcpy(&los_lower_left_corner_lon, &buf[42],4);

                    // the line of sight coordinates
                    QList<QGeoCoordinate> coords ;
                    if((double)los_upper_left_corner_lat < 360)
                     coords << QGeoCoordinate((double)los_upper_left_corner_lat,(double)los_upper_left_corner_lon);
                   if((double)los_upper_right_corner_lat < 360)
                       coords << QGeoCoordinate((double)los_upper_right_corner_lat,(double)los_upper_right_corner_lon);
                   if((double)los_lower_right_corner_lat != 400)
                       coords << QGeoCoordinate((double)los_lower_right_corner_lat,(double)los_lower_right_corner_lon);
                   if((double)los_lower_left_corner_lat != 400)
                       coords << QGeoCoordinate((double)los_lower_left_corner_lat,(double)los_lower_left_corner_lon);
                   // emit for updating the vehicle that the camera line of sight was changed
                   if(coords.size() == 4)
                   {
                       // emit for updating the vehicle that the camera line of sight was changed
                       emit lineOfSightChanged(coords);
                   }
                   else
                   {
                       // in case of less than 4 points, report 0 points. (will be fixed in the future)
                       QList<QGeoCoordinate> emptyCoords ;
                       emit lineOfSightChanged(emptyCoords);
                   }

                   mavlink_nvext_los_report_t los_report;
                   mavlink_nvext_los_report_decode(&_message,&los_report);

                   //get azimuth
                   emit nvAzimuthChanged(los_report.los_azimuth);
                }
                else if(gnd_crs_report.report_type == MavExtReport_System)
                {
                    //in system we want to get
                    //cpu temp, camera temp
                    mavlink_nvext_sys_report_t system_report;
                    mavlink_nvext_sys_report_decode(&_message,&system_report);

                    //uint8_t buf[256];
                    // memcpy(&buf, _MAV_PAYLOAD(&_message), _message.len);
                    // int snapshot_val = (int)buf[28];  //bytes 0-6 are header??  so this is byte 34 in the cos
                    //no idea why I'd do it above when I have the helper functions, but this was the nextvision docs

                    emit nvPresentStatusChanged(system_report.camera_ver);

                    int trip_ver = system_report.trip2_ver;
                    uint8_t build_num = (trip_ver & 0x00ff0000) >> 16;
                    uint8_t minor_ver = (trip_ver & 0x0000ff00) >> 8;
                    uint8_t major_ver = (trip_ver & 0x000000ff);

                    QString nvTripVer = QString::number(major_ver) + "." +  QString::number(minor_ver) + "." + QString::number(build_num);
                    //get trip software version
                    emit nvTripVersionChanged(nvTripVer);

                    //get snap shot status
                    emit snapShotStatusChanged(system_report.snapshot_busy);

                    //get current active sensor
                    emit nvSensorChanged(system_report.sensor);

                    //get the current mode
                    emit nvModeChanged(nvExtModeMap.value(system_report.mode));  //map to mode strings

                    //get the current fov
                    emit nvFovChanged(system_report.fov);



                    //get is recording
                    uint8_t recording_status = system_report.recording_status & 0x0f;  //first nibble is channel 0, which is all we care about for now
                    emit nvIsRecordingChanged(recording_status);

                    //get cpu temp
                    emit nvCpuTempChanged(system_report.cpu_temp);

                    //get camera temp
                    emit nvCamTempChanged(system_report.cam_temp);
                }
                else if(gnd_crs_report.report_type == MavExtReport_SDCard)
                {
                    //qDebug() << "got an sd card report";
                    mavlink_nvext_sd_card_report_t sd_report;
                    mavlink_nvext_sd_card_report_decode(&_message,&sd_report);
                    if (sd_report.sd_card_detected != 0)  //no need to report the rest if there is no SD card
                    {
                        //get total capacity and available capacity
                        emit nvSdTotalCapacityChanged(sd_report.sd_total_capacity);
                        emit nvSdAvailableCapacityChanged(sd_report.sd_available_capacity);
                    }
                }
            }

#if 0
            // Given the current state of SiK Radio firmwares there is no way to make the code below work.
            // The ArduPilot implementation of SiK Radio firmware always sends MAVLINK_MSG_ID_RADIO_STATUS as a mavlink 1
            // packet even if the vehicle is sending Mavlink 2.

            // Detect if we are talking to an old radio not supporting v2
            mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
            if (_message.msgid == MAVLINK_MSG_ID_RADIO_STATUS && _radio_version_mismatch_count != -1) {
                if ((mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1)
                && !(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
                    _radio_version_mismatch_count++;
                }
            }

            if (_radio_version_mismatch_count == 5) {
                // Warn the user if the radio continues to send v1 while the link uses v2
                emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Detected radio still using MAVLink v1.0 on a link with MAVLink v2.0 enabled. Please upgrade the radio firmware."));
                // Set to flag warning already shown
                _radio_version_mismatch_count = -1;
                // Flick link back to v1
                qDebug() << "Switching outbound to mavlink 1.0 due to incoming mavlink 1.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
                mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
            }
#endif

            // Update MAVLink status on every 32th packet
            if ((totalReceiveCounter[mavlinkChannel] & 0x1F) == 0) {
                emit mavlinkMessageStatus(_message.sysid, totalSent, totalReceiveCounter[mavlinkChannel], totalLossCounter[mavlinkChannel], receiveLossPercent);
            }

            // The packet is emitted as a whole, as it is only 255 - 261 bytes short
            // kind of inefficient, but no issue for a groundstation pc.
            // It buys as reentrancy for the whole code over all threads
            emit messageReceived(link, _message);

            // Anyone handling the message could close the connection, which deletes the link,
            // so we check if it's expired
            if (1 == linkPtr.use_count()) {
                break;
            }

            // Reset message parsing
            memset(&_status,  0, sizeof(_status));
            memset(&_message, 0, sizeof(_message));
        }
    }
}

/**
 * @return The name of this protocol
 **/
QString MAVLinkProtocol::getName()
{
    return tr("MAVLink protocol");
}

/** @return System id of this application */
int MAVLinkProtocol::getSystemId() const
{
    return systemId;
}

void MAVLinkProtocol::setSystemId(int id)
{
    systemId = id;
}

/** @return Component id of this application */
int MAVLinkProtocol::getComponentId()
{
    return MAV_COMP_ID_MISSIONPLANNER;
}

void MAVLinkProtocol::enableVersionCheck(bool enabled)
{
    m_enable_version_check = enabled;
    emit versionCheckChanged(enabled);
}

void MAVLinkProtocol::_vehicleCountChanged(void)
{
    int count = _multiVehicleManager->vehicles()->count();
    if (count == 0) {
        // Last vehicle is gone, close out logging
        _stopLogging();
        _radio_version_mismatch_count = 0;
    }
}

/// @brief Closes the log file if it is open
bool MAVLinkProtocol::_closeLogFile(void)
{
    if (_tempLogFile.isOpen()) {
        if (_tempLogFile.size() == 0) {
            // Don't save zero byte files
            _tempLogFile.remove();
            return false;
        } else {
            _tempLogFile.flush();
            _tempLogFile.close();
            return true;
        }
    }
    return false;
}

void MAVLinkProtocol::_startLogging(void)
{
    //-- Are we supposed to write logs?
    if (qgcApp()->runningUnitTests()) {
        return;
    }
    AppSettings* appSettings = _app->toolbox()->settingsManager()->appSettings();
    if(appSettings->disableAllPersistence()->rawValue().toBool()) {
        return;
    }
#ifdef __mobile__
    //-- Mobile build don't write to /tmp unless told to do so
    if (!appSettings->telemetrySave()->rawValue().toBool()) {
        return;
    }
#endif
    //-- Log is always written to a temp file. If later the user decides they want
    //   it, it's all there for them.
    if (!_tempLogFile.isOpen()) {
        if (!_logSuspendReplay) {
            if (!_tempLogFile.open()) {
                emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Opening Flight Data file for writing failed. "
                                                                      "Unable to write to %1. Please choose a different file location.").arg(_tempLogFile.fileName()));
                _closeLogFile();
                _logSuspendError = true;
                return;
            }

            qCDebug(MAVLinkProtocolLog) << "Temp log" << _tempLogFile.fileName();
            emit checkTelemetrySavePath();

            _logSuspendError = false;
        }
    }
}

void MAVLinkProtocol::_stopLogging(void)
{
    if (_tempLogFile.isOpen()) {
        if (_closeLogFile()) {
            if ((_vehicleWasArmed || _app->toolbox()->settingsManager()->appSettings()->telemetrySaveNotArmed()->rawValue().toBool()) &&
                _app->toolbox()->settingsManager()->appSettings()->telemetrySave()->rawValue().toBool() &&
                !_app->toolbox()->settingsManager()->appSettings()->disableAllPersistence()->rawValue().toBool()) {
                emit saveTelemetryLog(_tempLogFile.fileName());
            } else {
                QFile::remove(_tempLogFile.fileName());
            }
        }
    }
    _vehicleWasArmed = false;
}

/// @brief Checks the temp directory for log files which may have been left there.
///         This could happen if QGC crashes without the temp log file being saved.
///         Give the user an option to save these orphaned files.
void MAVLinkProtocol::checkForLostLogFiles(void)
{
    QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

    QString filter(QString("*.%1").arg(_logFileExtension));
    QFileInfoList fileInfoList = tempDir.entryInfoList(QStringList(filter), QDir::Files);
    //qDebug() << "Orphaned log file count" << fileInfoList.count();

    for(const QFileInfo& fileInfo: fileInfoList) {
        //qDebug() << "Orphaned log file" << fileInfo.filePath();
        if (fileInfo.size() == 0) {
            // Delete all zero length files
            QFile::remove(fileInfo.filePath());
            continue;
        }
        emit saveTelemetryLog(fileInfo.filePath());
    }
}

void MAVLinkProtocol::suspendLogForReplay(bool suspend)
{
    _logSuspendReplay = suspend;
}

void MAVLinkProtocol::deleteTempLogFiles(void)
{
    QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

    QString filter(QString("*.%1").arg(_logFileExtension));
    QFileInfoList fileInfoList = tempDir.entryInfoList(QStringList(filter), QDir::Files);

    for (const QFileInfo& fileInfo: fileInfoList) {
        QFile::remove(fileInfo.filePath());
    }
}

