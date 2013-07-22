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

public:
    QList<TaskProcess*> tasks;
public:
    void addTask(Metadata *metadata, TaskProcessType type, qint16 version);
    void addTask(const TaskProcessData &data);
    static qint16 taskIsRunning;
private:
    void nextTask();

public slots:
    void finished(TaskProcess*);

private:
    Ui::TasksList *ui;
};

#endif // TASKSLIST_H
