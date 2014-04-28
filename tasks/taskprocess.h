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

#ifndef TASKPROCESS_H
#define TASKPROCESS_H

#include <QThread>
#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QWebPage>
#include <QWebView>
#include <QWebFrame>
#include "misc/global.h"
#include "core/metadata.h"
#include "core/person.h"

typedef QPair<QString, QString> QProcessOutput;

class TaskProcessData {
public:
    bool            needCompleteScan;
    TaskProcessType type;
    QString         command, workingDirectory;
    QStringList     arguments;
    Metadata       *metadata;
    qint16          version;
public:
    QString         resultCommand;
    QStringList     resultOutput;

public:
    explicit TaskProcessData() {}
    explicit TaskProcessData(Metadata *_metadata, TaskProcessType _type, qint16 _version, bool _needCompleteScan) {
        metadata         = _metadata;
        type             = _type;
        version          = _version;
        needCompleteScan = _needCompleteScan;
    }
    explicit TaskProcessData(const QString &_command, const QString &_workingDirectory, const QStringList &_arguments) {
        type             = TaskProcessTypeProcess;
        command          = _command;
        workingDirectory = _workingDirectory;
        arguments        = _arguments;
    }
    void parseOutput(const QProcessOutput &result);
};


class TaskProcess : public QThread, public QTreeWidgetItem {
    Q_OBJECT
public:
    explicit TaskProcess(const TaskProcessData &_data, QTreeWidgetItem *parentItem, QObject *parent = 0);

public:
    void init();

private:
    QFileInfo       file;
public:
    TaskProcessData document;
    QString         name, thumbFilepath, thumbFilename;
    bool            started;
private:
    void run();
public:
    static QProcessOutput launchCommand(const TaskProcessData &processData);
    void sendFinishedSignal();

signals:
    void finished  (TaskProcess*);
    void updateList(TaskProcess*, int);
    void updateList(TaskProcess*, const QString &);
    void analyseWebContent(TaskProcess*);
};

#endif // TASKPROCESS_H
