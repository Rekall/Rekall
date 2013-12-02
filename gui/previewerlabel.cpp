#include "previewerlabel.h"

PreviewerLabel::PreviewerLabel(QWidget *parent) :
    QLabel(parent) {
}

void PreviewerLabel::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerLabel::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerLabel::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}


PreviewerPushButton::PreviewerPushButton(QWidget *parent) :
    QPushButton(parent) {
}

void PreviewerPushButton::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerPushButton::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerPushButton::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}

PreviewerVideoPlayer::PreviewerVideoPlayer(QWidget *parent) :
    Phonon::VideoPlayer(parent) {
}

void PreviewerVideoPlayer::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerVideoPlayer::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerVideoPlayer::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}
