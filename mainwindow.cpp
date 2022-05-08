#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _player = new VideoPlayer;

    // 信号 & 槽函数
    // 播放器初始化完成
    connect(_player, &VideoPlayer::initFinished, this, &MainWindow::onPlayerInitFinished);
    // 播放器状态变化
    connect(_player, &VideoPlayer::stateChanged, this, &MainWindow::onPlayerStateChanged);
    // 播放器发生错误时
    connect(_player, &VideoPlayer::playFailed, this, &MainWindow::onPlayerErrorOccurred);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _player;
}

void MainWindow::onPlayerInitFinished(VideoPlayer *player)
{
    int64_t duration = player->getDuration();

    // 设置视频滑块范围
    ui->videoProcessSlider->setRange(0, duration);

    // 设置总时长
    ui->lTotalTs->setText(getTimeText(duration));
}

void MainWindow::onPlayerStateChanged(VideoPlayer *player)
{
    VideoPlayer::State state = player->getState();

    // 若是正在播放
    if (state == VideoPlayer::Playing)
    {
        ui->startBtn->setText("暂停");
    }
    else
    {
        ui->startBtn->setText("播放");
    }

   if (state == VideoPlayer::Stopped)
   {
       ui->startBtn->setEnabled(false);
       ui->stopBtn->setEnabled(false);
       ui->silenceBtn->setEnabled(false);
       ui->videoProcessSlider->setEnabled(false);
       ui->videoProcessSlider->setValue(0);
       ui->lCurTs->setText(getTimeText(0));
       ui->lTotalTs->setText(getTimeText(0));
   }
   else
   {
       ui->startBtn->setEnabled(true);
       ui->stopBtn->setEnabled(true);
       ui->silenceBtn->setEnabled(true);
       ui->videoProcessSlider->setEnabled(true);
   }
}

void MainWindow::onPlayerErrorOccurred(VideoPlayer *player)
{
    QMessageBox::critical(nullptr, "提示", "播放出错");
}

void MainWindow::on_openFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                nullptr,
                "选择多媒体文件",
                "/Users/luoxiao/FFMpeg/data/video",
                "视频文件 (*.mp4 *.avi *.mkv);;"
                "音频文件 (*.mp3 *.aac *.wav)");

    qDebug() << filename;

    if (filename.isEmpty()) return;

    _player->setFilename(filename);
    _player->play();
}

void MainWindow::on_stopBtn_clicked()
{
    _player->stop();
}

void MainWindow::on_startBtn_clicked()
{
    VideoPlayer::State state = _player->getState();

    if (state == VideoPlayer::Playing)
    {
        _player->pause();
    }
    else
    {
        _player->play();
    }
}

// 获取时间字符串
QString MainWindow::getTimeText(int value)
{
    QLatin1Char fill = QLatin1Char('0');
    return QString("%1:%2:%3")
            .arg(value / 3600, 2 ,10, fill)
            .arg((value / 60) % 60, 2 ,10, fill)
            .arg(value % 60, 2 ,10, fill);
}

void MainWindow::on_videoProcessSlider_valueChanged(int value)
{
    ui->lCurTs->setText(getTimeText(value));
}
