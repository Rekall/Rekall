#include "previewerlabel.h"

PreviewerLabel::PreviewerLabel(QWidget *parent) :
    QLabel(parent) {
}

void PreviewerLabel::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerLabel::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}

PreviewerVideoPlayer::PreviewerVideoPlayer(QWidget *parent) :
    Phonon::VideoPlayer(parent) {
}

void PreviewerVideoPlayer::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerVideoPlayer::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}
