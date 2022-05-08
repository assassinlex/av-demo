#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _player = new VideoPlayer;
    connect(_player, &VideoPlayer::stateChanged, this, &MainWindow::onPlayerStateChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
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
       ui->lCurTs->setText("00:00:00");
       ui->lTotalTs->setText("00:00:00");
   }
   else
   {
       ui->startBtn->setEnabled(true);
       ui->stopBtn->setEnabled(true);
       ui->silenceBtn->setEnabled(true);
       ui->videoProcessSlider->setEnabled(true);
   }
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
