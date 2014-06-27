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

#ifndef WATCHERSNAPSHOT_H
#define WATCHERSNAPSHOT_H

#include <QWidget>
#include <QDesktopWidget>
#include <QPixmap>
#include "misc/global.h"
#include "core/project.h"

namespace Ui {
class WatcherSnapshot;
}

class WatcherSnapshot : public QWidget {
    Q_OBJECT

public:
    explicit WatcherSnapshot(Metadata *_document, qint16 _version, QWidget *parent = 0);
    ~WatcherSnapshot();

protected:
    void timerEvent(QTimerEvent *);

public slots:
    void action();
private:
    int timer;
    qint16 version;
    Metadata *document;

private:
    Ui::WatcherSnapshot *ui;
};

#endif // WATCHERSNAPSHOT_H
