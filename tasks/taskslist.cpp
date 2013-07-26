#include "taskslist.h"
#include "ui_taskslist.h"

qint16 TasksList::taskIsRunning = 0;

TasksList::TasksList(QWidget *parent) :
    QWidget(parent, Qt::Tool),
    ui(new Ui::TasksList) {
    ui->setupUi(this);
    hide();
    move(0, 0);
    taskIsRunning = 0;
}

TasksList::~TasksList() {
    delete ui;
}
void TasksList::addTask(Metadata *metadata, TaskProcessType type, qint16 version) {
    addTask(TaskProcessData(metadata, type, version));
}
void TasksList::addTask(const TaskProcessData &data) {
    if(data.metadata->file.exists()) {
        show();
        TaskProcess *task = new TaskProcess(data, ui->tasks->invisibleRootItem(), this);
        connect(task, SIGNAL(finished(TaskProcess*)), SLOT(finished(TaskProcess*)));
        connect(task, SIGNAL(updateList(TaskProcess*,int)), SLOT(updateList(TaskProcess*,int)));
        connect(task, SIGNAL(updateList(TaskProcess*,const QString &)), SLOT(updateList(TaskProcess*,const QString &)));
        tasks.append(task);
        task->init();
        nextTask();
    }
}

void TasksList::nextTask() {
    if(taskIsRunning < 5) {
        if(tasks.count()) {
            taskIsRunning++;
            foreach(TaskProcess *task, tasks)
                if(!task->taskStarted) {
                    task->start();
                    return;
                }
        }
        else
            hide();
    }
}

void TasksList::updateList(TaskProcess *task, int feedAction) {
    //Global::feedList->addFeed(new FeedItemBase((FeedItemBaseType)feedAction, "Rekall", task->processedDocument.metadata->file.baseName()));
}
void TasksList::updateList(TaskProcess *task, const QString &message) {
    task->setText(0, message);
}

void TasksList::finished(TaskProcess *task) {
    tasks.removeOne(task);
    ui->tasks->invisibleRootItem()->removeChild(task);
    taskIsRunning--;
    nextTask();
}
