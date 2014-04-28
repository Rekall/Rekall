/*
    This file is part of Rekall.
    Copyright (C) 2013-2014

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef REKALL_H
#define REKALL_H

#include <QMainWindow>
#include <QTimer>
#include <QClipboard>
#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QDesktopServices>
#include <QInputDialog>
#include "core/watcher.h"
#include "interfaces/userinfos.h"
#include "interfaces/http/httplistener.h"
#include "interfaces/fileuploadcontroller.h"
#include "items/uifileitem.h"
#include "gui/splash.h"
#include "gui/timeline.h"
#include "tasks/taskslist.h"
#include "tasks/feedlist.h"


namespace Ui {
class Rekall;
}

class Rekall : public RekallBase {
    Q_OBJECT
    
public:
    explicit Rekall(QWidget *parent = 0);
    ~Rekall();

private:
    bool          metaIsChanging, openProject, chutierIsUpdating;
    Splash       *splash;
    Project      *currentProject;
    Document     *currentDocument;
    Metadata     *currentMetadata;
    QSettings    *settings;
    QWebView     *gps;
    QPair<QString, QPixmap> picture;
private:
    HttpListener         *http;
    FileUploadController *httpUpload;

private:
    void displayDocumentName(const QString &documentName = "");
    void displayPixmap(DocumentType documentType, const QPair<QString, QPixmap> &picture);
    void displayPixmap(DocumentType documentType, const QString &filename, const QPixmap &picture);
    void displayGps(const QList<QPair<QString, QString> > &gps);
public:
    void refreshMetadata(void *_tag, bool inChutier);
    void setVisbility(bool visibility);
    void showPreviewTab();

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    bool parseMimeData(const QMimeData *mime, const QString &source, bool test = false);

private slots:
    void fileUploaded(const QString &, const QString &, const QString &);
    void action();
    void actionForceGL();
    void actionMetadata();
    void actionMetadata(QTreeWidgetItem *item, int col);
    void closeSplash();
    void refreshAndLastMetadata();
    void refreshMetadata();
    void chutierItemChanged(QTreeWidgetItem*,QTreeWidgetItem*,Tag* = 0);
    void personItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
    void displayMetadata(Metadata *metadata, Tag *tag, QTreeWidget*, QTreeWidgetItem*, QTreeWidgetItem*);
    void showHelp(bool);

protected:
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::Rekall *ui;
};

#endif // REKALL_H
