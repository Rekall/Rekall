/*
    This file is part of Rekall.
    Copyright (C) 2013-2014

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    bool counterTimeout;
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
    QString launchedApplicationBeforePopup;
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
