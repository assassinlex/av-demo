#include "videoplayer.h"
#include <thread>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}


#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof(errbuf));

#define END(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "errno:" << ret << "error:" << errbuf; \
        goto end; \
    }

#define RET(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        return ret; \
    }

VideoPlayer::VideoPlayer(QObject *parent) : QObject(parent)
{

}

VideoPlayer::~VideoPlayer()
{
    // TODO
}

void VideoPlayer::play()
{
    if (_state == Playing) return;

    // 子线程读取文件数据
    std::thread([this]() {
        readFile();
    }).detach();

    setState(Playing);
}

void VideoPlayer::pause()
{
    if (_state != Playing) return;

    setState(Paused);
}

void VideoPlayer::stop()
{
    if (_state == Stopped) return;

    setState(Stopped);
}

bool VideoPlayer::isPlaying()
{
    return _state == Playing;
}

VideoPlayer::State VideoPlayer::getState()
{
    return _state;
}

void VideoPlayer::setFilename(QString &filename)
{
    QByteArray name = filename.toLocal8Bit();
    memcpy(_filename, name.data(), name.size() + 1);
}

const char * VideoPlayer::getFilename()
{
    return _filename;
}

void VideoPlayer::setState(State state)
{
    if (_state == state) return;

    _state = state;

    emit stateChanged(this);
}

void VideoPlayer::readFile()
{
    //
    int ret = 0;

    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, _filename, nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 控制台输出流信息
    av_dump_format(_fmtCtx, 0, _filename, 0);
    fflush(stderr);

    // 初始化音频信息
    if (initAudioInfo() < 0) goto end;

    // 初始化视频信息
    if (initVideoInfo() < 0) goto end;

    // 从输入文件中读取数据
    AVPacket pkt;
    while (av_read_frame(_fmtCtx, &pkt) == 0)
    {
        if (pkt.stream_index == _aStream->index)
        {
            //
        }
        else if (pkt.stream_index == _vStream->index)
        {
            //
        }
        else
        {
            //
        }
    }

end:
    avcodec_free_context(&_aDecodeCtx);
    avcodec_free_context(&_vDecodeCtx);
    avformat_close_input(&_fmtCtx);

}

// 初始化解码器
int VideoPlayer::initDecoder(AVCodecContext **decodeCtx, AVStream **stream, AVMediaType type)
{
    // 根据 type 寻找合适的流信息, 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流
    int streamIdx = ret;
    *stream = _fmtCtx->streams[streamIdx];
    if (!stream)
    {
        qDebug() << "stream is empty";
        return -1;
    }

    // 为当前流找到合适的解码器
    const AVCodec *decoder = avcodec_find_decoder((*stream)->codecpar->codec_id);
    if (!decoder)
    {
        qDebug() << "decoder not found" << ((*stream)->codecpar->codec_id);
        return -1;
    }

    // 初始化解码上下文
    *decodeCtx = avcodec_alloc_context3(decoder);
    if (!decodeCtx) {
        qDebug() << "解码器上下文初始化失败";
        return -1;
    }

    // 从流中拷贝参数到解码上下文中
    ret = avcodec_parameters_to_context(*decodeCtx, (*stream)->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);

    return 0;
}

int VideoPlayer::initVideoInfo()
{
    // 初始化视频解码器
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);
    return 0;
}

int VideoPlayer::initAudioInfo()
{
    // 初始化音频解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);
    return 0;
}
