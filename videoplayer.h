#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}


/**
 * @brief The VideoPlayer class 预处理视频数据
 */
class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    // 状态
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
    } State;

    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    void play();
    void pause();
    void stop();
    bool isPlaying();
    State getState();
    void setFilename(QString &filename);
    const char * getFilename();
    int64_t getDuration();

signals:
    void stateChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);

public slots:

private:
    // 当前播放器状态
    State _state = Stopped;

    // 当前播放器播放文件名
    char _filename[512];

    // 解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    // 解码上下文
    AVCodecContext *_aDecodeCtx = nullptr, *_vDecodeCtx = nullptr;
    // 流
    AVStream *_aStream = nullptr, *_vStream = nullptr;
    // 解码后的数据
    AVFrame *_aFrame = nullptr, *_vFrame = nullptr;

    // 初始化
    int initVideoInfo();
    int initAudioInfo();
    int initDecoder(AVCodecContext **decodeCtx, AVStream **stream, AVMediaType type);


    // 设置播放器状态
    void setState(State state);
    // 读取文件数据
    void readFile();
};

#endif // VIDEOPLAYER_H
