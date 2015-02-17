/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

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

#include <QWidget>
#include <QDialog>
#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <QWebView>
#include <QFileDialog>
#include <QMenuBar>
#include "webwrapper.h"
#include "video/videoplayer.h"
#include "http/http.h"
#include "core/analyse.h"
#include "core/userinfos.h"
#include "core/project.h"

class Rekall : public QDialog, public RekallInterface {
    Q_OBJECT

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *trayMenuProjects, *trayAnalyse;
    QList<QIcon> trayIcons;
    quint16 trayIconIndex, trayIconIndexOld;

private:
    QSettings *globalSettings;
    bool forceUpdate;
    QString updateAnonymousId;
    QNetworkAccessManager *updateManager;
private slots:
    void checkForUpdatesFinished(QNetworkReply*);
public:
    void checkForUpdates();


public:
    void syncSettings();
    void addProject(ProjectInterface *project);
    void removeProject(ProjectInterface *project);

private:
    QTimer trayTimer, trayTimerOff;
private slots:
    void trayIconToOnPrivate();
    void trayIconToOffPrivate();
public slots:
    void trayIconToOn(qint16 duration);
    void trayIconToOff();
    void analyseTrayChanged(QString,bool);

public slots:
    void openWebPage();
    void closeRekall();

public slots:
    void showMessage(const QString &message);
    void incomingMessage(const QString &, quint16, const QString &, const QList<QVariant> &);
    void addProject();
    void updateGUI();
    void trayActivated(QSystemTrayIcon::ActivationReason);

protected:
    void timerEvent(QTimerEvent *);
    void takeScreenshot();


public:
    explicit Rekall(const QStringList &arguments = QStringList(), QWidget *parent = 0);
    ~Rekall();
};

#endif // REKALL_H
