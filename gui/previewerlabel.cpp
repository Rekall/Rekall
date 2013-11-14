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
