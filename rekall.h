#ifndef REKALL_H
#define REKALL_H

#include <QMainWindow>
#include <QTimer>
#include <QClipboard>
#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QDesktopServices>
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

class Rekall : public RekallBase {
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
    QWebView *gps;
    QPair<QString, QPixmap> picture;
public:
    void setVisbility(bool show);

private slots:
public:
    void displayDocumentName(const QString &documentName = "");
    void displayPixmap(const QPair<QString, QPixmap> &_picture);
    void displayGps(const QList<QPair<QString, QString> > &gps);

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    bool parseMimeData(const QMimeData *mime, const QString &source, bool test = false);

private slots:
    void fileUploaded(QString gps, QString filename, QTemporaryFile*);
    void action();
    void actionMetadata(QTreeWidgetItem *item, int col);
    void closeSplash();
    void refreshAndLastMetadata();
    void refreshMetadata();
    void chutierItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void personItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void displayMetadata(Metadata *metadata, QTreeWidget*, QTreeWidgetItem*, QTreeWidgetItem*);
    void showInspector();

private:
    qint32 updateUserInfos;
protected:
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::Rekall *ui;
};

#endif // REKALL_H
