/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef VideoSettings_H
#define VideoSettings_H

#include "SettingsGroup.h"

#ifndef __ENABLE_OTHER_VIDEO_SOURCE_INPUTS__
#define __ENABLE_OTHER_VIDEO_SOURCE_INPUTS__ 0
#endif

class VideoSettings : public SettingsGroup
{
    Q_OBJECT

public:
    VideoSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(videoSource)
    DEFINE_SETTINGFACT(multicastGroup)
    DEFINE_SETTINGFACT(udpPort)
    DEFINE_SETTINGFACT(tcpUrl)
    DEFINE_SETTINGFACT(rtspUrl)
    DEFINE_SETTINGFACT(aspectRatio)
    DEFINE_SETTINGFACT(videoFit)
    DEFINE_SETTINGFACT(gridLines)
    DEFINE_SETTINGFACT(showRecControl)
    DEFINE_SETTINGFACT(recordingFormat)
    DEFINE_SETTINGFACT(maxVideoSize)
    DEFINE_SETTINGFACT(enableStorageLimit)
    DEFINE_SETTINGFACT(rtspTimeout)
    DEFINE_SETTINGFACT(streamEnabled)
    DEFINE_SETTINGFACT(disableWhenDisarmed)
    DEFINE_SETTINGFACT(lowLatencyMode)
    DEFINE_SETTINGFACT(forceVideoDecoder)
    DEFINE_SETTINGFACT(objDetection)
    DEFINE_SETTINGFACT(remoteRecording)
    DEFINE_SETTINGFACT(videoPlaybackMode)    
    DEFINE_SETTINGFACT(fovOverlay)
    DEFINE_SETTINGFACT(targetOverlay)
    DEFINE_SETTINGFACT(pilotViewOnFBW)
    DEFINE_SETTINGFACT(nadirViewOnLand)
    DEFINE_SETTINGFACT(recordOnFlying)
    DEFINE_SETTINGFACT(autoNuc)
    DEFINE_SETTINGFACT(nucPeriod)
    DEFINE_SETTINGFACT(osd)
    DEFINE_SETTINGFACT(enableRemoteStreaming)
    DEFINE_SETTINGFACT(remoteStreamID)
    DEFINE_SETTINGFACT(remoteStreamToken)

    enum VideoDecoderOptions {
        ForceVideoDecoderDefault = 0,
        ForceVideoDecoderSoftware,
        ForceVideoDecoderNVIDIA,
        ForceVideoDecoderVAAPI,
        ForceVideoDecoderDirectX3D,
        ForceVideoDecoderVideoToolbox,
    };
    Q_ENUM(VideoDecoderOptions)

    Q_PROPERTY(bool     streamConfigured        READ streamConfigured       NOTIFY streamConfiguredChanged)
    Q_PROPERTY(bool     remoteStreamingConfigured        READ remoteStreamingConfigured       NOTIFY remoteStreamingConfiguredChanged)
    Q_PROPERTY(QString  rtspVideoSource         READ rtspVideoSource        CONSTANT)
    Q_PROPERTY(QString  udp264VideoSource       READ udp264VideoSource      CONSTANT)
    Q_PROPERTY(QString  udp265VideoSource       READ udp265VideoSource      CONSTANT)
    Q_PROPERTY(QString  udp265MulticastVideoSource       READ udp265MulticastVideoSource      CONSTANT)
    Q_PROPERTY(QString  tcpVideoSource          READ tcpVideoSource         CONSTANT)
    Q_PROPERTY(QString  mpegtsVideoSource       READ mpegtsVideoSource      CONSTANT)
    Q_PROPERTY(QString  disabledVideoSource     READ disabledVideoSource    CONSTANT)

    bool     streamConfigured       ();
    bool     remoteStreamingConfigured ();
    QString  rtspVideoSource        () { return videoSourceRTSP; }
    QString  udp264VideoSource      () { return videoSourceUDPH264; }
    QString  udp265VideoSource      () { return videoSourceUDPH265; }
    QString  udp265MulticastVideoSource      () { return videoSourceMulticastUDPH265; }
    QString  tcpVideoSource         () { return videoSourceTCP; }
    QString  mpegtsVideoSource      () { return videoSourceMPEGTS; }
    QString  disabledVideoSource    () { return videoDisabled; }

    static const char* videoSourceNoVideo;
    static const char* videoDisabled;
    static const char* videoSourceUDPH264;
    static const char* videoSourceUDPH265;
    static const char* videoSourceMulticastUDPH265;
    static const char* videoSourceRTSP;
    static const char* videoSourceTCP;
    static const char* videoSourceMPEGTS;
    static const char* videoSource3DRSolo;
    static const char* videoSourceParrotDiscovery;
    static const char* videoSourceYuneecMantisG;

signals:
    void streamConfiguredChanged             (bool configured);
    void remoteStreamingConfiguredChanged    (bool configured);

private slots:
    void _configChanged             (QVariant value);
    void _remoteStreamingChanged             (QVariant value);

private:
    void _setDefaults               ();

private:
    bool _noVideo = false;

};

#endif
