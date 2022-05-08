#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videoplayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayerStateChanged(VideoPlayer *player);

    void on_openFileBtn_clicked();

    void on_stopBtn_clicked();

    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
    VideoPlayer *_player;
};

#endif // MAINWINDOW_H
