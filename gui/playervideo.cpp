#include "playervideo.h"
#include "ui_playervideo.h"

PlayerVideo::PlayerVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerVideo) {
    ui->setupUi(this);
    ui->playerVideo->setVolume(0);
}

PlayerVideo::~PlayerVideo() {
    delete ui;
}

void PlayerVideo::load(const QString &filename) {
    ui->playerVideo->load(MediaSource(filename));
}
void PlayerVideo::play() {
    ui->playerVideo->play();
}
void PlayerVideo::pause() {
    ui->playerVideo->pause();
}
void PlayerVideo::seek(qint64 time) {
    ui->playerVideo->seek(time);
}
qint64 PlayerVideo::currentTime() {
    return ui->playerVideo->currentTime();
}
qint64 PlayerVideo::totalTime() {
    return ui->playerVideo->totalTime();
}
void PlayerVideo::setVolume(qreal volume) {
    ui->playerVideo->setVolume(volume);
}
void PlayerVideo::resizeEvent(QResizeEvent *) {
    ui->playerVideo->resize(size());
    //ui->mute->setGeometry(QRect(QPoint(width() / 2 - ui->mute->width() / 2, height() / 2 - ui->mute->height() / 2), ui->mute->size()));
}
