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

#ifndef ANALYSE_H
#define ANALYSE_H

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QCoreApplication>
#include "../global.h"

class AnalyseProcess;
class Analyse : public QThread, public AnalyseInterface {
    Q_OBJECT
public:
    explicit Analyse(QObject *parent = 0);
    ~Analyse();

public:
    void addToQueue(SyncEntry *file, ProjectInterface *project);
    void stop();

public:
    void run();

private:
    QDateTime lastAnalyse;
    qint64 lastFileAnalyzed;
    QQueue<AnalyseProcess*> metadataQueue, thumbnailQueue;
    bool okThread;
    quint16 thumbnailThreadsCount;
private slots:
    void threadFinished(AnalyseProcess *);

signals:
    void trayChanged(QString, bool);
    void trayIconToOn(qint16);
    void trayIconToOff();
};



class AnalyseProcess : public QThread {
    Q_OBJECT

public:
    bool noChanges, firstAnalysis;
    SyncEntry *file;
    ProjectInterface *project;
private:
    Metadatas previousMeta;

public:
    explicit AnalyseProcess(SyncEntry *_file, ProjectInterface *_project, QObject *parent = 0);
public:
    bool process();
    void run();
signals:
    void threadFinished(AnalyseProcess*);
    void projectChanged(SyncEntry*,bool);
public:
    void getMetadatas();
    void getThumbnails();
};

#endif // ANALYSE_H
