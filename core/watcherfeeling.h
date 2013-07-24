#ifndef WATCHERFEELING_H
#define WATCHERFEELING_H

#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QFileInfo>
#include "misc/global.h"
#include "core/project.h"

namespace Ui {
class WatcherFeeling;
}

class WatcherFeelingWaiter : public QPushButton {
public:
    explicit WatcherFeelingWaiter(QWidget *parent = 0);

private:
    quint16 counter;
    QString baseText;
    bool showCounter;
public:
    void resetCounter(const QString &_baseText, quint16 _counter = 0);
    void disableCounter();

protected:
    void timerEvent(QTimerEvent *e);
};

class WatcherFeeling : public QWidget {
    Q_OBJECT
    
public:
    explicit WatcherFeeling(QWidget *parent = 0);
    ~WatcherFeeling();

private:
    QString locationBase;
    QList<Document*> documents;
public:
    void display(Document *document);

protected:
    void changeEvent(QEvent *e);

public slots:
    void action();

private:
    Ui::WatcherFeeling *ui;
};

#endif // WATCHERFEELING_H
