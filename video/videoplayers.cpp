#include "videoplayers.h"
#include "ui_videoplayers.h"

VideoPlayers::VideoPlayers(QWidget *parent) :
    QWidget(parent, Qt::Tool),
    ui(new Ui::VideoPlayers) {
    ui->setupUi(this);

    firstOpening = true;
}

VideoPlayers::~VideoPlayers() {
    delete ui;
}

void VideoPlayers::update(const QUrl &url, bool askClose, const QString &title, qint64 timecode) {
    show();
    if(firstOpening)
        restoreGeometry(QSettings().value("VideoPlayersGeometry").toByteArray());
    firstOpening = false;

    VideoPlayer *player = 0;
    foreach(VideoPlayerInterface *videoPlayer, players) {
        if(videoPlayer->currentUrl.toString() == url.toString())
            player = (VideoPlayer*)videoPlayer;
    }

    //Création si manquant
    if((!askClose) && (!player)) {
        player = new VideoPlayer(this);
        ui->layout->addWidget(player, qFloor(players.count()/2), (players.count()%2));
        qDebug("%d", players.count());

        connect(this, SIGNAL(videoPause ()      ), player, SLOT(pause()));
        connect(this, SIGNAL(videoPlay  (qint64)), player, SLOT(play(qint64)));
        connect(this, SIGNAL(videoRewind(qint64)), player, SLOT(rewind(qint64)));
        connect(this, SIGNAL(videoSeek  (qint64)), player, SLOT(seek(qint64)));

        players << player;
    }
    player->setUrl(url, askClose, title);
    show();

    if((askClose) && (player))
        players.removeOne(player);
    else if(timecode >= 0)
        player->seek(timecode);
}

void VideoPlayers::rewind(qint64 timecode) {
    emit videoRewind(timecode);
}
void VideoPlayers::seek(qint64 timecode) {
    emit videoSeek(timecode);
}
void VideoPlayers::play(qint64 timecode) {
    emit videoPlay(timecode);
}
void VideoPlayers::pause() {
    emit videoPause();
}

void VideoPlayers::forceClose() {
    close();
}

void VideoPlayers::closeEvent(QCloseEvent *) {
    Global::userInfos->setDockIcon(this, false);
    QSettings().setValue("VideoPlayersGeometry", saveGeometry());
}
