#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player) {
    ui->setupUi(this);
    forceResizeEvent();
    Global::video = this;

    timeMode   = true;
    isUpdating = false;
    volumeMasterOld = volumeMaster = 0;
    setVolume(0.5);
    startTimer(200);
}

Player::~Player() {
    delete ui;
}

void Player::load(void *_tag) {
    Tag *tag = (Tag*)_tag;
    if(!tags.contains(tag)) {
        if(tag->player)
            tags.append(tag);
        tag->player->setParent(ui->globalFramePlayer);
        tag->player->window = this;
        tag->player->setVisible(true);
        play(Global::timerPlay);
        seek(Global::time);
        resizeEvent(0);
    }
}
void Player::unload(void *_tag) {
    Tag *tag = (Tag*)_tag;
    if(tags.contains(tag)) {
        tags.removeOne(tag);
        tag->player->pause();
        tag->player->setVisible(false);
        tag->player->setParent(0);
        resizeEvent(0);
    }
}

void Player::timerEvent(QTimerEvent *) {
    isUpdating = true;
    if(Global::timeline) {
        ui->seek->setMaximum(Global::currentProject->totalTime());
        ui->seek->setValue(Global::time);
        ui->playPause->setChecked(Global::timerPlay);
        if(timeMode)    ui->time->setText(QString("%1\n%2") .arg(Sorting::timeToString(Global::time)).arg(Sorting::timeToString(Global::currentProject->totalTime())));
        else            ui->time->setText(QString("%1\n-%2").arg(Sorting::timeToString(Global::time)).arg(Sorting::timeToString(Global::currentProject->totalTime() - Global::time)));
    }
    isUpdating = false;
}
void Player::setVolume(qreal _volumeMaster) {
    if(_volumeMaster > 0.1)
        volumeMasterOld = volumeMaster;
    volumeMaster = _volumeMaster;

    foreach(Tag *tag, tags)
        tag->player->setVolume(-1, volumeMaster);

    if(sender() != ui->volume)
        ui->volume->setValue(ui->volume->maximum() * volumeMaster);
    if(volumeMaster == 0) ui->volumeLow->setChecked(true);
    else                  ui->volumeLow->setChecked(false);
}
void Player::action() {
    if((!isUpdating) && (Global::timeline)) {
        if(sender() == ui->volumeLow) {
            if(!ui->volumeLow->isChecked())     setVolume(volumeMasterOld);
            else                                setVolume(0);
        }
        else if(sender() == ui->volumeHigh)     setVolume(1);
        else if(sender() == ui->volume)         setVolume((qreal)ui->volume->value() / (qreal)ui->volume->maximum());
        else if(sender() == ui->playPause)
            Global::timeline->actionPlay();
        else if(sender() == ui->seek)
            Global::timeline->seek(ui->seek->value(), true, true);
        else if(sender() == ui->time)
            timeMode = !timeMode;
        else if(sender() == ui->rewind)
            Global::timeline->seek(0, true, true);
        timerEvent(0);
    }
}
void Player::actionDbl() {
    Global::timeline->jumpTo();
}



void Player::globalPlayPause() {
    Global::timeline->actionPlay();
}
void Player::forceResizeEvent() {
    QList<Tag*> videos, audios;

    //Player
    foreach(Tag *tag, tags)
        if(     (tag->player) && ( tag->player->isVideo) && (tag->player->isDisplayed()))
            videos << tag;
        else if (tag->player)
            audios << tag;

    //Space division
    QRect videoSpace(QPoint(0, 0), QSize(width(), height() - ui->globalFrameTransparent->height() - (qCeil((qreal)audios.count() / 2.) * 40)));
    QRect audioSpace(videoSpace.bottomLeft(), QSize(videoSpace.width(), height() - ui->globalFrameTransparent->height() - videoSpace.height()));

    //Distribution
    quint16 index = 0;
    foreach(Tag *video, videos) {
        QSize videoSize(videoSpace.width() / 2, videoSpace.height() / qCeil((qreal)videos.count() / 2));
        if(video->player)
            video->player->setGeometry(QRect(videoSpace.topLeft() + QPoint((index % 2) * videoSize.width(), qFloor(index / 2) * videoSize.height()), videoSize));
        index++;
    }
    index = 0;
    foreach(Tag *audio, audios) {
        QSize audioSize(audioSpace.width() / 2, audioSpace.height() / qCeil((qreal)audios.count() / 2));
        if(audio->player)
            audio->player->setGeometry(QRect(audioSpace.topLeft() + QPoint((index % 2) * audioSize.width(), qFloor(index / 2) * audioSize.height()), audioSize));
        index++;
    }

    ui->globalFrameTransparent->setGeometry(QRect(QPoint(0, height() - ui->globalFrameTransparent->height()), QSize(width(), ui->globalFrameTransparent->height())));
}
void Player::resizeEvent(QResizeEvent *) {
    forceResizeEvent();
}

void Player::seek(qreal time) {
    foreach(Tag *tag, tags)
        tag->player->seek(qBound(0., (tag->getDocument()->getMetadata("Rekall", "Media Offset").toDouble() + time - tag->getTimeStart()) * 1000., (qreal)tag->player->totalTime()));
}
void Player::play(bool state) {
    foreach(Tag *tag, tags) {
        if(tag->contains(Global::time)) {
            if(state)   tag->player->play();
            else        tag->player->pause();
        }
    }
}


qreal Player::getCurrentTime() const {
    if(tags.count())    return tags.first()->player->currentTime() / 1000.;
    else                return 0;
}
