#if defined(__mobile__)
/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

/**
 * @file
 *   @brief QGC Video Receiver
 *   @author Gus Grubba <gus@auterion.com>
 */

#pragma once

#include "QGCLoggingCategory.h"
#include <QTimer>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QQueue>
#include <QQuickItem>

#include "VideoReceiver.h"

#include <gst/gst.h>

Q_DECLARE_LOGGING_CATEGORY(VideoReceiverLog)

class Worker : public QThread
{
    Q_OBJECT

public:
    bool needDispatch() {
        return QThread::currentThread() != this;
    }

    void dispatch(std::function<void()> t) {
        QMutexLocker lock(&_taskQueueSync);
        _taskQueue.enqueue(t);
        _taskQueueUpdate.wakeOne();
    }

    void shutdown() {
        if (needDispatch()) {
            dispatch([this](){
                _shutdown = true;
            });
            QThread::wait();
        } else {
            QThread::terminate();
        }
    }

protected:
    void run() {
        while(!_shutdown) {
            _taskQueueSync.lock();

            while (_taskQueue.isEmpty()) {
                _taskQueueUpdate.wait(&_taskQueueSync);
            }

            Task t = _taskQueue.dequeue();

            _taskQueueSync.unlock();

            t();
        }
    }

private:
    typedef std::function<void()> Task;
    QWaitCondition      _taskQueueUpdate;
    QMutex              _taskQueueSync;
    QQueue<Task>        _taskQueue;
    bool                _shutdown = false;
};

class GstVideoReceiver : public VideoReceiver
{
    Q_OBJECT

public:
    explicit GstVideoReceiver(QObject* parent = nullptr);
    ~GstVideoReceiver(void);

public slots:
    virtual void start(const QString& uri, unsigned timeout, int buffer = 0);
    virtual void stop(void);
    virtual void startDecoding(void* sink);
    virtual void stopDecoding(void);
    virtual void startRecording(const QString& videoFile, FILE_FORMAT format);
    virtual void stopRecording(void);
    virtual void takeScreenshot(const QString& imageFile);

protected slots:
    virtual void _watchdog(void);
    virtual void _handleEOS(void);

protected:
    virtual GstElement* _makeSource(const QString& uri);
    virtual GstElement* _makeDecoder(GstCaps* caps = nullptr, GstElement* videoSink = nullptr);
    virtual GstElement* _makeFileSink(const QString& videoFile, FILE_FORMAT format);

    virtual void _onNewSourcePad(GstPad* pad);
    virtual void _onNewDecoderPad(GstPad* pad);
    virtual bool _addDecoder(GstElement* src);
    virtual bool _addVideoSink(GstPad* pad);
    virtual void _noteTeeFrame(void);
    virtual void _noteVideoSinkFrame(void);
    virtual void _noteEndOfStream(void);
    virtual bool _unlinkBranch(GstElement* from);
    virtual void _shutdownDecodingBranch (void);
    virtual void _shutdownRecordingBranch(void);

    bool _needDispatch(void);
    void _dispatchSignal(std::function<void()> emitter);

    static gboolean _onBusMessage(GstBus* bus, GstMessage* message, gpointer user_data);
    static void _onNewPad(GstElement* element, GstPad* pad, gpointer data);
    static void _wrapWithGhostPad(GstElement* element, GstPad* pad, gpointer data);
    static void _linkPad(GstElement* element, GstPad* pad, gpointer data);
    static gboolean _padProbe(GstElement* element, GstPad* pad, gpointer user_data);
    static gboolean _filterParserCaps(GstElement* bin, GstPad* pad, GstElement* element, GstQuery* query, gpointer data);
    static GstPadProbeReturn _teeProbe(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);
    static GstPadProbeReturn _videoSinkProbe(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);
    static GstPadProbeReturn _eosProbe(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);
    static GstPadProbeReturn _keyframeWatch(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);

    bool                _streaming;
    bool                _decoding;
    bool                _recording;
    bool                _removingDecoder;
    bool                _removingRecorder;
    GstElement*         _source;
    GstElement*         _tee;
    GstElement*         _decoderValve;
    GstElement*         _recorderValve;
    GstElement*         _decoder;
    GstElement*         _videoSink;
    GstElement*         _fileSink;
    GstElement*         _pipeline;

    qint64              _lastSourceFrameTime;
    qint64              _lastVideoFrameTime;
    bool                _resetVideoSink;
    gulong              _videoSinkProbeId = 0;

    gulong              _teeProbeId = 0;

    QTimer              _watchdogTimer;

    //-- RTSP UDP reconnect timeout
    uint64_t            _udpReconnect_us;

    QString             _uri;
    unsigned            _timeout;
    int                 _buffer;

    Worker              _slotHandler;
    uint32_t            _signalDepth;

    bool                _endOfStream;

    static const char*  _kFileMux[FILE_FORMAT_MAX - FILE_FORMAT_MIN];
};

void* createVideoSink(void* widget);

void initializeVideoReceiver(int argc, char* argv[], int debuglevel);
#else
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
#include "VideoReceiver.h"

#ifndef __USE_RIDGERUN_INTERPIPE__
#define __USE_RIDGERUN_INTERPIPE__ 1
#endif

class SubtitleWriter;
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

#if !__USE_RIDGERUN_INTERPIPE__
    class _ConnectionQueue;
#endif

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
#if !__USE_RIDGERUN_INTERPIPE__
    std::unique_ptr<_ConnectionQueue> mp_decodeConnectionQueue;
#endif

    //recording pipeline
    bool m_recorderActive;
    _GstElement* mp_recordingPipeline;
    QString m_videoDirectory;
    QString m_currentVideoFile;
    FILE_FORMAT m_videoFormat;
    qint64 m_lastRecordingFrameTime;
    bool m_restartingRecord;
    QWaitCondition m_recordShutdownWaitCondition;
    QMutex m_recordShutdownMut;
    QAtomicInteger<bool> m_recordEOS;
#if !__USE_RIDGERUN_INTERPIPE__
    std::unique_ptr<_ConnectionQueue> mp_recordConnectionQueue;
#endif

    //remote streaming pipeline
    bool m_remoteStreamActive;
    _GstElement* mp_remoteStreamPipeline;
    QString m_remoteStreamURI;
    qint64 m_lastRemoteStreamFrameTime;
    bool m_restartingRemoteStream;
#if !__USE_RIDGERUN_INTERPIPE__
    std::unique_ptr<_ConnectionQueue> mp_remoteStreamConnectionQueue;
#endif

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
    #if !__USE_RIDGERUN_INTERPIPE__
    static int _newSample(_GstElement* p_appsink, void* p_data);
#endif

    STATUS _startDecodingPipeline();
    void _stopDecodingPipeline();
    static int _onDecodingPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);
    #if !__USE_RIDGERUN_INTERPIPE__
    static void _decodeNeedData(_GstElement* p_appsrc, unsigned int size, void* p_udata);
#endif

    STATUS _startRecordingPipeline();
    void _stopRecordingPipeline();
    static int _onRecordingPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);
    _GstElement* _createSaveBin();
    #if !__USE_RIDGERUN_INTERPIPE__
    static void _recordNeedData(_GstElement* p_appsrc, unsigned int size, void* p_udata);
#endif

    STATUS _startRemoteStreamPipeline();
    void _stopRemoteStreamPipeline();
    static int _onRemoteStreamPipelineBusMessage(_GstBus* p_bus, _GstMessage* p_msg, void* p_data);
    #if !__USE_RIDGERUN_INTERPIPE__
    static void _remoteStreamNeedData(_GstElement* p_appsrc, unsigned int size, void* p_udata);
#endif
};

void* createVideoSink(void* widget);

void initializeVideoReceiver(int argc, char* argv[], int debuglevel);
#endif
