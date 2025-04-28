//
// Created by liu_zongchang on 2025/1/14 14:01.
// Email 1439797751@qq.com
// 
//

#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include <QThread>
#include <queue>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/timestamp.h>
    #include <libavdevice/avdevice.h>
}

#define ERROR_TIMES_MAX 30

class FFmpegRecorder final : public QThread{
    Q_OBJECT
public:
    enum Status {
        STATUS_OK = 0,
        STATUS_ERROR,
        STATUS_INITIALIZING,
        STATUS_RECORDING,
        STATUS_STOPPED,
    };
    Q_ENUM(Status)

    static const char *getErrorString(const int errorCode) {
        const auto errorStr = new char[1024];
        return av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, errorCode);
    }

    static const char *getTsString(const int64_t ts) {
        const auto tsStr = new char[1024];
        return av_ts_make_string(tsStr, ts);
    }

    static const char *getTsTimeString(const int64_t ts, AVRational* tb) {
        const auto tsTimeStr = new char[1024];
        return av_ts_make_time_string(tsTimeStr, ts, tb);
    }

    explicit FFmpegRecorder(QObject *parent = nullptr);
    ~FFmpegRecorder() override;

    [[nodiscard]] QString getDstPath() const { return QString::fromStdString(m_dstPath); }

    int init();

    int startRecord(const std::string &path, int width, int height, int fps);
    int stopRecord();

    void run() override;

    Q_SLOT void onFrameReceived(const AVFrame* frame);

    Q_SIGNAL void sigRecordStatusChange(int status);
private:
    bool m_isRecording{false};
    bool m_isInitialized{false};

    std::string m_dstPath;
    AVDictionary* m_options{nullptr};
    AVFormatContext* m_dstFormatContext{nullptr};
    AVCodecContext* m_dstCodecContext{nullptr};
    AVStream* m_dstStream{nullptr};
    int m_width{-1};
    int m_height{-1};
    int m_fps{-1};

    SwsContext* m_swsContext{nullptr};
    std::queue<AVFrame*> m_frameQueue;
    AVFrame* m_frame{nullptr};
    AVPacket* m_packet{nullptr};
};



#endif //FFMPEG_RECORDER_H
