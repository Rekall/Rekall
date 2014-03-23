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
