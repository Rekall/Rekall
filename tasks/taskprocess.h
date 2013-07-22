#ifndef TASKPROCESS_H
#define TASKPROCESS_H

#include <QThread>
#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QTreeWidgetItem>
#include "misc/global.h"
#include "core/metadata.h"

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
    void parseOutput(const QPair<QString,QString> &result);
    static QString getBetween(const QString &data, const QString &start, const QString &end, bool trim = true);
    static qreal getDurationFromString(const QString &timeStr);
    static QPair<QString,QString> seperateMetadata(const QString &metaline, const QString &separator = QString(":"));
    static QPair<QString, QPair<QString,QString> > seperateMetadataAndGroup(const QString &metaline, const QString &separator = QString(":"));

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

public:
    TaskProcessData processedDocument;
    bool taskStarted;
public:
    static QPair<QString,QString> launchCommand(const TaskProcessData &processData);

private:
    void run();
    void changeText(const QString &message);
    
signals:
    void finished(TaskProcess*);
};

#endif // TASKPROCESS_H
