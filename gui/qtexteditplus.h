#ifndef QTEXTEDITPLUS_H
#define QTEXTEDITPLUS_H

#include <QTextEdit>

class QTextEditPlus : public QTextEdit {
    Q_OBJECT
public:
    explicit QTextEditPlus(QWidget *parent = 0);

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

signals:
    void focusChanged(bool);

public slots:

};

#endif // QTEXTEDITPLUS_H
