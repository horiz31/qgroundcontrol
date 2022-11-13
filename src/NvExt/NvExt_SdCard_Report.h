#pragma once
// MESSAGE MAVLINK NvExt SD CARD Report

#define MAVLINK_MSG_NVEXT_SD_CARD_REPORT 248

MAVPACKED(
typedef struct __mavlink_nvext_sd_card_report_t {
    uint16_t            report_type;
    uint8_t             sd_card_detected;
    float               sd_total_capacity;
    float               sd_available_capacity;
}) mavlink_nvext_sd_card_report_t;

#define MAVLINK_MSG_NVEXT_SD_CARD_REPORT_LEN 11
#define MAVLINK_MSG_NVEXT_SD_CARD_REPORT_MIN_LEN 11
#define MAVLINK_MSG_NVEXT_SD_CARD_REPORT_CRC 8

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MSG_NVEXT_SD_CARD_REPORT { \
    248, \
    "V2_EXTENSION", \
    5, \
    {  { "target_network", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(mavlink_v2_extension_t, target_network) }, \
         { "target_system", NULL, MAVLINK_TYPE_UINT8_T, 0, 3, offsetof(mavlink_v2_extension_t, target_system) }, \
         { "target_component", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_v2_extension_t, target_component) }, \
         { "message_type", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_v2_extension_t, message_type) }, \
         { "payload", NULL, MAVLINK_TYPE_UINT8_T, 249, 5, offsetof(mavlink_v2_extension_t, payload) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_NVEXT_SD_CARD_REPORT { \
    "NVEXT_SD_CARD_REPORT", \
    5, \
    {  { "report_type", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(__mavlink_nvext_sd_card_report_t, report_type) }, \
         { "sd_card_detected", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(__mavlink_nvext_sd_card_report_t, sd_card_detected) }, \
         { "sd_total_capacity", NULL, MAVLINK_TYPE_FLOAT, 0, 3, offsetof(__mavlink_nvext_sd_card_report_t, sd_total_capacity) }, \
         { "sd_available_capacity", NULL, MAVLINK_TYPE_FLOAT, 0, 7, offsetof(__mavlink_nvext_sd_card_report_t, sd_available_capacity) }, \
         } \
}
#endif


// MESSAGE MAVLINK_MSG_NVEXT_SD_CARD_REPORT UNPACKING


/**
 * @brief Get field report_type from mavlink_nvext_sd_card_report message
 *
 * @return Report type
 */
static inline uint16_t mavlink_nvext_sd_card_report_get_report_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Get field sd_card_detected from mavlink_nvext_sd_card_report message
 *
 * @return sd_card_detected
 */
static inline uint8_t mavlink_nvext_sd_card_report_get_sd_card_detected(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  2);
}

/**
 * @brief Get field sd_total_capacity from mavlink_nvext_sd_card_report message
 *
 * @return sd_total_capacity
 */
static inline float mavlink_nvext_sd_card_report_get_sd_total_capacity(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  3);
}

/**
 * @brief Get field sd_available_capacity from mavlink_nvext_sd_card_report message
 *
 * @return sd_available_capacity
 */
static inline float mavlink_nvext_sd_card_report_get_sd_available_capacity(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  7);
}

/**
 * @brief Decode a v2_extension message into a struct
 *
 * @param msg The message to decode
 * @param nvext_sd_card_report C-struct to decode the message contents into
 */
static inline void mavlink_nvext_sd_card_report_decode(const mavlink_message_t* msg, mavlink_nvext_sd_card_report_t* nvext_sd_card_report)
{
    nvext_sd_card_report->report_type = mavlink_nvext_sd_card_report_get_report_type(msg);
    nvext_sd_card_report->sd_card_detected = mavlink_nvext_sd_card_report_get_sd_card_detected(msg);
    nvext_sd_card_report->sd_total_capacity = mavlink_nvext_sd_card_report_get_sd_total_capacity(msg);
    nvext_sd_card_report->sd_available_capacity = mavlink_nvext_sd_card_report_get_sd_available_capacity(msg);
}
