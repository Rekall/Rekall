/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

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

#ifndef WATCHERLOCAL_H
#define WATCHERLOCAL_H

#include <QCoreApplication>
#include "../global.h"
#include "../watcher/folderwatcher.h"

class WatcherLocal : public WatcherInterface {
    Q_OBJECT

public:
    explicit WatcherLocal(ProjectInterface *parent = 0);

public:
    WatcherInterface *monitor(const QFileInfo &path);

public:
    void start();
    QFileInfo monitoredPath;
private:
    Mirall::FolderWatcher *watcher;
private:
    const QFileInfoList scanFolder(SyncEntry *path, bool returnsOnly = false);

private slots:
    void directoryChanged(const QString &path);
};

#endif // WATCHERLOCAL_H
