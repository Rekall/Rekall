#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player) {
    ui->setupUi(this);
    Global::video = this;
}

Player::~Player() {
    delete ui;
}

void Player::load(void *_tag) {
    Tag *tag = (Tag*)_tag;
    if(!tags.contains(tag)) {
        if(tag->videoPlayer)
            tags.append(tag);
        ui->video->addWidget(tag->videoPlayer);
        tag->videoPlayer->setVisible(true);
        play(Global::timerPlay);
        seek(Global::time);
        qDebug("%s, %d %d", qPrintable(tag->getDocument()->getMetadata("Rekall", "Document Name").toString()), tag->videoPlayer->width(), tag->videoPlayer->height());
    }
}
void Player::unload(void *_tag) {
    Tag *tag = (Tag*)_tag;
    if(tags.contains(tag)) {
        tags.removeOne(tag);
        tag->videoPlayer->pause();
        tag->videoPlayer->setVisible(false);
        ui->video->removeWidget(tag->videoPlayer);
        tag->videoPlayer->setParent(0);
    }
}


void Player::seek(qreal time) {
    foreach(Tag *tag, tags)
        tag->videoPlayer->seek(qBound(0., (tag->getDocument()->getMetadata("Rekall", "Media Offset").toDouble() + time - tag->timeStart) * 1000., (qreal)tag->videoPlayer->totalTime()));
}
void Player::play(bool state) {
    foreach(Tag *tag, tags) {
        if(tag->contains(Global::time)) {
            if(state)   tag->videoPlayer->play();
            else        tag->videoPlayer->pause();
        }
    }
}


qreal Player::getCurrentTime() const {
    if(tags.count())
        return tags.first()->videoPlayer->currentTime() / 1000.;
    else
        return 0;
}
