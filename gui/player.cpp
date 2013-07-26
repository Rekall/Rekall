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
        if(tag->playerVideo)
            tags.append(tag);
        tag->playerVideo->setParent(ui->globalFrameBlk);

        tag->playerVideo->setVisible(true);
        play(Global::timerPlay);
        seek(Global::time);
        resizeEvent(0);
    }
}
void Player::unload(void *_tag) {
    Tag *tag = (Tag*)_tag;
    if(tags.contains(tag)) {
        tags.removeOne(tag);
        tag->playerVideo->pause();
        tag->playerVideo->setVisible(false);
        tag->playerVideo->setParent(0);
        resizeEvent(0);
    }
}


void Player::resizeEvent(QResizeEvent *) {
    quint16 videoNb = 0;

    foreach(Tag *tag, tags)
        if((tag->getDocument()->type == DocumentTypeVideo) || (tag->getDocument()->type == DocumentTypeImage))
            videoNb++;

    if(videoNb) {
        quint16 videoIndex = 0;
        QPoint start;
        QSize videoSize(size().width() / qMin(videoNb, (quint16)2), size().height() / qMax(1, qCeil((qreal)videoNb / 2.)));
        foreach(Tag *tag, tags) {
            if((tag->getDocument()->type == DocumentTypeVideo) || (tag->getDocument()->type == DocumentTypeImage)) {
                tag->playerVideo->setGeometry(QRect(start, videoSize));
                videoIndex++;
                if(videoIndex % 2 == 0) start += QPoint(-start.x(), videoSize.height());
                else                    start += QPoint(videoSize.width(), 0);
            }
        }
    }
}

void Player::seek(qreal time) {
    foreach(Tag *tag, tags)
        tag->playerVideo->seek(qBound(0., (tag->getDocument()->getMetadata("Rekall", "Media Offset").toDouble() + time - tag->timeStart) * 1000., (qreal)tag->playerVideo->totalTime()));
}
void Player::play(bool state) {
    foreach(Tag *tag, tags) {
        if(tag->contains(Global::time)) {
            if(state)   tag->playerVideo->play();
            else        tag->playerVideo->pause();
        }
    }
}


qreal Player::getCurrentTime() const {
    if(tags.count())    return tags.first()->playerVideo->currentTime() / 1000.;
    else                return 0;
}
