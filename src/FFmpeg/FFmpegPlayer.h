//
// Created by liu_zongchang on 2025/1/13 1:52.
// Email 1439797751@qq.com
// 
//

#ifndef FFMPEG_PLAYER_H
#define FFMPEG_PLAYER_H

#include <QThread>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavdevice/avdevice.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

#define ERROR_TIMES_MAX 30



class FFmpegPlayer final : public QThread {
    Q_OBJECT
public:
    enum Status {
        STATUS_ERROR,
        STATUS_INITIALIZING,
        STATUS_PLAYING,
        STATUS_STOPPED,
    };
    Q_ENUM(Status)

    static const char *getErrorString(const int errorCode) {
        const auto errorStr = new char[1024];
        return av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, errorCode);
    }

    explicit FFmpegPlayer(QObject *parent = nullptr);
    ~FFmpegPlayer() override;

    int open(const std::string &url, AVDictionary *options = nullptr);
    int close();

    void playDevice(const QString& path, int width, int height, int fps, const QString& format);
    void playUrl(const std::string& url, AVDictionary* options = nullptr);

    [[nodiscard]]AVFrame* getFrame() const;
    [[nodiscard]] int getFrameWidth() const;
    [[nodiscard]] int getFrameHeight() const;
    [[nodiscard]] int getFps() const;

    void run() override;

    Q_SIGNAL void sigFrameReaded(const AVFrame* frame);
    Q_SIGNAL void sigPlayStatusChange(int statue);
    Q_SIGNAL void sigNewFrame();
private:
    bool m_isPlaying{false};
    bool m_isOpened{false};

    AVFormatContext* m_srcFormatContext{nullptr};
    AVCodecContext* m_srcCodecContext{nullptr};
    AVDictionary* m_options{nullptr};
    QString m_srcPath;
    int m_videoStreamIndex{-1};
    int m_width{-1};
    int m_height{-1};
    int m_fps{-1};
    QString m_format;

    SwsContext* m_swsContext{nullptr};

    AVFrame* m_frame{nullptr};
};



#endif //FFMPEG_PLAYER_H
