#include "playervideo.h"
#include "ui_playervideo.h"

PlayerVideo::PlayerVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerVideo) {
    ui->setupUi(this);
    ui->playerVideo->setVolume(0);
    ui->volume    ->setVisible(false);
    ui->volumeLow ->setVisible(false);
    ui->volumeHigh->setVisible(false);
    window = 0;
    volumeOld = 0;
    setVolume(1, 1);
}

PlayerVideo::~PlayerVideo() {
    delete ui;
}

void PlayerVideo::load(Nameable *_tag, bool _isVideo, const QString &filename) {
    tag     = _tag;
    isVideo = _isVideo;
    if(!isVideo) {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/res_icon_audio_off.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/icons/res_icon_audio_on.png"),  QSize(), QIcon::Normal, QIcon::On);
        ui->showVideo->setIcon(icon);
    }
    ui->playerVideo->load(MediaSource(filename));
    refreshControls();
}
void PlayerVideo::play() {
    refreshControls();
    ui->playerVideo->play();
}
void PlayerVideo::pause() {
    refreshControls();
    ui->playerVideo->pause();
}
void PlayerVideo::seek(qint64 time) {
    refreshControls();
    ui->playerVideo->seek(time);
}
bool PlayerVideo::isDisplayed() const {
    return ui->showVideo->isChecked();
}
qint64 PlayerVideo::currentTime() {
    return ui->playerVideo->currentTime();
}
qint64 PlayerVideo::totalTime() {
    return ui->playerVideo->totalTime();
}
void PlayerVideo::setVolume(qreal _volume, qreal _masterVolume) {
    if(_masterVolume >= 0)
        masterVolume = _masterVolume;

    if(_volume >= 0) {
        if(_volume > 0.1)
            volumeOld = volume;
        volume = _volume;
        if(sender() != ui->volume)
            ui->volume->setValue(ui->volume->maximum() * volume);
        if(volume == 0) ui->volumeLow->setChecked(true);
        else            ui->volumeLow->setChecked(false);
    }
    ui->playerVideo->setVolume(volume * masterVolume);
}
void PlayerVideo::action() {
    if(sender() == ui->volumeLow) {
        if(!ui->volumeLow->isChecked())     setVolume(volumeOld);
        else                                setVolume(0);
    }
    else if(sender() == ui->volumeHigh)     setVolume(1);
    else if(sender() == ui->volume)         setVolume((qreal)ui->volume->value() / (qreal)ui->volume->maximum());
    else if(sender() == ui->title)          ui->showVideo->setChecked(!ui->showVideo->isChecked());
    else if((sender() == ui->showVideo) && (window)) {
        window->forceResizeEvent();
        if(ui->showVideo->isChecked()) {
            ui->volumeLow->setChecked(false);
            setVolume(volumeOld);
        }
        else {
            ui->volumeLow->setChecked(true);
            setVolume(0);
        }
        if(ui->showVideo->isChecked())  ui->title->setStyleSheet("");
        else                            ui->title->setStyleSheet("color: rgb(100, 100, 100);");
        ui->playerVideo->setVisible(ui->showVideo->isChecked());
    }
    else if((sender() == ui->playerVideo) && (window))
        window->globalPlayPause();
}

void PlayerVideo::enterEvent(QEvent *) {
    ui->volume    ->setVisible(true);
    ui->volumeLow ->setVisible(true);
    ui->volumeHigh->setVisible(true);
}
void PlayerVideo::leaveEvent(QEvent *) {
    ui->volume    ->setVisible(false);
    ui->volumeLow ->setVisible(false || (ui->volumeLow->isChecked() && ui->showVideo->isChecked()));
    ui->volumeHigh->setVisible(false);
}

void PlayerVideo::resizeEvent(QResizeEvent *) {
    refreshControls();
    ui->globalFramePlayer->resize(QSize(width(), ui->globalFramePlayer->height()));
    ui->playerVideo      ->resize(QSize(width(), height() - ui->globalFramePlayer->height()));
    ui->globalFramePlayer->move(ui->playerVideo->geometry().bottomLeft());
}

void PlayerVideo::refreshControls() {
    if(tag)
        ui->title->setText(tag->getTitle());
}
