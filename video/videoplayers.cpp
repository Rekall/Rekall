#include "videoplayers.h"
#include "ui_videoplayers.h"

VideoPlayers::VideoPlayers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlayers) {
    ui->setupUi(this);
}

VideoPlayers::~VideoPlayers() {
    delete ui;
}

void VideoPlayers::update(const QUrl &url, bool askClose, const QString &title, qint64 timecode) {
    VideoPlayerInterface *player = 0;
    foreach(VideoPlayerInterface *videoPlayer, players) {
        if(videoPlayer->currentUrl.toString() == url.toString())
            player = videoPlayer;
    }

    //Création si manquant
    if((!askClose) && (!player)) {
        VideoPlayer *playerP = new VideoPlayer();
        players << playerP;
        connect(this, SIGNAL(videoPause ()      ), playerP, SLOT(pause()));
        connect(this, SIGNAL(videoPlay  (qint64)), playerP, SLOT(play(qint64)));
        connect(this, SIGNAL(videoRewind(qint64)), playerP, SLOT(rewind(qint64)));
        connect(this, SIGNAL(videoSeek  (qint64)), playerP, SLOT(seek(qint64)));
        player = playerP;
    }
    player->setUrl(url, askClose, title);

    if((askClose) && (player))
        players.removeOne(player);
    else if(timecode >= 0)
        player->seek(timecode);
}

void VideoPlayers::rewind(qint64 timecode) {
    emit videoRewind(timecode);
}
void VideoPlayers::play(qint64 timecode) {
    emit videoPlay(timecode);
}
void VideoPlayers::pause() {
    emit videoPause();
}

