#include "GstVideoReceiver.h"
#include <QNetworkInterface>
#include <QUrl>
#include <gst/gst.h>

namespace
{
constexpr const char *np_kFileMux[VideoReceiver::FILE_FORMAT_MAX - VideoReceiver::FILE_FORMAT_MIN]
    = {"matroskamux", "qtmux", "mp4mux"};

}
QGC_LOGGING_CATEGORY(VideoReceiverLog, "VideoReceiverLog")

#define __BREAK_IF_FAIL(__call__) \
    if (!(__call__)) \
    { \
        qCCritical(VideoReceiverLog) << #__call__ " failed " << m_uri; \
        result = STATUS_INVALID_URL; \
        break; \
    } \
    (void) 0

bool Worker::needDispatch()
{
    return QThread::currentThread() != this;
}

void Worker::dispatch(std::function<void()> const t)
{
    QMutexLocker lock(&_taskQueueSync);
    _taskQueue.enqueue(t);
    _taskQueueUpdate.wakeOne();
}

void Worker::shutdown()
{
    if (needDispatch())
    {
        dispatch([this]() { _shutdown = true; });
        QThread::wait();
    }
    else
    {
        QThread::terminate();
    }
}

void Worker::run()
{
    while (!_shutdown)
    {
        _taskQueueSync.lock();
        while (_taskQueue.isEmpty())
        {
            _taskQueueUpdate.wait(&_taskQueueSync);
        }
        auto const t = _taskQueue.dequeue();
        _taskQueueSync.unlock();
        t();
    }
}

GstVideoReceiver::GstVideoReceiver(QObject *const p_parent)
    : VideoReceiver(p_parent)
    , m_watchdogTimer{}
    , mp_slotHandler{std::make_unique<Worker>()}
    , m_sourceActive{false}
    , mp_sourcePipeline{nullptr}
    , m_uri{}
    , m_timeout{0}
    , m_buffer{0}
    , m_lastSourceFrameTime{0}
    , m_sourcePipelineProbeId{0}
    , m_restartingSource{false}
    , m_decoderActive{false}
    , mp_decodingPipeline{nullptr}
    , mp_videoSink{nullptr}
    , m_lastDecodingFrameTime{0}
    , m_decodingPipelineProbeId{0}
    , m_restartingDecode{false}
    , m_recorderActive{false}
    , mp_recordingPipeline{nullptr}
    , m_videoFile{}
    , m_videoFormat{FILE_FORMAT_MAX}
    , m_lastRecordingFrameTime{0}
    , m_recordingPipelineProbeId{0}
    , m_restartingRecord{false}
    , m_remoteStreamActive{false}
    , mp_remoteStreamPipeline{nullptr}
    , m_remoteStreamURI{}
    , m_lastRemoteStreamFrameTime{0}
    , m_remoteStreamPipelineProbeId{0}
    , m_restartingRemoteStream{false}
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::GstVideoReceiver";
    mp_slotHandler->start();
    connect(&m_watchdogTimer, &QTimer::timeout, this, &GstVideoReceiver::_pipelineWatchdog);
    m_watchdogTimer.start(1000);
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::GstVideoReceiver";
}

GstVideoReceiver::~GstVideoReceiver() noexcept
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::~GstVideoReceiver";
    _stopSourcePipeline();
    _stopDecodingPipeline();
    _stopRecordingPipeline();
    _stopRemoteStreamPipeline();
    if (mp_videoSink)
    {
        gst_object_unref(mp_videoSink);
        mp_videoSink = nullptr;
    }
    mp_slotHandler->shutdown();
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::~GstVideoReceiver";
}

void GstVideoReceiver::start(QString const &uri, unsigned const timeout, int const buffer)
{
    if (mp_slotHandler->needDispatch())
    {
        auto const cachedUri = uri;
        mp_slotHandler->dispatch([this, cachedUri, timeout, buffer]()
                                 { start(cachedUri, timeout, buffer); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::start(" << uri << ", " << timeout
                                  << ", " << buffer << ")";
        auto result = STATUS_OK;
        if (uri.trimmed().isEmpty())
        {
            result = STATUS_INVALID_URL;
            qCDebug(VideoReceiverLog) << "Failed because URI is not specified";
        }
        else
        {
            m_uri = uri.trimmed();
            m_timeout = timeout;
            m_buffer = buffer;
            qCDebug(VideoReceiverLog)
                << "Starting uri=" << m_uri << ", timeout=" << m_timeout << ", buffer=" << m_buffer;
            result = _startSourcePipeline();
        }
        emit onStartComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::start(" << uri << ", " << timeout
                                  << ", " << buffer << ")";
    }
}

void GstVideoReceiver::stop()
{
    if (mp_slotHandler->needDispatch())
    {
        mp_slotHandler->dispatch([this]() { stop(); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::stop " << m_uri;
        auto result = STATUS_OK;
        _stopSourcePipeline();
        _stopDecodingPipeline();
        _stopRecordingPipeline();
        _stopRemoteStreamPipeline();
        if (m_decoderActive)
        {
            m_decoderActive = false;
            qCDebug(VideoReceiverLog) << "Decoding stopped at uri=" << m_uri;
            emit decodingChanged(m_decoderActive);
        }
        if (m_recorderActive)
        {
            m_recorderActive = false;
            qCDebug(VideoReceiverLog) << "Recording stopped at uri=" << m_uri;
            emit recordingChanged(m_recorderActive);
        }
        if (m_remoteStreamActive)
        {
            m_remoteStreamActive = false;
            qCDebug(VideoReceiverLog) << "Remote streaming stopped at uri=" << m_uri;
            emit remoteStreamingChanged(m_remoteStreamActive);
        }
        emit onStopComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::stop " << m_uri;
    }
}

void GstVideoReceiver::startDecoding(void *const p_sink)
{
    if (mp_slotHandler->needDispatch())
    {
        auto *const p_videoSink = GST_ELEMENT(p_sink);
        gst_object_ref(p_videoSink);
        mp_slotHandler->dispatch(
            [this, p_videoSink]() mutable
            {
                startDecoding(p_videoSink);
                gst_object_unref(p_videoSink);
            });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::startDecoding " << m_uri;
        auto result = STATUS_OK;
        if (p_sink == nullptr)
        {
            qCCritical(VideoReceiverLog) << "VideoSink is NULL " << m_uri;
            result = STATUS_FAIL;
        }
        else if (m_decoderActive && mp_videoSink && mp_decodingPipeline)
        {
            qCDebug(VideoReceiverLog) << "Already decoding! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        else
        {
            qCDebug(VideoReceiverLog) << "Starting decoding " << m_uri;
            _stopDecodingPipeline();
            auto *const p_videoSink = GST_ELEMENT(p_sink);
            auto *p_pad = gst_element_get_static_pad(p_videoSink, "sink");
            if (p_pad == nullptr)
            {
                qCCritical(VideoReceiverLog) << "Unable to find sink pad of video sink " << m_uri;
                result = STATUS_FAIL;
            }
            else
            {
                gst_object_unref(p_pad);
                p_pad = nullptr;
                if (p_videoSink != mp_videoSink)
                {
                    gst_object_ref(p_videoSink);
                    if (mp_videoSink)
                    {
                        gst_object_unref(mp_videoSink);
                    }
                    mp_videoSink = p_videoSink;
                }
                result = _startDecodingPipeline();
            }
        }
        emit onStartDecodingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::startDecoding " << m_uri;
    }
}

void GstVideoReceiver::stopDecoding()
{
    if (mp_slotHandler->needDispatch())
    {
        mp_slotHandler->dispatch([this]() { stopDecoding(); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::stopDecoding " << m_uri;
        auto result = STATUS_OK;
        if (mp_decodingPipeline && m_decoderActive)
        {
            _stopDecodingPipeline();
            if (m_decoderActive)
            {
                m_decoderActive = false;
                qCDebug(VideoReceiverLog) << "Decoding stopped at uri=" << m_uri;
                emit decodingChanged(m_decoderActive);
            }
        }
        else
        {
            qCDebug(VideoReceiverLog) << "Not decoding! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        emit onStopDecodingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::stopDecoding " << m_uri;
    }
}

void GstVideoReceiver::startRecording(QString const &videoFile, FILE_FORMAT const format)
{
    if (mp_slotHandler->needDispatch())
    {
        auto const cachedVideoFile = videoFile;
        mp_slotHandler->dispatch([this, cachedVideoFile, format]()
                                 { startRecording(cachedVideoFile, format); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::startRecording(" << videoFile << ", "
                                  << format << ") " << m_uri;
        auto result = STATUS_OK;
        if (videoFile.trimmed().isEmpty() || format < FILE_FORMAT_MIN || format >= FILE_FORMAT_MAX)
        {
            qCCritical(VideoReceiverLog) << "Video file is empty or format is invalid! " << m_uri;
            result = STATUS_FAIL;
        }
        else if (m_recorderActive && !m_videoFile.isEmpty() && m_videoFormat != FILE_FORMAT_MAX
                 && mp_recordingPipeline)
        {
            qCDebug(VideoReceiverLog)
                << "Already recording to video file '" << m_videoFile << "'! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        else
        {
            qCDebug(VideoReceiverLog) << "Starting recording to video file '" << videoFile
                                      << "' in format " << format << "." << m_uri;
            _stopRecordingPipeline();
            m_videoFile = videoFile.trimmed();
            m_videoFormat = format;
            result = _startRecordingPipeline();
            if (result != STATUS_OK)
            {
                m_videoFile.clear();
                m_videoFormat = FILE_FORMAT_MAX;
            }
        }
        emit onStartRecordingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::startRecording(" << videoFile << ", "
                                  << format << ") " << m_uri;
    }
}

void GstVideoReceiver::stopRecording()
{
    if (mp_slotHandler->needDispatch())
    {
        mp_slotHandler->dispatch([this]() { stopRecording(); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::stopRecording " << m_uri;
        auto result = STATUS_OK;
        if (mp_recordingPipeline)
        {
            _stopRecordingPipeline();
            m_videoFile.clear();
            m_videoFormat = FILE_FORMAT_MAX;
            if (m_recorderActive)
            {
                m_recorderActive = false;
                qCDebug(VideoReceiverLog) << "Recording stopped at uri=" << m_uri;
                emit recordingChanged(m_recorderActive);
            }
        }
        else
        {
            qCDebug(VideoReceiverLog) << "Not recording! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        emit onStopRecordingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::stopRecording " << m_uri;
    }
}

void GstVideoReceiver::startRemoteStreaming(QString const &streamURL)
{
    if (mp_slotHandler->needDispatch())
    {
        auto const cachedStreamURL = streamURL;
        mp_slotHandler->dispatch([this, cachedStreamURL]()
                                 { startRemoteStreaming(cachedStreamURL); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::startRemoteStreaming(" << streamURL
                                  << ") " << m_uri;
        auto result = STATUS_OK;
        auto const isSrt = streamURL.trimmed().startsWith("srt://", Qt::CaseInsensitive);
        auto const isUdp265 = m_uri.startsWith("udp265://", Qt::CaseInsensitive);
        auto const isUdp264 = m_uri.startsWith("udp://", Qt::CaseInsensitive);
        if (!isSrt)
        {
            qCCritical(VideoReceiverLog)
                << "Only SRT remote streaming URLs are supported " << m_uri;
            result = STATUS_INVALID_URL;
        }
        else if (!isUdp265 && !isUdp264)
        {
            qCCritical(VideoReceiverLog)
                << "Only h265 and h264 sources are supported for remote-streaming " << m_uri;
            result = STATUS_INVALID_URL;
        }
        else if (m_remoteStreamActive
                 && m_remoteStreamURI.toLower() == streamURL.trimmed().toLower()
                 && mp_remoteStreamPipeline)
        {
            qCDebug(VideoReceiverLog)
                << "Already remote streaming to '" << m_remoteStreamURI << "'! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        else
        {
            qCDebug(VideoReceiverLog)
                << "Starting remote streaming to '" << streamURL << "'." << m_uri;
            _stopRemoteStreamPipeline();
            m_remoteStreamURI = streamURL.trimmed();
            result = _startRemoteStreamPipeline();
            if (result != STATUS_OK)
            {
                m_remoteStreamURI.clear();
            }
        }
        emit onStartRemoteStreamingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::startRemoteStreaming(" << streamURL
                                  << ") " << m_uri;
    }
}

void GstVideoReceiver::stopRemoteStreaming()
{
    if (mp_slotHandler->needDispatch())
    {
        mp_slotHandler->dispatch([this]() { stopRemoteStreaming(); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::stopRemoteStreaming " << m_uri;
        auto result = STATUS_OK;
        if (mp_remoteStreamPipeline)
        {
            _stopRemoteStreamPipeline();
            m_remoteStreamURI.clear();
            if (m_remoteStreamActive)
            {
                m_remoteStreamActive = false;
                qCDebug(VideoReceiverLog) << "Remote streaming stopped at uri=" << m_uri;
                emit remoteStreamingChanged(m_remoteStreamActive);
            }
        }
        else
        {
            qCDebug(VideoReceiverLog) << "Not remote streaming! " << m_uri;
            result = STATUS_INVALID_STATE;
        }
        emit onStopRemoteStreamingComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::stopRemoteStreaming " << m_uri;
    }
}

void GstVideoReceiver::takeScreenshot(QString const &imageFile)
{
    if (mp_slotHandler->needDispatch())
    {
        auto const cachedImageFile = imageFile;
        mp_slotHandler->dispatch([this, cachedImageFile]() { takeScreenshot(cachedImageFile); });
    }
    else
    {
        qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::takeScreenshot(" << imageFile << ") "
                                  << m_uri;
        auto result = STATUS_NOT_IMPLEMENTED;
        //TODO not implemented
        emit onTakeScreenshotComplete(result);
        qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::takeScreenshot(" << imageFile << ") "
                                  << m_uri;
    }
}

void GstVideoReceiver::_pipelineWatchdog()
{
    mp_slotHandler->dispatch(
        [this]()
        {
            bool stopped = false;
            auto const now = QDateTime::currentSecsSinceEpoch();
            if ((mp_sourcePipeline && m_sourceActive) || m_restartingSource)
            {
                //auto savedLastFrameTime = m_lastSourceFrameTime;
                if (m_lastSourceFrameTime == 0)
                {
                    m_lastSourceFrameTime = now;
                }
                auto const timeSinceLastFrame = now - m_lastSourceFrameTime;
                if (timeSinceLastFrame > m_timeout)
                {
                    qCCritical(VideoReceiverLog) << "Source stream timeout, no frames for "
                                                 << timeSinceLastFrame << "" << m_uri;

#if 1
                    emit timeout();
                    //if source fails, just stop the whole receiver and let video manager restart it
                    stopped = true;
                    stop();
#else
                    if (!m_restartingSource)
                    {
                        _stopSourcePipeline();
                    }
                    if (_startSourcePipeline() == STATUS_OK)
                    {
                        m_restartingSource = false;
                    }
                    else
                    {
                        m_lastSourceFrameTime = savedLastFrameTime;
                        m_restartingSource = true;
                    }
#endif
                }
            }
            if (!stopped && ((mp_decodingPipeline && m_decoderActive) || m_restartingDecode))
            {
                auto savedLastFrameTime = m_lastDecodingFrameTime;
                if (m_lastDecodingFrameTime == 0)
                {
                    m_lastDecodingFrameTime = now;
                }
                auto const timeSinceLastFrame = now - m_lastDecodingFrameTime;
                if (timeSinceLastFrame > m_timeout * 3)
                {
                    qCCritical(VideoReceiverLog) << "Decoding stream timeout, no frames for "
                                                 << timeSinceLastFrame << "" << m_uri;
#if 0
                    emit timeout();
                    stopped = true;
                    stop();
#else
                    //attempt to restart the decoding pipeline without disturbing the other pipelines
                    if (!m_restartingDecode)
                    {
                        _stopDecodingPipeline();
                    }
                    if (_startDecodingPipeline() == STATUS_OK)
                    {
                        m_restartingDecode = false;
                    }
                    else
                    {
                        m_lastDecodingFrameTime = savedLastFrameTime;
                        m_restartingDecode = true;
                    }
#endif
                }
            }
            if (!stopped && ((mp_recordingPipeline && m_recorderActive) || m_restartingRecord))
            {
                //auto savedLastFrameTime = m_lastRecordingFrameTime;
                if (m_lastRecordingFrameTime == 0)
                {
                    m_lastRecordingFrameTime = now;
                }
                auto const timeSinceLastFrame = now - m_lastRecordingFrameTime;
                if (timeSinceLastFrame > m_timeout * 3)
                {
                    qCCritical(VideoReceiverLog) << "Recording stream timeout, no frames for "
                                                 << timeSinceLastFrame << "" << m_uri;
#if 1
                    emit timeout();
                    //if recording fails, just stop the whole receiver and let video manager restart it
                    stopped = true;
                    stop();
#else
                    if (!m_restartingRemoteStream)
                    {
                        _stopRecordingPipeline();
                    }
                    if (_startRecordingPipeline() == STATUS_OK)
                    {
                        m_restartingRecord = false;
                    }
                    else
                    {
                        m_lastRecordingFrameTime = savedLastFrameTime;
                        m_restartingRecord = true;
                    }
#endif
                }
            }
            if (!stopped
                && ((mp_remoteStreamPipeline && m_remoteStreamActive) || m_restartingRemoteStream))
            {
                auto savedLastFrameTime = m_lastRemoteStreamFrameTime;
                if (m_lastRemoteStreamFrameTime == 0)
                {
                    m_lastRemoteStreamFrameTime = now;
                }
                auto const timeSinceLastFrame = now - m_lastRemoteStreamFrameTime;
                if (timeSinceLastFrame > m_timeout * 3)
                {
                    qCCritical(VideoReceiverLog)
                        << "Remote Streaming stream timeout, no frames for " << timeSinceLastFrame
                        << "" << m_uri;
#if 0
                    emit timeout();
                    stopped = true;
                    stop();
#else
                    //attempt to restart the remote streaming pipeline without disturbing the other pipelines
                    if (!m_restartingRemoteStream)
                    {
                        _stopRemoteStreamPipeline();
                    }
                    if (_startRemoteStreamPipeline() == STATUS_OK)
                    {
                        m_restartingRemoteStream = false;
                    }
                    else
                    {
                        m_lastRemoteStreamFrameTime = savedLastFrameTime;
                        m_restartingRemoteStream = true;
                    }
#endif
                }
            }
        });
}

gboolean GstVideoReceiver::_onPipelineBusMessageHelper(GstBus *,
                                                       GstMessage *const p_msg,
                                                       std::function<void()> const restartFunc,
                                                       QString const &pipelineName)
{
    gboolean result = true;
#if 0
    Q_ASSERT(p_msg != nullptr);
    switch (GST_MESSAGE_TYPE(p_msg))
    {
    case GST_MESSAGE_ERROR:
    {
        gchar *p_debug = nullptr;
        GError *p_error = nullptr;
        gst_message_parse_error(p_msg, &p_error, &p_debug);
        if (p_debug != nullptr)
        {
            g_free(p_debug);
            p_debug = nullptr;
        }
        if (p_error != nullptr)
        {
            qCCritical(VideoReceiverLog) << "GStreamer error:" << p_error->message;
            g_error_free(p_error);
            p_error = nullptr;
        }
#if 1
        qCDebug(VideoReceiverLog) << "Stopping because of error on " << pipelineName << " pipeline";
        restartFunc();
#else
        mp_slotHandler->dispatch(
            [restartFunc, pipelineName]()
            {
                qCDebug(VideoReceiverLog)
                    << "Stopping because of error on " << pipelineName << " pipeline";
                restartFunc();
            });
#endif
        break;
    }
    case GST_MESSAGE_EOS:
    {
#if 1
        qCDebug(VideoReceiverLog) << "Received EOS on " << pipelineName << " pipeline";
        restartFunc();
#else
        mp_slotHandler->dispatch(
            [restartFunc, pipelineName]()
            {
                qCDebug(VideoReceiverLog) << "Received EOS on " << pipelineName << " pipeline";
                restartFunc();
            });
#endif
        break;
    }
GST_MESSAGE_ELEMENT:
    {
        auto const *const p_struct = gst_message_get_structure(p_msg);
        if (gst_structure_has_name(p_struct, "GstBinForwarded"))
        {
            GstMessage *p_forwardMsg = nullptr;
            gst_structure_get(p_struct, "message", GST_TYPE_MESSAGE, &p_forwardMsg, nullptr);
            if (p_forwardMsg != nullptr)
            {
                if (GST_MESSAGE_TYPE(p_forwardMsg) == GST_MESSAGE_EOS)
                {
#if 1
                    qCDebug(VideoReceiverLog)
                        << "Received branch EOS on " << pipelineName << " pipeline";
                    restartFunc();
#else
                    mp_slotHandler->dispatch(
                        [restartFunc, pipelineName]()
                        {
                            qCDebug(VideoReceiverLog)
                                << "Received branch EOS on " << pipelineName << " pipeline";
                            restartFunc();
                        });
#endif
                }
                gst_message_unref(p_forwardMsg);
                p_forwardMsg = nullptr;
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }
#endif
    return result;
}

void GstVideoReceiver::_deferredLinkhelper(GstElement *const p_src)
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_deferredLinkhelper " << m_uri;
    GstPad *p_srcPad = nullptr;
    GstIterator *p_it = gst_element_iterate_src_pads(p_src);
    if (p_it != nullptr)
    {
        GValue vpad = G_VALUE_INIT;
        if (gst_iterator_next(p_it, &vpad) == GST_ITERATOR_OK)
        {
            p_srcPad = GST_PAD(g_value_get_object(&vpad));
            gst_object_ref(p_srcPad);
            g_value_reset(&vpad);
        }
        gst_iterator_free(p_it);
        p_it = nullptr;
    }
    if (p_srcPad == nullptr)
    {
        qCDebug(VideoReceiverLog) << "deferred _onNewPad to pad-added signal " << m_uri;
        g_signal_connect(p_src, "pad-added", G_CALLBACK(_onNewPad), this);
    }
    else
    {
        qCDebug(VideoReceiverLog) << "direct to _onNewPad";
        _onNewPad(p_src, p_srcPad, this);
        gst_object_unref(p_srcPad);
        p_srcPad = nullptr;
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_deferredLinkhelper " << m_uri;
}

void GstVideoReceiver::_onNewPad(GstElement *const p_element,
                                 GstPad *const p_pad,
                                 gpointer const p_data)
{
    auto *const p_self = static_cast<GstVideoReceiver *>(p_data);
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_onNewPad " << p_self->m_uri;

    gchar *p_name = gst_element_get_name(p_element);
    if (p_name == nullptr)
    {
        qCDebug(VideoReceiverLog) << "Element has no name! " << p_self->m_uri;
    }
    else if (strcmp(p_name, "mp_sourcePipeline->parsebin") == 0)
    {
        //called from _startSourcePipeline
        qCDebug(VideoReceiverLog) << "GstVideoReceiver::_onNewPad with mp_sourceParser "
                                  << p_self->m_uri;
        if (p_self->mp_sourcePipeline == nullptr)
        {
            qCCritical(VideoReceiverLog) << "p_self->mp_sourcePipeline==nullptr " << p_self->m_uri;
        }
        else
        {
            auto *p_sourceQueue3 = gst_bin_get_by_name(GST_BIN(p_self->mp_sourcePipeline),
                                                       "mp_sourcePipeline->mp_sourceQueue3");
            if (p_sourceQueue3 == nullptr)
            {
                qCCritical(VideoReceiverLog) << "p_sourceQueue3==nullptr " << p_self->m_uri;
            }
            else
            {
                auto *p_srcSink1 = gst_bin_get_by_name(GST_BIN(p_self->mp_sourcePipeline),
                                                       "mp_sourcePipeline->mp_sourceSink1");
                if (p_srcSink1 == nullptr)
                {
                    qCCritical(VideoReceiverLog) << "p_srcSink1==nullptr " << p_self->m_uri;
                }
                else
                {
                    if (!gst_element_link(p_element, p_sourceQueue3))
                    {
                        qCCritical(VideoReceiverLog) << "gst_element_link(p_self->mp_sourceParser, "
                                                        "p_self->mp_sourceQueue3) failed "
                                                     << p_self->m_uri;
                    }
                    if (!gst_element_link(p_sourceQueue3, p_srcSink1))
                    {
                        qCCritical(VideoReceiverLog) << "gst_element_link(p_self->mp_sourceQueue3, "
                                                        "p_self->mp_srcSink1) failed "
                                                     << p_self->m_uri;
                    }
                    gst_object_unref(p_srcSink1);
                    p_srcSink1 = nullptr;
                }
                gst_object_unref(p_sourceQueue3);
                p_sourceQueue3 = nullptr;
            }
        }
    }
    else if (strcmp(p_name, "mp_decodingPipeline->decodebin3") == 0)
    {
        //called from _startDecodingPipeline
        qCDebug(VideoReceiverLog) << "GstVideoReceiver::_onNewPad with mp_decoder "
                                  << p_self->m_uri;
        if (!gst_element_link(p_element, p_self->mp_videoSink))
        {
            qCCritical(VideoReceiverLog)
                << "gst_element_link(p_self->mp_decoder, p_self->mp_videoSink) failed "
                << p_self->m_uri;
        }
    }
    else
    {
        qCDebug(VideoReceiverLog) << "Unexpected call! " << p_self->m_uri;
    }
    g_free(p_name);
    p_name = nullptr;
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_onNewPad " << p_self->m_uri;
}

bool GstVideoReceiver::_syncPipelineToSourceAndStart(GstElement *const p_dependentPipeline)
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_syncPipelineToSourceAndStart " << m_uri;
    auto running = false;
//TODO investigate if there is any benefit to syncing the pipeline clocks
#if 1
    running = gst_element_set_state(p_dependentPipeline, GST_STATE_PLAYING)
              != GST_STATE_CHANGE_FAILURE;
#else
    auto *p_sourceClock = gst_pipeline_get_clock(GST_PIPELINE(mp_sourcePipeline));
    if (p_sourceClock == nullptr)
    {
        qCCritical(VideoReceiverLog) << "Unable to get source pipline clock";
    }
    else
    {
        gst_pipeline_use_clock(GST_PIPELINE(p_dependentPipeline), p_sourceClock);
        gst_object_unref(p_sourceClock);
        p_sourceClock = nullptr;
        auto const sourceTime = gst_element_get_base_time(mp_sourcePipeline);
        gst_element_set_base_time(p_dependentPipeline, sourceTime);
        //start the pipeline
        running = gst_element_set_state(p_dependentPipeline, GST_STATE_PLAYING)
                  != GST_STATE_CHANGE_FAILURE;
    }
#endif
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_syncPipelineToSourceAndStart " << m_uri;
    return running;
}

VideoReceiver::STATUS GstVideoReceiver::_startSourcePipeline()
{
    // It was necessary to split this into two branches because mpegtsmux on the remote streaming pipeline
    // is very picky about what it will accept. We can't use the same parser for it as we can for the
    // recording and decoding pipelines
    //
    //                                  this branch feeds to mp_decodingPipeline and mp_recordingPipeline
    //                                  +-->p_sourceQueue1-->mp_sourceParser-->mp_sourceQueue3-->mp_sourceSink1
    //                                  |
    // mp_sourceElement(*)-->mp_sourceTee
    //                                  |
    //                                  +-->mp_sourceQueue2-->mp_sourceSink2
    //                                  this branch feeds to mp_remoteStreamPipeline
    //
    // (*) - mp_sourceElement may be followed by a tsdemux and/or an rtpjitterbuffer

    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_startSourcePipeline " << m_uri;
    auto result = STATUS_OK;
    if (mp_sourcePipeline)
    {
        qCDebug(VideoReceiverLog) << "Source pipeline already started " << m_uri;
        result = STATUS_INVALID_STATE;
    }
    else if (m_uri.isEmpty())
    {
        qCCritical(VideoReceiverLog) << "m_uri cannot be empty " << m_uri;
        result = STATUS_INVALID_URL;
    }
    else
    {
        auto const isTaisync = m_uri.startsWith("tsusb://", Qt::CaseInsensitive);
        auto const isUdp264 = m_uri.startsWith("udp://", Qt::CaseInsensitive);
        auto const isRtsp = m_uri.startsWith("rtsp://", Qt::CaseInsensitive);
        auto const isUdp265 = m_uri.startsWith("udp265://", Qt::CaseInsensitive);
        auto const isTcpMPEGTS = m_uri.startsWith("tcp://", Qt::CaseInsensitive);
        auto const isUdpMPEGTS = m_uri.startsWith("mpegts://", Qt::CaseInsensitive);
        GstElement *p_source = nullptr;
        auto sourceAdded = false;
        GstElement *p_tsdemux = nullptr;
        auto tsdemuxAdded = false;
        GstElement *p_buffer = nullptr;
        auto bufferAdded = false;
        GstElement *p_sourceTee = nullptr;
        auto sourceTeeAdded = false;
        GstElement *p_sourceQueue1 = nullptr;
        auto sourceQueue1Added = false;
        GstElement *p_sourceQueue2 = nullptr;
        auto sourceQueue2Added = false;
        GstElement *p_parser = nullptr;
        auto parserAdded = false;
        GstElement *p_sourceQueue3 = nullptr;
        auto sourceQueue3Added = false;
        GstElement *p_sourceSink1 = nullptr;
        auto sourceSink1Added = false;
        GstElement *p_sourceSink2 = nullptr;
        auto sourceSink2Added = false;
        auto running = false;
        m_lastSourceFrameTime = 0;
        do
        {
            {
                __BREAK_IF_FAIL(mp_sourcePipeline = gst_pipeline_new("mp_sourcePipeline"));
                g_object_set(mp_sourcePipeline, "message-forward", true, nullptr);
            }
            {
                //create the source element
                QUrl const url(m_uri);
                if (isTcpMPEGTS)
                {
                    __BREAK_IF_FAIL(
                        p_source = gst_element_factory_make("tcpclientsrc",
                                                            "mp_sourcePipeline->sourceElement"));
                    g_object_set(static_cast<gpointer>(p_source),
                                 "host",
                                 qPrintable(url.host()),
                                 "port",
                                 url.port(),
                                 nullptr);
                }
                else if (isRtsp)
                {
                    __BREAK_IF_FAIL(
                        p_source = gst_element_factory_make("rtspsrc",
                                                            "mp_sourcePipeline->sourceElement"));
                    g_object_set(static_cast<gpointer>(p_source),
                                 "location",
                                 qPrintable(m_uri),
                                 "latency",
                                 17,
                                 "udp-reconnect",
                                 1,
                                 "timeout",
                                 5000000,
                                 nullptr);
                }
                else if (isUdp264 || isUdp265 || isUdpMPEGTS || isTaisync)
                {
                    __BREAK_IF_FAIL(
                        p_source = gst_element_factory_make("udpsrc",
                                                            "mp_sourcePipeline->sourceElement"));
                    g_object_set(static_cast<gpointer>(p_source),
                                 "uri",
                                 QString("udp://%1:%2")
                                     .arg(qPrintable(url.host()), QString::number(url.port()))
                                     .toUtf8()
                                     .data(),
                                 nullptr);
                    {
                        GstCaps *p_caps = nullptr;
                        if (isUdp264)
                        {
                            __BREAK_IF_FAIL(p_caps = gst_caps_from_string(
                                                "application/x-rtp, media=(string)video, "
                                                "clock-rate=(int)90000, "
                                                "encoding-name=(string)H264"));
                        }
                        else if (isUdp265)
                        {
                            __BREAK_IF_FAIL(p_caps = gst_caps_from_string(
                                                "application/x-rtp, media=(string)video, "
                                                "clock-rate=(int)90000, "
                                                "encoding-name=(string)H265"));
                        }
                        if (p_caps != nullptr)
                        {
                            g_object_set(static_cast<gpointer>(p_source), "caps", p_caps, nullptr);
                            gst_caps_unref(p_caps);
                            p_caps = nullptr;
                        }
                    }
                    // Add multicast support caps, looping through all the available interfaces
                    QString interfaceList = "";
                    auto counter = 0;
                    foreach (QNetworkInterface iface, QNetworkInterface::allInterfaces())
                    {
                        if (iface.flags().testFlag(QNetworkInterface::IsUp)
                            && iface.flags().testFlag(QNetworkInterface::CanMulticast)
                            && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
                        {
                            if (++counter > 1)
                            {
                                interfaceList.append(",");
                            }
                            interfaceList.append(iface.humanReadableName());
                        }
                    }
                    g_object_set(static_cast<gpointer>(p_source),
                                 "auto-multicast",
                                 true,
                                 "loop",
                                 false,
                                 "multicast-iface",
                                 qPrintable(interfaceList),
                                 nullptr);
                }
                else
                {
                    result = STATUS_INVALID_URL;
                    qCDebug(VideoReceiverLog) << "URI is not recognized " << m_uri;
                    break;
                }
                __BREAK_IF_FAIL(sourceAdded = gst_bin_add(GST_BIN(mp_sourcePipeline), p_source));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_source));
                //mp_sourceElement = p_source;
                //check if we need a demuxer after the source
                if (isTcpMPEGTS || isUdpMPEGTS)
                {
                    //create the demuxer element
                    __BREAK_IF_FAIL(
                        p_tsdemux = gst_element_factory_make("tsdemux",
                                                             "mp_sourcePipeline->tsdemux"));
                    __BREAK_IF_FAIL(
                        tsdemuxAdded = gst_bin_add(GST_BIN(mp_sourcePipeline), p_tsdemux));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_tsdemux));
                    __BREAK_IF_FAIL(gst_element_link(p_source, p_tsdemux));
                    //source becomes demuxer
                    p_source = p_tsdemux;
                    p_tsdemux = nullptr;
                }
                //check if we need a jitter buffer
                auto probeResult = 0;
                gst_element_foreach_src_pad(
                    p_source,
                    [](GstElement *, GstPad *p_pad, gpointer p_user_data) -> gboolean
                    {
                        auto *p_probeResult = (int *) p_user_data;
                        *p_probeResult |= 1;
                        auto *p_filter = gst_caps_from_string("application/x-rtp");
                        if (p_filter != nullptr)
                        {
                            auto *p_caps = gst_pad_query_caps(p_pad, nullptr);
                            if (p_caps != nullptr)
                            {
                                if (!gst_caps_is_any(p_caps)
                                    && gst_caps_can_intersect(p_caps, p_filter))
                                {
                                    *p_probeResult |= 2;
                                }
                                gst_caps_unref(p_caps);
                                p_caps = nullptr;
                            }
                            gst_caps_unref(p_filter);
                            p_filter = nullptr;
                        }
                        return true;
                    },
                    &probeResult);
                if (!(probeResult & 1))
                {
                    qCCritical(VideoReceiverLog) << "No source pad found " << m_uri;
                    result = STATUS_FAIL;
                    break;
                }
                if (probeResult & 2 && m_buffer >= 0)
                {
                    //create the jitter buffer element
                    __BREAK_IF_FAIL(
                        p_buffer = gst_element_factory_make("rtpjitterbuffer",
                                                            "mp_sourcePipeline->rtpjitterbuffer"));
                    __BREAK_IF_FAIL(bufferAdded = gst_bin_add(GST_BIN(mp_sourcePipeline), p_buffer));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_buffer));
                    __BREAK_IF_FAIL(gst_element_link(p_source, p_buffer));
                    //source becomes buffer
                    p_source = p_buffer;
                    p_buffer = nullptr;
                }
            }
            {
                //create the tee element
                __BREAK_IF_FAIL(
                    p_sourceTee = gst_element_factory_make("tee", "mp_sourcePipeline->tee"));
                __BREAK_IF_FAIL(
                    sourceTeeAdded = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceTee));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceTee));
                //mp_sourceTee = p_sourceTee;
                //add a probe to the tee to keep track of when it gets a frame
                GstPad *p_sinkPad = nullptr;
                __BREAK_IF_FAIL(p_sinkPad = gst_element_get_static_pad(p_sourceTee, "sink"));
                m_sourcePipelineProbeId = gst_pad_add_probe(
                    p_sinkPad,
                    GST_PAD_PROBE_TYPE_BUFFER,
                    [](GstPad *const,
                       GstPadProbeInfo *const,
                       gpointer const p_userData) -> GstPadProbeReturn
                    {
                        if (p_userData != nullptr)
                        {
                            //record the frame time so we know if we've lost the feed (see watchdog)
                            auto *const p_self = static_cast<GstVideoReceiver *>(p_userData);
                            p_self->m_lastSourceFrameTime = QDateTime::currentSecsSinceEpoch();
                            if (!p_self->m_sourceActive)
                            {
                                p_self->m_sourceActive = true;
                                qCDebug(VideoReceiverLog) << "Streaming started " << p_self->m_uri;
                                emit p_self->streamingChanged(p_self->m_sourceActive);
                            }
                        }
                        return GST_PAD_PROBE_OK;
                    },
                    this,
                    nullptr);
                gst_object_unref(p_sinkPad);
                p_sinkPad = nullptr;
            }
            {
                //create the queue element for recording and decoding
                __BREAK_IF_FAIL(
                    p_sourceQueue1
                    = gst_element_factory_make("queue", "mp_sourcePipeline->mp_sourceQueue1"));
                g_object_set(p_sourceQueue1,
                             "max-size-buffers",
                             200 * 3,
                             nullptr); // 3 times the default value
                g_object_set(p_sourceQueue1,
                             "max-size-bytes",
                             10485760 * 3,
                             nullptr); // 30MB, 3 times the default value
                g_object_set(p_sourceQueue1,
                             "max-size-time",
                             guint64(1000000000) * 3,
                             nullptr); // 3 seconds, 3 times the default value
                g_object_set(p_sourceQueue1, "silent", true, nullptr);
                __BREAK_IF_FAIL(
                    sourceQueue1Added = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceQueue1));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceQueue1));
            }
            {
                //create the queue element for remote streaming
                __BREAK_IF_FAIL(
                    p_sourceQueue2
                    = gst_element_factory_make("queue", "mp_sourcePipeline->mp_sourceQueue2"));
                g_object_set(p_sourceQueue2, "silent", true, nullptr);
                __BREAK_IF_FAIL(
                    sourceQueue2Added = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceQueue2));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceQueue2));
            }
            {
                //create the parser element
                __BREAK_IF_FAIL(
                    p_parser = gst_element_factory_make("parsebin", "mp_sourcePipeline->parsebin"));
                __BREAK_IF_FAIL(parserAdded = gst_bin_add(GST_BIN(mp_sourcePipeline), p_parser));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_parser));
            }
            {
                //create the queue element for recording and decoding (after the parser)
                __BREAK_IF_FAIL(
                    p_sourceQueue3
                    = gst_element_factory_make("queue", "mp_sourcePipeline->mp_sourceQueue3"));
                g_object_set(p_sourceQueue3,
                             "max-size-buffers",
                             200 * 3,
                             nullptr); // 3 times the default value
                g_object_set(p_sourceQueue3,
                             "max-size-bytes",
                             10485760 * 3,
                             nullptr); // 30MB, 3 times the default value
                g_object_set(p_sourceQueue3,
                             "max-size-time",
                             guint64(1000000000) * 3,
                             nullptr); // 3 seconds, 3 times the default value
                g_object_set(p_sourceQueue3, "silent", true, nullptr);
                __BREAK_IF_FAIL(
                    sourceQueue1Added = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceQueue3));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceQueue3));
                //mp_sourceQueue3 = p_sourceQueue3;
            }
            {
                //create the intersink element for the recorder and decoder pipelines
                __BREAK_IF_FAIL(
                    p_sourceSink1 = gst_element_factory_make("intersink",
                                                             "mp_sourcePipeline->mp_sourceSink1"));
                g_object_set(p_sourceSink1, "producer-name", "mp_sourceSink1", nullptr);
                __BREAK_IF_FAIL(
                    sourceSink1Added = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceSink1));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceSink1));
            }
            {
                //create the intersink element for remote stream pipeline
                __BREAK_IF_FAIL(
                    p_sourceSink2 = gst_element_factory_make("intersink",
                                                             "mp_sourcePipeline->mp_sourceSink2"));
                g_object_set(p_sourceSink2, "producer-name", "mp_sourceSink2", nullptr);
                __BREAK_IF_FAIL(
                    sourceSink2Added = gst_bin_add(GST_BIN(mp_sourcePipeline), p_sourceSink2));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sourceSink2));
            }
            {
                __BREAK_IF_FAIL(gst_element_link(p_source, p_sourceTee));
                __BREAK_IF_FAIL(gst_element_link(p_sourceTee, p_sourceQueue1));
                __BREAK_IF_FAIL(gst_element_link(p_sourceQueue1, p_parser));
                _deferredLinkhelper(p_parser);
                __BREAK_IF_FAIL(gst_element_link(p_sourceTee, p_sourceQueue2));
                __BREAK_IF_FAIL(gst_element_link(p_sourceQueue2, p_sourceSink2));
            }
            {
                p_source = nullptr;
                p_tsdemux = nullptr;
                p_buffer = nullptr;
                p_sourceTee = nullptr;
                p_sourceQueue1 = nullptr;
                p_sourceQueue2 = nullptr;
                p_parser = nullptr;
                p_sourceQueue3 = nullptr;
                p_sourceSink1 = nullptr;
                p_sourceSink2 = nullptr;
            }
            {
                //create a means of monitoring bus messages on the source pipeline
                GstBus *p_bus = nullptr;
                __BREAK_IF_FAIL(p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_sourcePipeline)));
                gst_bus_enable_sync_message_emission(p_bus);
                g_signal_connect(p_bus,
                                 "sync-message",
                                 G_CALLBACK(_onSourcePipelineBusMessage),
                                 this);
                gst_object_unref(p_bus);
                p_bus = nullptr;
            }
            running = gst_element_set_state(mp_sourcePipeline, GST_STATE_PLAYING)
                      != GST_STATE_CHANGE_FAILURE;
        } while (0);
        if (running && result == STATUS_OK)
        {
            qCDebug(VideoReceiverLog) << "Started source pipeline with uri=" << m_uri;
        }
        else
        {
            qCCritical(VideoReceiverLog) << "Failed to start source pipeline " << m_uri;
            if (result == STATUS_OK)
            {
                result = STATUS_FAIL;
            }
            if (mp_sourcePipeline != nullptr)
            {
                gst_element_set_state(mp_sourcePipeline, GST_STATE_NULL);
                gst_object_unref(mp_sourcePipeline);
                mp_sourcePipeline = nullptr;
            }
            if (!sourceAdded && p_source)
            {
                gst_object_unref(p_source);
                p_source = nullptr;
            }
            if (!tsdemuxAdded && p_tsdemux)
            {
                gst_object_unref(p_tsdemux);
                p_tsdemux = nullptr;
            }
            if (!bufferAdded && p_buffer)
            {
                gst_object_unref(p_buffer);
                p_buffer = nullptr;
            }
            if (!sourceTeeAdded && p_sourceTee)
            {
                gst_object_unref(p_sourceTee);
                p_sourceTee = nullptr;
            }
            if (!sourceQueue1Added && p_sourceQueue1)
            {
                gst_object_unref(p_sourceQueue1);
                p_sourceQueue1 = nullptr;
            }
            if (!sourceQueue2Added && p_sourceQueue2)
            {
                gst_object_unref(p_sourceQueue2);
                p_sourceQueue2 = nullptr;
            }
            if (!parserAdded && p_parser)
            {
                gst_object_unref(p_parser);
                p_parser = nullptr;
            }
            if (!sourceQueue3Added && p_sourceQueue3)
            {
                gst_object_unref(p_sourceQueue3);
                p_sourceQueue3 = nullptr;
            }
            if (!sourceSink1Added && p_sourceSink1)
            {
                gst_object_unref(p_sourceSink1);
                p_sourceSink1 = nullptr;
            }
            if (!sourceSink2Added && p_sourceSink2)
            {
                gst_object_unref(p_sourceSink2);
                p_sourceSink2 = nullptr;
            }
        }
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_startSourcePipeline " << m_uri;
    return result;
}

void GstVideoReceiver::_stopSourcePipeline()
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_stopSourcePipeline " << m_uri;

    if (mp_sourcePipeline)
    {
        if (m_sourcePipelineProbeId != 0)
        {
            auto *p_sourceTee = gst_bin_get_by_name(GST_BIN(mp_sourcePipeline),
                                                    "mp_sourcePipeline->tee");
            if (p_sourceTee != nullptr)
            {
                //remove the probe from the sink
                auto *p_sinkpad = gst_element_get_static_pad(p_sourceTee, "sink");
                if (p_sinkpad != nullptr)
                {
                    gst_pad_remove_probe(p_sinkpad, m_sourcePipelineProbeId);
                    gst_object_unref(p_sinkpad);
                    p_sinkpad = nullptr;
                }
                gst_object_unref(p_sourceTee);
                p_sourceTee = nullptr;
            }
        }
        gst_element_set_state(mp_sourcePipeline, GST_STATE_NULL);
        auto *p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_sourcePipeline));
        if (p_bus == nullptr)
        {
            qCCritical(VideoReceiverLog) << "gst_pipeline_get_bus() failed";
        }
        else
        {
            gst_bus_disable_sync_message_emission(p_bus);
            g_signal_handlers_disconnect_by_data(p_bus, this);
            gst_object_unref(p_bus);
            p_bus = nullptr;
        }
        gst_object_unref(mp_sourcePipeline);
        mp_sourcePipeline = nullptr;
    }
    m_lastSourceFrameTime = 0;
    m_sourcePipelineProbeId = 0;
    if (m_sourceActive)
    {
        //inform listeners that the streaming as stopped
        m_sourceActive = false;
        qCDebug(VideoReceiverLog) << "Streaming stopped at uri=" << m_uri;
        emit streamingChanged(m_sourceActive);
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_stopSourcePipeline " << m_uri;
}

gboolean GstVideoReceiver::_onSourcePipelineBusMessage(GstBus *const p_bus,
                                                       GstMessage *const p_msg,
                                                       gpointer const p_data)
{
    Q_ASSERT(p_data != nullptr);
    auto *const p_this = (GstVideoReceiver *) p_data;
    auto const result = p_this->_onPipelineBusMessageHelper(
        p_bus,
        p_msg,
        [p_this]()
        {
            p_this->_stopSourcePipeline();
            p_this->_startSourcePipeline();
        },
        "source");
    return result;
}

VideoReceiver::STATUS GstVideoReceiver::_startDecodingPipeline()
{
    // mp_sourceSink1-->intersrc-->queue-->decodebin3-->mp_videoSink]

    //note that mp_videoSink should already be set
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_startDecodingPipeline " << m_uri;
    auto result = STATUS_OK;
    if (mp_decodingPipeline)
    {
        qCDebug(VideoReceiverLog) << "Decoding pipeline already started " << m_uri;
    }
    else if (mp_videoSink == nullptr)
    {
        qCCritical(VideoReceiverLog) << "mp_videoSink should have already been set before "
                                        "calling _startDecodingPipeline "
                                     << m_uri;
        result = STATUS_FAIL;
    }
    else
    {
        GstElement *p_source = nullptr;
        auto sourceAdded = false;
        GstElement *p_queue = nullptr;
        auto queueAdded = false;
        GstElement *p_decoder = nullptr;
        auto decoderAdded = false;
        bool running = false;
        m_lastDecodingFrameTime = 0;
        do
        {
            {
                __BREAK_IF_FAIL(mp_decodingPipeline = gst_pipeline_new("mp_decodingPipeline"));
                g_object_set(mp_decodingPipeline, "message-forward", true, nullptr);
            }

            {
                //create the intersrc element that accepts input from mp_sourceSink1
                __BREAK_IF_FAIL(
                    p_source = gst_element_factory_make("intersrc",
                                                        "mp_decodingPipeline->intersrc"));
                g_object_set(p_source, "producer-name", "mp_sourceSink1", nullptr);
                __BREAK_IF_FAIL(sourceAdded = gst_bin_add(GST_BIN(mp_decodingPipeline), p_source));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_source));
            }
            {
                //create the queue element
                __BREAK_IF_FAIL(
                    p_queue = gst_element_factory_make("queue", "mp_decodingPipeline->queue"));
                g_object_set(p_queue, "leaky", 2, nullptr); // leak older buffers
                g_object_set(p_queue, "silent", true, nullptr);
                __BREAK_IF_FAIL(queueAdded = gst_bin_add(GST_BIN(mp_decodingPipeline), p_queue));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_queue));
            }
            {
                //create the decoder
                __BREAK_IF_FAIL(
                    p_decoder = gst_element_factory_make("decodebin3",
                                                         "mp_decodingPipeline->decodebin3"));
                __BREAK_IF_FAIL(decoderAdded = gst_bin_add(GST_BIN(mp_decodingPipeline), p_decoder));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_decoder));
            }
            {
                //add a probe to the video sink to keep track of when it gets a frame
                GstPad *p_pad = nullptr;
                __BREAK_IF_FAIL(p_pad = gst_element_get_static_pad(mp_videoSink, "sink"));
                m_decodingPipelineProbeId = gst_pad_add_probe(
                    p_pad,
                    GST_PAD_PROBE_TYPE_BUFFER,
                    [](GstPad *const,
                       GstPadProbeInfo *const,
                       gpointer const p_userData) -> GstPadProbeReturn
                    {
                        if (p_userData != nullptr)
                        {
                            //record the frame time so we know if we've lost the feed (see watchdog)
                            auto *const p_this = static_cast<GstVideoReceiver *>(p_userData);
                            p_this->m_lastDecodingFrameTime = QDateTime::currentSecsSinceEpoch();
                            if (!p_this->m_decoderActive)
                            {
                                p_this->m_decoderActive = true;
                                qCDebug(VideoReceiverLog) << "Decoding started " << p_this->m_uri;
                                emit p_this->decodingChanged(p_this->m_decoderActive);
                            }
                        }
                        return GST_PAD_PROBE_OK;
                    },
                    this,
                    nullptr);
                gst_object_unref(p_pad);
                p_pad = nullptr;
                __BREAK_IF_FAIL(gst_bin_add(GST_BIN(mp_decodingPipeline), mp_videoSink));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(mp_videoSink));
            }
            {
                //sync the sink to the state of the pipeline
                gst_element_sync_state_with_parent(mp_videoSink);
                g_object_set(mp_videoSink, "sync", m_buffer >= 0, nullptr);
                // Extracting video size from source is more guaranteed
                if (mp_sourcePipeline)
                {
                    auto *p_sourceElement = gst_bin_get_by_name(GST_BIN(mp_sourcePipeline),
                                                                "mp_sourcePipeline->sourceElement");
                    if (p_sourceElement != nullptr)
                    {
                        auto *p_sourcePad = gst_element_get_static_pad(p_sourceElement, "src");
                        if (p_sourcePad != nullptr)
                        {
                            auto *p_sourcePadCaps = gst_pad_query_caps(p_sourcePad, nullptr);
                            if (p_sourcePadCaps != nullptr)
                            {
                                auto *const p_struct = gst_caps_get_structure(p_sourcePadCaps, 0);
                                if (p_struct != nullptr)
                                {
                                    gint width, height;
                                    gst_structure_get_int(p_struct, "width", &width);
                                    gst_structure_get_int(p_struct, "height", &height);
                                    emit videoSizeChanged(QSize(width, height));
                                }
                                gst_caps_unref(p_sourcePadCaps);
                                p_sourcePadCaps = nullptr;
                            }
                            gst_object_unref(p_sourcePad);
                            p_sourcePad = nullptr;
                        }
                        gst_object_unref(p_sourceElement);
                        p_sourceElement = nullptr;
                    }
                }
            }
            {
                __BREAK_IF_FAIL(gst_element_link(p_source, p_queue));
                __BREAK_IF_FAIL(gst_element_link(p_queue, p_decoder));
                //we must wait until the source pad of the decoder is exposed
                //before we can link it to the sink
                _deferredLinkhelper(p_decoder);
            }
            {
                p_source = nullptr;
                p_queue = nullptr;
                p_decoder = nullptr;
            }
            {
                //create a means of monitoring bus messages on the source pipeline
                GstBus *p_bus = nullptr;
                __BREAK_IF_FAIL(p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_decodingPipeline)));
                gst_bus_enable_sync_message_emission(p_bus);
                g_signal_connect(p_bus,
                                 "sync-message",
                                 G_CALLBACK(_onDecodingPipelineBusMessage),
                                 this);
                gst_object_unref(p_bus);
                p_bus = nullptr;
            }
            running = _syncPipelineToSourceAndStart(mp_decodingPipeline);
        } while (0);
        if (running && result == STATUS_OK)
        {
            qCDebug(VideoReceiverLog) << "Started decoding pipeline with uri=" << m_uri;
        }
        else
        {
            qCCritical(VideoReceiverLog) << "Failed to start decoding pipeline " << m_uri;
            if (result == STATUS_OK)
            {
                result = STATUS_FAIL;
            }
            if (mp_decodingPipeline != nullptr)
            {
                gst_element_set_state(mp_decodingPipeline, GST_STATE_NULL);
                gst_object_unref(mp_decodingPipeline);
                mp_decodingPipeline = nullptr;
            }
            if (!sourceAdded && p_source)
            {
                gst_object_unref(p_source);
                p_source = nullptr;
            }
            if (!queueAdded && p_queue)
            {
                gst_object_unref(p_queue);
                p_queue = nullptr;
            }
            if (!decoderAdded && p_decoder)
            {
                gst_object_unref(p_decoder);
                p_decoder = nullptr;
            }
        }
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_startDecodingPipeline " << m_uri;
    return result;
}

void GstVideoReceiver::_stopDecodingPipeline()
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_stopDecodingPipeline " << m_uri;
    if (m_decodingPipelineProbeId != 0 && mp_videoSink != nullptr)
    {
        //remove the probe from the videosink
        auto *p_sinkpad = gst_element_get_static_pad(mp_videoSink, "sink");
        if (p_sinkpad != nullptr)
        {
            gst_pad_remove_probe(p_sinkpad, m_decodingPipelineProbeId);
            gst_object_unref(p_sinkpad);
            p_sinkpad = nullptr;
        }
    }
    if (mp_decodingPipeline)
    {
        gst_element_set_state(mp_decodingPipeline, GST_STATE_NULL);
        if (mp_videoSink)
        {
            //remove and unlink the video sink, but do not dispose of it
            gst_bin_remove(GST_BIN(mp_decodingPipeline), mp_videoSink);
            gst_element_set_state(mp_videoSink, GST_STATE_NULL);
        }
        auto *p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_decodingPipeline));
        if (p_bus == nullptr)
        {
            qCCritical(VideoReceiverLog) << "gst_pipeline_get_bus() failed";
        }
        else
        {
            gst_bus_disable_sync_message_emission(p_bus);
            g_signal_handlers_disconnect_by_data(p_bus, this);
            gst_object_unref(p_bus);
            p_bus = nullptr;
        }
        //TODO find out why this throws an exception when debugging
        gst_object_unref(mp_decodingPipeline);
        mp_decodingPipeline = nullptr;
    }
    m_lastDecodingFrameTime = 0;
    m_decodingPipelineProbeId = 0;
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_stopDecodingPipeline " << m_uri;
}

gboolean GstVideoReceiver::_onDecodingPipelineBusMessage(GstBus *const p_bus,
                                                         GstMessage *const p_msg,
                                                         gpointer const p_data)
{
    Q_ASSERT(p_data != nullptr);
    auto *const p_this = (GstVideoReceiver *) p_data;
    auto const result = p_this->_onPipelineBusMessageHelper(
        p_bus,
        p_msg,
        [p_this]()
        {
            p_this->_stopDecodingPipeline();
            p_this->_startDecodingPipeline();
        },
        "decoding");
    return result;
}

VideoReceiver::STATUS GstVideoReceiver::_startRecordingPipeline()
{
    // mp_sourceSink1-->intersrc-->queue-->mux-->filesink]

    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_startRecordingPipeline " << m_uri;
    auto result = STATUS_OK;
    if (mp_recordingPipeline)
    {
        qCDebug(VideoReceiverLog) << "Recording pipeline already started " << m_uri;
    }
    else if (m_videoFile.trimmed().isEmpty() || m_videoFormat < FILE_FORMAT_MIN
             || m_videoFormat >= FILE_FORMAT_MAX)
    {
        qCCritical(VideoReceiverLog) << "Video file is empty or format is invalid! " << m_uri;
        result = STATUS_FAIL;
    }
    else
    {
        GstElement *p_source = nullptr;
        auto sourceAdded = false;
        GstElement *p_queue = nullptr;
        auto queueAdded = false;
        GstElement *p_mux = nullptr;
        auto muxAdded = false;
        GstElement *p_sink = nullptr;
        auto sinkAdded = false;
        auto running = false;
        m_lastRecordingFrameTime = 0;
        do
        {
            {
                __BREAK_IF_FAIL(mp_recordingPipeline = gst_pipeline_new("mp_recordingPipeline"));
                g_object_set(mp_recordingPipeline, "message-forward", true, nullptr);
            }

            {
                //create the intersrc element that accepts input from mp_sourceSink1
                __BREAK_IF_FAIL(
                    p_source = gst_element_factory_make("intersrc",
                                                        "mp_recordingPipeline->intersrc"));
                g_object_set(p_source, "producer-name", "mp_sourceSink1", nullptr);
                __BREAK_IF_FAIL(sourceAdded = gst_bin_add(GST_BIN(mp_recordingPipeline), p_source));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_source));
            }
            {
                //create the queue element
                __BREAK_IF_FAIL(
                    p_queue = gst_element_factory_make("queue", "mp_recordingPipeline->queue"));
                g_object_set(p_queue,
                             "max-size-buffers",
                             200 * 3,
                             nullptr); // 3 times the default value
                g_object_set(p_queue,
                             "max-size-bytes",
                             10485760 * 3,
                             nullptr); // 30MB, 3 times the default value
                g_object_set(p_queue,
                             "max-size-time",
                             guint64(1000000000) * 3,
                             nullptr); // 3 seconds, 3 times the default value
                g_object_set(p_queue, "silent", true, nullptr);
                __BREAK_IF_FAIL(queueAdded = gst_bin_add(GST_BIN(mp_recordingPipeline), p_queue));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_queue));
            }
            {
                //create the mux element
                __BREAK_IF_FAIL(
                    p_mux = gst_element_factory_make(np_kFileMux[m_videoFormat - FILE_FORMAT_MIN],
                                                     "mp_recordingPipeline->mux"));
                __BREAK_IF_FAIL(muxAdded = gst_bin_add(GST_BIN(mp_recordingPipeline), p_mux));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_mux));
            }
            {
                //create the sink element
                __BREAK_IF_FAIL(
                    p_sink = gst_element_factory_make("filesink", "mp_recordingPipeline->filesink"));
                g_object_set(static_cast<gpointer>(p_sink),
                             "location",
                             qPrintable(m_videoFile),
                             nullptr);
                __BREAK_IF_FAIL(sinkAdded = gst_bin_add(GST_BIN(mp_recordingPipeline), p_sink));
                __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sink));
            }
            {
                GstPad *p_keyFrameWatchProbePad = nullptr;
                __BREAK_IF_FAIL(
                    p_keyFrameWatchProbePad = gst_element_get_static_pad(p_source, "src"));
                // Install a probe on the recording branch to drop buffers until we hit our first keyframe
                // When we hit our first keyframe, we can offset the timestamps appropriately according to the first keyframe time
                // This will ensure the first frame is a keyframe at t=0, and decoding can begin immediately on playback
                //TODO investigate if this can be worked into the probe below (m_recordingPipelineProbeId)
                gst_pad_add_probe(
                    p_keyFrameWatchProbePad,
                    GST_PAD_PROBE_TYPE_BUFFER,
                    [](GstPad *const p_pad,
                       GstPadProbeInfo *const p_info,
                       gpointer const p_userData) -> GstPadProbeReturn
                    {
                        auto result = GST_PAD_PROBE_DROP;
                        if (p_info == nullptr || p_userData == nullptr)
                        {
                            qCCritical(VideoReceiverLog) << "Invalid arguments";
                        }
                        else
                        {
                            auto *const p_buf = gst_pad_probe_info_get_buffer(p_info);
                            if (!GST_BUFFER_FLAG_IS_SET(p_buf, GST_BUFFER_FLAG_DELTA_UNIT))
                            { // wait for a keyframe
                                // set media file '0' offset to current timeline position - we don't want to touch other elements in the graph, except these which are downstream!
                                gst_pad_set_offset(p_pad, -static_cast<gint64>(p_buf->pts));
                                GstVideoReceiver *p_this = static_cast<GstVideoReceiver *>(
                                    p_userData);
                                qCDebug(VideoReceiverLog)
                                    << "Got keyframe, stop dropping buffers " << p_this->m_uri;
                                emit p_this->recordingStarted();
                                result = GST_PAD_PROBE_REMOVE;
                            }
                        }
                        return result;
                    },
                    this,
                    nullptr);
                gst_object_unref(p_keyFrameWatchProbePad);
                p_keyFrameWatchProbePad = nullptr;
            }
            {
                //add a probe to the sink to keep track of when it gets a frame
                GstPad *p_sinkPad = nullptr;
                __BREAK_IF_FAIL(p_sinkPad = gst_element_get_static_pad(p_sink, "sink"));
                m_recordingPipelineProbeId = gst_pad_add_probe(
                    p_sinkPad,
                    GST_PAD_PROBE_TYPE_BUFFER,
                    [](GstPad *const p_pad,
                       GstPadProbeInfo *const p_info,
                       gpointer const p_userData) -> GstPadProbeReturn
                    {
                        if (p_info != nullptr && p_userData != nullptr)
                        {
                            //record the frame time so we know if we've lost the feed (see watchdog)
                            auto *const p_this = static_cast<GstVideoReceiver *>(p_userData);
                            p_this->m_lastRecordingFrameTime = QDateTime::currentSecsSinceEpoch();
                            if (!p_this->m_recorderActive)
                            {
                                //notify listeners that the recorder is active
                                p_this->m_recorderActive = true;
                                qCDebug(VideoReceiverLog) << "Recording started " << p_this->m_uri;
                                emit p_this->recordingChanged(p_this->m_recorderActive);
                            }
                        }
                        return GST_PAD_PROBE_OK;
                    },
                    this,
                    nullptr);
                gst_object_unref(p_sinkPad);
                p_sinkPad = nullptr;
            }
            {
                __BREAK_IF_FAIL(gst_element_link(p_source, p_queue));
                __BREAK_IF_FAIL(gst_element_link(p_queue, p_mux));
                __BREAK_IF_FAIL(gst_element_link(p_mux, p_sink));
            }
            {
                p_source = nullptr;
                p_queue = nullptr;
                p_mux = nullptr;
                p_sink = nullptr;
            }
            {
                //create a means of monitoring bus messages on the source pipeline
                GstBus *p_bus = nullptr;
                __BREAK_IF_FAIL(p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_recordingPipeline)));
                gst_bus_enable_sync_message_emission(p_bus);
                g_signal_connect(p_bus,
                                 "sync-message",
                                 G_CALLBACK(_onRecordingPipelineBusMessage),
                                 this);
                gst_object_unref(p_bus);
                p_bus = nullptr;
            }
            running = _syncPipelineToSourceAndStart(mp_recordingPipeline);
        } while (0);
        if (running && result == STATUS_OK)
        {
            qCDebug(VideoReceiverLog) << "Started recording pipeline with uri=" << m_uri;
        }
        else
        {
            qCCritical(VideoReceiverLog) << "Failed to start decoding pipeline " << m_uri;
            if (result == STATUS_OK)
            {
                result = STATUS_FAIL;
            }
            if (mp_recordingPipeline != nullptr)
            {
                gst_element_set_state(mp_recordingPipeline, GST_STATE_NULL);
                gst_object_unref(mp_recordingPipeline);
                mp_recordingPipeline = nullptr;
            }
            if (!sourceAdded && p_source)
            {
                gst_object_unref(p_source);
                p_source = nullptr;
            }
            if (!queueAdded && p_queue)
            {
                gst_object_unref(p_queue);
                p_queue = nullptr;
            }
            if (!muxAdded && p_mux)
            {
                gst_object_unref(p_mux);
                p_mux = nullptr;
            }
            if (!sinkAdded && p_sink)
            {
                gst_object_unref(p_sink);
                p_sink = nullptr;
            }
        }
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_startRecordingPipeline " << m_uri;
    return result;
}

void GstVideoReceiver::_stopRecordingPipeline()
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_stopRecordingPipeline " << m_uri;

    if (mp_recordingPipeline)
    {
        if (m_recordingPipelineProbeId != 0)
        {
            auto *p_fileSink = gst_bin_get_by_name(GST_BIN(mp_recordingPipeline),
                                                   "mp_recordingPipeline->filesink");
            if (p_fileSink != nullptr)
            {
                //remove the probe from the sink
                auto *p_sinkpad = gst_element_get_static_pad(p_fileSink, "sink");
                if (p_sinkpad != nullptr)
                {
                    gst_pad_remove_probe(p_sinkpad, m_sourcePipelineProbeId);
                    gst_object_unref(p_sinkpad);
                    p_sinkpad = nullptr;
                }
                gst_object_unref(p_fileSink);
                p_fileSink = nullptr;
            }
        }
        //stop the pipeline
        gst_element_set_state(mp_recordingPipeline, GST_STATE_NULL);
        auto *p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_recordingPipeline));
        if (p_bus == nullptr)
        {
            qCCritical(VideoReceiverLog) << "gst_pipeline_get_bus() failed";
        }
        else
        {
            gst_bus_disable_sync_message_emission(p_bus);
            g_signal_handlers_disconnect_by_data(p_bus, this);
            gst_object_unref(p_bus);
            p_bus = nullptr;
        }
        gst_object_unref(mp_recordingPipeline);
        mp_recordingPipeline = nullptr;
    }
    m_lastRecordingFrameTime = 0;
    m_recordingPipelineProbeId = 0;
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_stopRecordingPipeline " << m_uri;
}

gboolean GstVideoReceiver::_onRecordingPipelineBusMessage(GstBus *const p_bus,
                                                          GstMessage *const p_msg,
                                                          gpointer const p_data)
{
    Q_ASSERT(p_data != nullptr);
    auto *const p_this = (GstVideoReceiver *) p_data;
    auto const result = p_this->_onPipelineBusMessageHelper(
        p_bus,
        p_msg,
        [p_this]()
        {
            p_this->_stopRecordingPipeline();
            p_this->_startRecordingPipeline();
        },
        "recording");
    return result;
}

VideoReceiver::STATUS GstVideoReceiver::_startRemoteStreamPipeline()
{
    // mp_sourceSink2-->intersrc-->queue-->parser-->mpegtsmux-->srtsink]

    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_startRemoteStreamPipeline " << m_uri;
    auto result = STATUS_OK;
    if (mp_remoteStreamPipeline)
    {
        qCDebug(VideoReceiverLog) << "Remote stream pipeline already started " << m_uri;
    }
    else
    {
        auto const isSrt = m_remoteStreamURI.startsWith("srt://", Qt::CaseInsensitive);
        auto const isUdp265 = m_uri.startsWith("udp265://", Qt::CaseInsensitive);
        auto const isUdp264 = m_uri.startsWith("udp://", Qt::CaseInsensitive);
        if (!isSrt)
        {
            qCCritical(VideoReceiverLog)
                << "Only SRT remote streaming URLs are supported " << m_uri;
            result = STATUS_INVALID_URL;
        }
        else if (!isUdp265 && !isUdp264)
        {
            qCCritical(VideoReceiverLog)
                << "Only h265 and h264 sources are supported for remote-streaming " << m_uri;
            result = STATUS_INVALID_URL;
        }
        else
        {
            GstElement *p_source = nullptr;
            auto sourceAdded = false;
            GstElement *p_queue = nullptr;
            auto queueAdded = false;
            GstElement *p_parser = nullptr;
            auto parserAdded = false;
            GstElement *p_mux = nullptr;
            auto muxAdded = false;
            GstElement *p_sink = nullptr;
            auto sinkAdded = false;
            auto running = false;
            m_lastRemoteStreamFrameTime = 0;
            do
            {
                {
                    __BREAK_IF_FAIL(
                        mp_remoteStreamPipeline = gst_pipeline_new("mp_remoteStreamPipeline"));
                    g_object_set(mp_remoteStreamPipeline, "message-forward", true, nullptr);
                }
                {
                    //create the intersrc element that accepts input from mp_sourceSink1
                    __BREAK_IF_FAIL(
                        p_source = gst_element_factory_make("intersrc",
                                                            "mp_remoteStreamPipeline->intersrc"));
                    g_object_set(p_source, "producer-name", "mp_sourceSink2", nullptr);
                    __BREAK_IF_FAIL(
                        sourceAdded = gst_bin_add(GST_BIN(mp_remoteStreamPipeline), p_source));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_source));
                }
                {
                    //create the queue element
                    __BREAK_IF_FAIL(
                        p_queue = gst_element_factory_make("queue",
                                                           "mp_remoteStreamPipeline->queue"));
                    g_object_set(p_queue, "leaky", 2, nullptr); // leak older buffers
                    g_object_set(p_queue, "silent", true, nullptr);
                    __BREAK_IF_FAIL(
                        queueAdded = gst_bin_add(GST_BIN(mp_remoteStreamPipeline), p_queue));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_queue));
                }
                {
                    //create the parser element
                    if (isUdp265)
                    {
                        __BREAK_IF_FAIL(
                            p_parser
                            = gst_element_factory_make("rtph265depay",
                                                       "mp_remoteStreamPipeline->rtph265depay"));
                    }
                    else if (isUdp264)
                    {
                        __BREAK_IF_FAIL(
                            p_parser
                            = gst_element_factory_make("rtph264depay",
                                                       "mp_remoteStreamPipeline->rtph264depay"));
                    }
                    __BREAK_IF_FAIL(
                        parserAdded = gst_bin_add(GST_BIN(mp_remoteStreamPipeline), p_parser));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_parser));
                }
                {
                    //create the mux element
                    __BREAK_IF_FAIL(
                        p_mux = gst_element_factory_make("mpegtsmux",
                                                         "mp_remoteStreamPipeline->mpegtsmux"));
                    __BREAK_IF_FAIL(muxAdded = gst_bin_add(GST_BIN(mp_remoteStreamPipeline), p_mux));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_mux));
                }
                {
                    //create the sink element
                    __BREAK_IF_FAIL(
                        p_sink = gst_element_factory_make("srtsink",
                                                          "mp_remoteStreamPipeline->srtsink"));
                    g_object_set(static_cast<gpointer>(p_sink),
                                 "uri",
                                 qPrintable(m_remoteStreamURI),
                                 nullptr);
                    __BREAK_IF_FAIL(
                        sinkAdded = gst_bin_add(GST_BIN(mp_remoteStreamPipeline), p_sink));
                    __BREAK_IF_FAIL(gst_element_sync_state_with_parent(p_sink));
                }
                {
                    //add a probe to the sink to keep track of when it gets a frame
                    GstPad *p_sinkPad = nullptr;
                    __BREAK_IF_FAIL(p_sinkPad = gst_element_get_static_pad(p_sink, "sink"));
                    m_remoteStreamPipelineProbeId = gst_pad_add_probe(
                        p_sinkPad,
                        GST_PAD_PROBE_TYPE_BUFFER_LIST,
                        [](GstPad *const p_pad,
                           GstPadProbeInfo *const p_info,
                           gpointer const p_userData) -> GstPadProbeReturn
                        {
                            if (p_info != nullptr && p_userData != nullptr)
                            {
                                //record the frame time so we know if we've lost the feed (see watchdog)
                                auto *const p_this = static_cast<GstVideoReceiver *>(p_userData);
                                p_this->m_lastRemoteStreamFrameTime
                                    = QDateTime::currentSecsSinceEpoch();
                                if (!p_this->m_remoteStreamActive)
                                {
                                    p_this->m_remoteStreamActive = true;
                                    qCDebug(VideoReceiverLog)
                                        << "Remote stream started " << p_this->m_uri;
                                    emit p_this->remoteStreamingChanged(
                                        p_this->m_remoteStreamActive);
                                }
                            }
                            return GST_PAD_PROBE_OK;
                        },
                        this,
                        nullptr);
                    gst_object_unref(p_sinkPad);
                    p_sinkPad = nullptr;
                }
                {
                    __BREAK_IF_FAIL(gst_element_link(p_source, p_queue));
                    __BREAK_IF_FAIL(gst_element_link(p_queue, p_parser));
                    __BREAK_IF_FAIL(gst_element_link(p_parser, p_mux));
                    __BREAK_IF_FAIL(gst_element_link(p_mux, p_sink));
                }
                {
                    p_source = nullptr;
                    p_queue = nullptr;
                    p_parser = nullptr;
                    p_mux = nullptr;
                    p_sink = nullptr;
                }
                {
                    //create a means of monitoring bus messages on the remote streaming pipeline
                    GstBus *p_bus = nullptr;
                    __BREAK_IF_FAIL(
                        p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_remoteStreamPipeline)));
                    gst_bus_enable_sync_message_emission(p_bus);
                    g_signal_connect(p_bus,
                                     "sync-message",
                                     G_CALLBACK(_onRemoteStreamPipelineBusMessage),
                                     this);
                    gst_object_unref(p_bus);
                    p_bus = nullptr;
                }
                running = _syncPipelineToSourceAndStart(mp_remoteStreamPipeline);

            } while (0);
            if (running && result == STATUS_OK)
            {
                qCDebug(VideoReceiverLog) << "Started remote stream pipeline with uri=" << m_uri;
            }
            else
            {
                qCCritical(VideoReceiverLog) << "Failed to start remote stream pipeline " << m_uri;
                if (result == STATUS_OK)
                {
                    result = STATUS_FAIL;
                }
                if (mp_remoteStreamPipeline != nullptr)
                {
                    gst_element_set_state(mp_remoteStreamPipeline, GST_STATE_NULL);
                    gst_object_unref(mp_remoteStreamPipeline);
                    mp_remoteStreamPipeline = nullptr;
                }
                if (!sourceAdded && p_source)
                {
                    gst_object_unref(p_source);
                    p_source = nullptr;
                }
                if (!queueAdded && p_queue)
                {
                    gst_object_unref(p_queue);
                    p_queue = nullptr;
                }
                if (!parserAdded && p_parser)
                {
                    gst_object_unref(p_parser);
                    p_parser = nullptr;
                }
                if (!muxAdded && p_mux)
                {
                    gst_object_unref(p_mux);
                    p_mux = nullptr;
                }
                if (!sinkAdded && p_sink)
                {
                    gst_object_unref(p_sink);
                    p_sink = nullptr;
                }
            }
        }
    }
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_startRemoteStreamPipeline " << m_uri;
    return result;
}

void GstVideoReceiver::_stopRemoteStreamPipeline()
{
    qCDebug(VideoReceiverLog) << "ENTER GstVideoReceiver::_stopRemoteStreamPipeline " << m_uri;
    if (mp_remoteStreamPipeline)
    {
        if (m_remoteStreamPipelineProbeId != 0)
        {
            auto *p_remoteStreamSink = gst_bin_get_by_name(GST_BIN(mp_remoteStreamPipeline),
                                                           "mp_remoteStreamPipeline->srtsink");
            if (p_remoteStreamSink != nullptr)
            {
                //remove the probe from the sink
                auto *p_sinkpad = gst_element_get_static_pad(p_remoteStreamSink, "sink");
                if (p_sinkpad != nullptr)
                {
                    gst_pad_remove_probe(p_sinkpad, m_sourcePipelineProbeId);
                    gst_object_unref(p_sinkpad);
                    p_sinkpad = nullptr;
                }
                gst_object_unref(p_remoteStreamSink);
                p_remoteStreamSink = nullptr;
            }
        }
        gst_element_set_state(mp_remoteStreamPipeline, GST_STATE_NULL);
        auto *p_bus = gst_pipeline_get_bus(GST_PIPELINE(mp_remoteStreamPipeline));
        if (p_bus == nullptr)
        {
            qCCritical(VideoReceiverLog) << "gst_pipeline_get_bus() failed";
        }
        else
        {
            gst_bus_disable_sync_message_emission(p_bus);
            g_signal_handlers_disconnect_by_data(p_bus, this);
            gst_object_unref(p_bus);
            p_bus = nullptr;
        }
        gst_object_unref(mp_remoteStreamPipeline);
        mp_remoteStreamPipeline = nullptr;
    }
    m_lastRemoteStreamFrameTime = 0;
    m_remoteStreamPipelineProbeId = 0;
    qCDebug(VideoReceiverLog) << "EXIT GstVideoReceiver::_stopRemoteStreamPipeline " << m_uri;
}

gboolean GstVideoReceiver::_onRemoteStreamPipelineBusMessage(GstBus *const p_bus,
                                                             GstMessage *const p_msg,
                                                             gpointer const p_data)
{
    Q_ASSERT(p_data != nullptr);
    auto *const p_this = (GstVideoReceiver *) p_data;
    auto const result = p_this->_onPipelineBusMessageHelper(
        p_bus,
        p_msg,
        [p_this]()
        {
            p_this->_stopRemoteStreamPipeline();
            p_this->_startRemoteStreamPipeline();
        },
        "remotestream");
    return result;
}
