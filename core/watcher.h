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

#ifndef WATCHER_H
#define WATCHER_H

#include <QObject>
#include <QMainWindow>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include "watcherfeeling.h"
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

class Watcher : public WatcherBase {
    Q_OBJECT

public:
    explicit Watcher(QObject *parent = 0);

private:
    QIcon     trayIconOff, trayIconOn;
    QPixmap   lastScreenshot;
    QDateTime lastScreenshotTimestamp;
private:
    WatcherFeeling  *feeling;
    QStringList      watcherTracking;
    QSystemTrayIcon *trayMenu;

public:
    void sync  (const QString &file, bool inTracker = false);
    void unsync(const QString &file, bool inTracker = false);

public slots:
    void fileWatcherDirChanged (QString);
    void fileWatcherFileChanged(QString);
    void takeTemporarySnapshot();
    void trayActivateApp();
    void writeNote();
    void trayIconToOn(Document *document = 0);
    void trayIconToOff();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // WATCHER_H
