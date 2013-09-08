#ifndef TASKPROCESS_H
#define TASKPROCESS_H

#include <QThread>
#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QTreeWidgetItem>
#include "misc/global.h"
#include "core/metadata.h"
#include "core/person.h"

typedef QPair<QString, QString> QProcessOutput;

class TaskProcessData {
public:
    QString command, workingDirectory;
    QStringList arguments;
    Metadata *metadata;
    TaskProcessType type;
    qint16 version;
public:
    QString resultCommand;
    QStringList resultOutput;
public:
    void parseOutput(const QProcessOutput &result);

public:
    explicit TaskProcessData() {}
    explicit TaskProcessData(Metadata *_metadata, TaskProcessType _type, qint16 _version) {
        metadata         = _metadata;
        type             = _type;
        version          = _version;
    }
    explicit TaskProcessData(const QString &_command, const QString &_workingDirectory, const QStringList &_arguments) {
        type             = TaskProcessTypeProcess;
        command          = _command;
        workingDirectory = _workingDirectory;
        arguments        = _arguments;
    }
};


class TaskProcess : public QThread, public QTreeWidgetItem {
    Q_OBJECT
public:
    explicit TaskProcess(const TaskProcessData &_data, QTreeWidgetItem *parentItem, QObject *parent = 0);
    void init();

public:
    TaskProcessData processedDocument;
    bool taskStarted;
public:
    static QProcessOutput launchCommand(const TaskProcessData &processData);

private:
    void run();

signals:
    void finished(TaskProcess*);
    void updateList(TaskProcess*, int);
    void updateList(TaskProcess*, const QString &);
};

#endif // TASKPROCESS_H