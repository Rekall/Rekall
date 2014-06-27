#ifndef WATCHERSNAPSHOT_H
#define WATCHERSNAPSHOT_H

#include <QWidget>
#include <QDesktopWidget>
#include <QPixmap>
#include "misc/global.h"
#include "core/project.h"

namespace Ui {
class WatcherSnapshot;
}

class WatcherSnapshot : public QWidget {
    Q_OBJECT

public:
    explicit WatcherSnapshot(Metadata *_document, qint16 _version, QWidget *parent = 0);
    ~WatcherSnapshot();

protected:
    void timerEvent(QTimerEvent *);

public slots:
    void action();
private:
    int timer;
    qint16 version;
    Metadata *document;

private:
    Ui::WatcherSnapshot *ui;
};

#endif // WATCHERSNAPSHOT_H
