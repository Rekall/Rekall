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

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include <QTextDocument>
#include <QTimer>
#include <QAction>
#include "../http/filecontroller.h"
#include "../global.h"
#include "watcherlocal.h"
#include "video/videoplayer.h"


class Project : public ProjectInterface {
    Q_OBJECT

public:
    explicit Project(const QString &_name, const QString &_friendlyName, bool _isPublic, const QFileInfo &_path, QObject *parent = 0);

private:
    QDomElement xmlProject;
    bool hasChanged, isLoaded;
    QTimer saveTimer;
public:
    static bool sortEventFunction(SyncEntryEvent *a, SyncEntryEvent *b);

private:
    QAction* addEvent(SyncEntryEvent *event);
    void addItemToMenu(QAction *action);
protected:
    void timerEvent(QTimerEvent *);

private:
    int timerLoadId;
public slots:
    void load(bool existing);
    void save();
    void openWebPage();
    void openFolder();
    void updateGUI();
    void videosRewind(qint64 timecode = 0);
    void videosPlay(qint64 timecode = -1);
    void videosPause();
    void updateVideo(const QUrl &url, bool askClose = false, const QString &title = "", qint64 timecode = 0);
    void fileChanged(SyncEntry *file);
    void projectChanged(SyncEntry *file, bool firstChange);
    void projectChanged(const QString &strChanges);
    void projectChanged();
signals:
    void videoSeek(qint64);
    void videoRewind(qint64);
    void videoPlay(qint64);
    void videoPause();
    void projectChangedLoopback();
};

#endif // PROJECT_H
