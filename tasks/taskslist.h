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

#ifndef TASKSLIST_H
#define TASKSLIST_H

#include <QWidget>
#include <QDomElement>
#include "taskprocess.h"

namespace Ui {
class TasksList;
}

class TasksList : public QWidget, public TaskListBase {
    Q_OBJECT
    
public:
    explicit TasksList(QWidget *parent = 0);
    ~TasksList();

private:
    QList<TaskProcess*>  tasks;
    QToolBox            *toolbox;
    QWebView            *webView;
    TaskProcess         *webViewTask;
    static qint16        runningTasks, runningWebTasks;
    quint16              oldToolboxIndex;
public:
    void setToolbox(QToolBox *);
    void addTask   (Metadata *metadata, TaskProcessType type, qint16 version, bool needCompleteScan);
    void addTask   (const TaskProcessData &data);
private slots:
    void webPageLoaded();
private:
    void nextTask();
    void webPageLoadedEnd();

public slots:
    void analyseWebContent(TaskProcess*);
    void finished(TaskProcess*);
    void updateList(TaskProcess*, int);
    void updateList(TaskProcess*, const QString &);

private:
    Ui::TasksList *ui;
};

#endif // TASKSLIST_H
