
/**
 * @file
 *   @brief New QGC Video Receiver
 *   @author Thomas Lyons
 */

#pragma once

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>
#include "QGCLoggingCategory.h"
#include "SubtitleWriter.h"
#include "VideoReceiver.h"

class Worker : public QThread
{
    Q_OBJECT
public:
    bool needDispatch();
    void dispatch(std::function<void()> t);
    void shutdown();

protected:
    void run() override;

private:
    typedef std::function<void()> Task;
    QWaitCondition _taskQueueUpdate;
    QMutex _taskQueueSync;
    QQueue<Task> _taskQueue;
    bool _shutdown = false;
};

Q_DECLARE_LOGGING_CATEGORY(VideoReceiverLog)

struct _GstElement;
struct _GstBus;
struct _GstMessage;
struct _GstPad;
class GstVideoReceiver : public VideoReceiver
{
    Q_OBJECT

public:
    explicit GstVideoReceiver(QObject* p_parent = nullptr);
    GstVideoReceiver(GstVideoReceiver const&) = delete;
    GstVideoReceiver(GstVideoReceiver&&) = delete;
    GstVideoReceiver& operator=(GstVideoReceiver const&) = delete;
    GstVideoReceiver& operator=(GstVideoReceiver&&) = delete;
    ~GstVideoReceiver(void) noexcept;

public slots:
    virtual void start(QString const& uri, unsigned timeout, int buffer) override;
    virtual void stop(void) override;
    virtual void startDecoding(void* p_sink) override;
    virtual void stopDecoding(void) override;
    virtual void startRecording(QString const& videoDirectory, FILE_FORMAT format) override;
    virtual void stopRecording(void) override;
    virtual void startRemoteStreaming(QString const& streamURL) override;
    virtual void stopRemoteStreaming(void) override;
    virtual void takeScreenshot(QString const& imageFile) override;

private:
    QTimer m_watchdogTimer;
    std::unique_ptr<Worker> mp_slotHandler;
    //TODO consider creating wrapper classes to get ride of a lot of boilerplate
    //source pipeline
    bool m_sourceActive;
    _GstElement* mp_sourcePipeline;
    QString m_uri;
    unsigned m_timeout;
    int m_buffer;
    qint64 m_lastSourceFrameTime;
    bool m_restartingSource;

    //decoding pipeline
    bool m_decoderActive;
    _GstElement* mp_decodingPipeline;
    _GstElement* mp_videoSink;
    qint64 m_lastDecodingFrameTime;
    unsigned long m_decodingPipelineProbeId;
    bool m_restartingDecode;

    //recording pipeline
    bool m_recorderActive;
    _GstElement* mp_recordingPipeline;
    QString m_videoDirectory;
    QString m_currentVideoFile;
    FILE_FORMAT m_videoFormat;
    qint64 m_lastRecordingFrameTime;
    bool m_restartingRecord;
    SubtitleWriter m_subtitleWriter;
    QWaitCondition m_recordShutdownWaitCondition;
    QMutex m_recordShutdownMut;
    QAtomicInteger<bool> m_recordEOS;

    //remote streaming pipeline
    bool m_remoteStreamActive;
    _GstElement* mp_remoteStreamPipeline;
    QString m_remoteStreamURI;
    qint64 m_lastRemoteStreamFrameTime;
    bool m_restartingRemoteStream;

    void _pipelineWatchdog();
    int _onPipelineBusMessageHelper(_GstBus* p_bus,
                                    _GstMessage* p_msg,
                                    std::function<void()> restartFunc,
                                    QString const& pipelineName);
    void _deferredLinkhelper(_GstElement* p_src);
    static void _onNewPad(_GstElement* p_element, _GstPad* p_pad, void* p_data);
    bool _syncPipelineToSourceAndStart(_GstElement* p_dependentPipeline);

    STATUS _startSourcePipeline();
    void _stopSourcePipeline();
    static int _onSourcePipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);

    STATUS _startDecodingPipeline();
    void _stopDecodingPipeline();
    static int _onDecodingPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);

    STATUS _startRecordingPipeline();
    void _stopRecordingPipeline();
    static int _onRecordingPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);
    _GstElement* _createSaveBin();

    STATUS _startRemoteStreamPipeline();
    void _stopRemoteStreamPipeline();
    static int _onRemoteStreamPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);
};

void* createVideoSink(void* widget);

void initializeVideoReceiver(int argc, char* argv[], int debuglevel);
