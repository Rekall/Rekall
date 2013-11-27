#include "previewer.h"
#include "ui_previewer.h"

Previewer::Previewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Previewer) {
    ui->setupUi(this);
    volumeOld = 0;
    isUpdating = false;
    setVolume(0.5);
    startTimer(500);
}

Previewer::~Previewer() {
    delete ui;
}
void Previewer::timerEvent(QTimerEvent *) {
    if(ui->type->currentIndex() == 2) {
        isUpdating = true;
        ui->seek->setValue(ui->playerVideo->currentTime()/1000);
        ui->time->setText(QString("%1\n%2").arg(Global::timeToString(ui->playerVideo->currentTime()/1000)).arg(Global::timeToString(ui->playerVideo->totalTime()/1000)));
        isUpdating = false;
    }
}

void Previewer::resizeEvent(QResizeEvent *) {
    if((picture.size().width() > 0) && (picture.size().height() > 0)) {
        QSizeF targetSize = QSizeF(QSizeF(picture.size()) / (qreal)qMax(picture.width(), picture.height()) * (qreal)qMax(ui->type->width(), ui->type->height()));
        qreal scaleDepassement = qMax(targetSize.width() / (qreal)ui->type->width(), targetSize.height() / (qreal)ui->type->height());
        if(scaleDepassement > 1)
            targetSize = QSizeF(targetSize / scaleDepassement);
        ui->picture->resize(QSize(targetSize.width() - 20, targetSize.height() - 20));
        ui->picture->move(ui->type->width() / 2 - ui->picture->width() / 2, ui->type->height() / 2 - ui->picture->height() / 2);
    }
}
void Previewer::preview(int documentType, const QString &_filename, const QPixmap &_picture) {
    filename = _filename;
    ui->playerVideo->pause();

    if((documentType == 1) || (documentType == 2)) {
        if(documentType == 1)   ui->playerVideo->setVisible(true);
        else                    ui->playerVideo->setVisible(false);
        ui->type->setCurrentIndex(2);
        ui->picture->setVisible(false);
        ui->playPause->setChecked(false);
        ui->seek->setValue(0);
        ui->playerVideo->load(MediaSource(filename));
        ui->seek->setMaximum(ui->playerVideo->totalTime()/1000);
    }
    else if(!_picture.isNull()) {
        picture = _picture;
        ui->picture->setPixmap(picture);
        ui->picture->setVisible(true);
        ui->type->setCurrentIndex(1);
        ui->playerVideo->setVisible(false);
    }
    else {
        ui->type->setCurrentIndex(0);
        ui->picture->setVisible(false);
        ui->playerVideo->setVisible(false);
    }

    resizeEvent(0);
}

void Previewer::setVolume(qreal volume) {
    if(volume > 0.1)
        volumeOld = ui->playerVideo->volume();
    ui->playerVideo->setVolume(volume);
    if(sender() != ui->volume)
        ui->volume->setValue(ui->volume->maximum() * volume);
    if(volume == 0) ui->volumeLow->setChecked(true);
    else            ui->volumeLow->setChecked(false);
}
void Previewer::action() {
    if(sender() == ui->volumeLow) {
        if(!ui->volumeLow->isChecked())     setVolume(volumeOld);
        else                                setVolume(0);
    }
    else if(sender() == ui->volumeHigh)     setVolume(1);
    else if(sender() == ui->volume)         setVolume((qreal)ui->volume->value() / (qreal)ui->volume->maximum());
    else if(sender() == ui->playPause) {
        if(ui->playPause->isChecked())      ui->playerVideo->play();
        else                                ui->playerVideo->pause();
    }
    else if(sender() == ui->playerVideo) {
        ui->playPause->setChecked(!ui->playPause->isChecked());
    }
    else if((sender() == ui->seek) && (!isUpdating)) {
        ui->playerVideo->seek(ui->seek->value()*1000);
        ui->time->setText(QString("%1\n%2").arg(Global::timeToString(ui->playerVideo->currentTime()/1000)).arg(Global::timeToString(ui->playerVideo->totalTime()/1000)));
    }
}
void Previewer::actionOpen() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}
void Previewer::actionFinished() {
    ui->playPause->setChecked(false);
    ui->seek->setValue(0);
}
