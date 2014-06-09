#include "qtexteditplus.h"

QTextEditPlus::QTextEditPlus(QWidget *parent) :
    QTextEdit(parent) {
}

void QTextEditPlus::focusInEvent(QFocusEvent *e) {
    emit(focusChanged(true));
    QTextEdit::focusInEvent(e);
}
void QTextEditPlus::focusOutEvent(QFocusEvent *e) {
    emit(focusChanged(false));
    QTextEdit::focusOutEvent(e);
}
