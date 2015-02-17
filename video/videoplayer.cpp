#include "videoplayer.h"
#include "ui_videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlayer) {
    ui->setupUi(this);

    player = new QMediaPlayer();
    player->setVideoOutput(ui->videoPlayer);
    //connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
}

VideoPlayer::~VideoPlayer() {
    delete ui;
}

void VideoPlayer::setUrl(const QUrl &url, bool askClose, const QString &title) {
    if((askClose) && (currentUrl.toString() == url.toString())) {
        close();
    }
    else if(!askClose) {
        if(currentUrl.toString() != url.toString()) {
            currentUrl = url;
            Global::userInfos->setDockIcon(true);
            player->setMedia(currentUrl);
        }

        if(title.isEmpty()) setWindowTitle(tr("Rekall — ") + currentUrl.fileName());
        else                setWindowTitle(title);
        show();
    }
}

void VideoPlayer::seek(qint64 timecode) {
    if(timecode >= 0)
        player->setPosition(timecode);
}
void VideoPlayer::play(qint64 timecode) {
    seek(timecode);
    player->play();
}
void VideoPlayer::pause() {
    player->pause();
}
void VideoPlayer::rewind(qint64 timecode) {
    player->pause();
    seek(timecode);
}

void VideoPlayer::closeEvent(QCloseEvent *) {
    //Global::userInfos->setDockIcon(false);
}
