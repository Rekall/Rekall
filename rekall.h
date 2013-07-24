#ifndef REKALL_H
#define REKALL_H

#include <QMainWindow>
#include <QTimer>
#include <QClipboard>
#include "core/watcher.h"
#include "interfaces/userinfos.h"
#include "gui/inspector.h"
#include "items/uifileitem.h"
#include "gui/splash.h"
#include "gui/timeline.h"
#include "tasks/taskslist.h"
#include "tasks/feedlist.h"
#include "interfaces/http/httplistener.h"
#include "interfaces/fileuploadcontroller.h"


namespace Ui {
class Rekall;
}

class Rekall : public QMainWindow {
    Q_OBJECT
    
public:
    explicit Rekall(QWidget *parent = 0);
    ~Rekall();

private:
    Splash *splash;
    Project *currentProject;
    Inspector *inspector;
    bool metaIsChanging, openProject;
    Document *currentDocument;
    HttpListener *http;
    FileUploadController *httpUpload;
    QSettings *settings;

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    bool parseMimeData(const QMimeData *mime, bool test = false);

private slots:
    void fileUploaded(QString gps, QString filename, QTemporaryFile*);
    void action();
    void actionMetadata(QTreeWidgetItem *item, int col);
    void closeSplash();
    void displayMetadata();
    void refreshAndLastMetadata();
    void refreshMetadata();
    void refreshPerson();
    void chutierItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void showInspector();

private:
    qint32 updateUserInfos;
protected:
    void timerEvent(QTimerEvent *);

private:
    Ui::Rekall *ui;
};

#endif // REKALL_H
