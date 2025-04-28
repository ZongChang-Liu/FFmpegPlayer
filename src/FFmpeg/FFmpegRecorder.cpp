//
// Created by liu_zongchang on 2025/1/14 14:01.
// Email 1439797751@qq.com
// 
//


#include <QImage>
#include "FFmpegRecorder.h"

#include "FileTools/Microscope_Utils_Log.h"

FFmpegRecorder::FFmpegRecorder(QObject* parent) : QThread(parent)
{
    av_log_set_level(AV_LOG_INFO);
    avdevice_register_all();
}

FFmpegRecorder::~FFmpegRecorder()
{
    stopRecord();
}

int FFmpegRecorder::init()
{
    Q_EMIT sigRecordStatusChange(STATUS_INITIALIZING);
    if (!m_isInitialized)
    {
        LOG_ERROR("FFmpegRecorder is not initialized");
        return -1;
    }

    int ret = avformat_alloc_output_context2(&m_dstFormatContext, nullptr, "mp4", m_dstPath.c_str());
    if (ret < 0) {
        LOG_ERROR("alloc output context failed: {}", getErrorString(ret));
        return -1;
    }

    m_dstStream = avformat_new_stream(m_dstFormatContext, nullptr);
    if (m_dstStream == nullptr) {
        LOG_ERROR("new stream failed");
        return -1;
    }
    m_dstStream->codecpar->codec_tag = 0;

    const AVCodec* codec = avcodec_find_encoder_by_name("libx264");
    if (codec == nullptr) {
        LOG_ERROR("find encoder failed");
        return -1;
    }
    LOG_INFO("find encoder success: {}", codec->name);

    m_dstCodecContext = avcodec_alloc_context3(codec);
    if (m_dstCodecContext == nullptr) {
        LOG_ERROR("alloc codecCtx failed");
        return -1;
    }

    m_dstCodecContext->width = m_width;
    m_dstCodecContext->height = m_height;
    m_dstCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    m_dstCodecContext->time_base = {1, m_fps};
    m_dstCodecContext->framerate = {m_fps, 1};
    m_dstCodecContext->bit_rate = 4000000;
    m_dstCodecContext->gop_size = 10;

    ret = avcodec_open2(m_dstCodecContext, codec, nullptr);
    if (ret < 0) {
        LOG_ERROR("open codec failed: {}", getErrorString(ret));
        return -1;
    }

    m_swsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_RGB24, m_width, m_height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (m_swsContext == nullptr) {
        LOG_ERROR("sws_getContext failed");
        return -1;
    }

    ret = avcodec_parameters_from_context(m_dstStream->codecpar, m_dstCodecContext);
    if (ret < 0) {
        LOG_ERROR("copy codec parameters to context failed: {}", getErrorString(ret));
        return -1;
    }

    av_dump_format(m_dstFormatContext, 0, m_dstPath.c_str(), 1);

    ret = avio_open(&m_dstFormatContext->pb, m_dstPath.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0) {
        LOG_ERROR("avio_open failed: {}", getErrorString(ret));
        return -1;
    }

    m_frame = av_frame_alloc();
    if (m_frame == nullptr) {
        LOG_ERROR("alloc frame failed");
        return -1;
    }

    m_frame->format = m_dstCodecContext->pix_fmt;
    m_frame->width = m_width;
    m_frame->height = m_height;

    ret = av_frame_get_buffer(m_frame, 0);
    if (ret < 0) {
        LOG_ERROR("alloc frame buffer failed: {}", getErrorString(ret));
        return -1;
    }

    m_packet = av_packet_alloc();
    if (m_packet == nullptr) {
        LOG_ERROR("alloc packet failed");
        return -1;
    }

    return 0;
}

int FFmpegRecorder::startRecord(const std::string& path, const int width, const int height, const int fps)
{
    if (path.empty() || width <= 0 || height <= 0 || fps <= 0) {
        LOG_ERROR("input params is invalid");
        return -1;
    }

    if (m_isRecording) {
        stopRecord();
    }

    m_dstPath = path;
    m_width = width;
    m_height = height;
    m_fps = fps;
    m_isInitialized = true;
    start();
    return 0;
}

int FFmpegRecorder::stopRecord()
{
    m_isRecording = false;
    this->wait();

    if (m_swsContext != nullptr) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }

    if (m_dstCodecContext != nullptr) {
        avcodec_close(m_dstCodecContext);
        avcodec_free_context(&m_dstCodecContext);
        m_dstCodecContext = nullptr;
    }

    if (m_dstFormatContext != nullptr) {
        avformat_free_context(m_dstFormatContext);
        m_dstFormatContext = nullptr;
    }

    if (m_frame != nullptr) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    m_width = -1;
    m_height = -1;
    m_fps = -1;
    m_dstPath.clear();

    m_isInitialized = false;

    return 0;
}


void FFmpegRecorder::run()
{
    if (init() < 0) {
        return;
    }

    int ret = avformat_write_header(m_dstFormatContext, nullptr);
    if (ret < 0) {
        LOG_ERROR("write header failed: {}\n", getErrorString(ret));
        return;
    }

    m_isRecording = true;
    int pts = 0;
    Q_EMIT sigRecordStatusChange(STATUS_RECORDING);
    bool isError = false;
    while (m_isRecording && !isError)
    {
        if (m_frameQueue.empty()) {
            msleep(1000/m_fps);
            continue;
        }
        const AVFrame* frame = m_frameQueue.front();
        m_frameQueue.pop();
        ret = sws_scale(m_swsContext, frame->data, frame->linesize, 0, frame->height, m_frame->data, m_frame->linesize);
        if (ret < 0) {
            LOG_ERROR("sws_scale failed: {}\n", getErrorString(ret));
            Q_EMIT sigRecordStatusChange(STATUS_ERROR);
            isError = true;
            break;
        }
        m_frame->pts = pts++;
        ret = avcodec_send_frame(m_dstCodecContext, m_frame);
        if (ret < 0) {
            LOG_ERROR("send frame failed: {}\n", getErrorString(ret));
            Q_EMIT sigRecordStatusChange(STATUS_ERROR);
            isError = true;
            break;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(m_dstCodecContext, m_packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                break;
            }
            if (ret < 0) {
                LOG_ERROR("receive packet failed: {}\n", getErrorString(ret));
                isError = true;
                break;
            }

            av_packet_rescale_ts(m_packet, m_dstCodecContext->time_base, m_dstStream->time_base);
            m_packet->stream_index = m_dstStream->index;
            ret = av_interleaved_write_frame(m_dstFormatContext, m_packet);
            if (ret < 0) {
                LOG_ERROR("write frame failed: {}\n", getErrorString(ret));
                isError = true;
                break;
            }
            LOG_INFO("write frame success");
        }
    }

    ret = av_write_trailer(m_dstFormatContext);
    if (ret < 0) {
        LOG_ERROR("write trailer failed: {}\n", getErrorString(ret));
    }
    avio_close(m_dstFormatContext->pb);
    Q_EMIT sigRecordStatusChange(STATUS_STOPPED);
    if (!isError)
    {
        Q_EMIT sigRecordStatusChange(STATUS_OK);
    }
}

void FFmpegRecorder::onFrameReceived(const AVFrame* frame)
{
    if (m_isRecording)
    {
        m_frameQueue.push(av_frame_clone(frame));
    }
}
