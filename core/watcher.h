#ifndef WATCHER_H
#define WATCHER_H

#include <QObject>
#include <QMainWindow>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include "watcherfeeling.h"
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

class Watcher : public WatcherBase {
    Q_OBJECT

public:
    explicit Watcher(QObject *parent = 0);

private:
    WatcherFeeling *feeling;
    QStringList watcherTracking;
    QIcon trayIconOff, trayIconOn;
    QSystemTrayIcon *trayMenu;
    QPixmap lastScreenshot;
    QDateTime lastScreenshotTimestamp;

public:
    void sync  (const QString &file, bool inTracker = false);
    void unsync(const QString &file, bool inTracker = false);

public slots:
    void fileWatcherDirChanged(QString);
    void fileWatcherFileChanged(QString);
    void takeTemporarySnapshot();
    void trayActivateApp();
    void writeNote();
    void trayIconToOn(Document *document = 0);
    void trayIconToOff();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // WATCHER_H
