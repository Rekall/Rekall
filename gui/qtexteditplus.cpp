#include "qtexteditplus.h"

QTextEditPlus::QTextEditPlus(QWidget *parent) :
    QLineEdit(parent) {
}

void QTextEditPlus::focusInEvent(QFocusEvent *e) {
    emit(focusChanged(true));
    QLineEdit::focusInEvent(e);
}
void QTextEditPlus::focusOutEvent(QFocusEvent *e) {
    emit(focusChanged(false));
    QLineEdit::focusOutEvent(e);
}
