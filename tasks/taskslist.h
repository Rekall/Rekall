#ifndef TASKSLIST_H
#define TASKSLIST_H

#include <QWidget>
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
    quint16              oldToolboxIndex;
    static qint16        taskIsRunning;
public:
    void setToolbox(QToolBox *);
    void addTask   (Metadata *metadata, TaskProcessType type, qint16 version);
    void addTask   (const TaskProcessData &data);
private:
    void nextTask();

public slots:
    void finished(TaskProcess*);
    void updateList(TaskProcess*, int);
    void updateList(TaskProcess*, const QString &);

private:
    Ui::TasksList *ui;
};

#endif // TASKSLIST_H
