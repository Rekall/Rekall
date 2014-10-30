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

#include "project.h"



Project::Project(const QString &_name, const QString &_friendlyName, bool _isPublic, const QFileInfo &_path, QObject *parent) :
    ProjectInterface(parent) {
    name = _name;
    friendlyName = _friendlyName;
    path = _path;
    isPublic = _isPublic;
    hasChanged = false;

    QDir().mkpath(path.absoluteFilePath() + "/rekall_cache");

    QSettings* fileSettings = new QSettings(Global::configFileName, QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    fileController = new FileController(fileSettings, path.absoluteFilePath().toUtf8(), this, this);

    trayMenuTitle = new QAction(this);
    trayMenuTitle->setEnabled(false);
    trayMenuEvents = new QMenu(tr("Recent activity"));
    trayMenuEvents->setEnabled(false);

    trayMenuWeb = new QAction(tr("Open project"), this);
    connect(trayMenuWeb, SIGNAL(triggered()), SLOT(openWebPage()));

    trayMenuFolder = new QAction(tr("Show folder"), this);
    connect(trayMenuFolder, SIGNAL(triggered()), SLOT(openFolder()));

    sync = (new WatcherLocal(this))->monitor(path);
    connect(sync, SIGNAL(fileChanged(SyncEntry*)), SLOT(fileChanged(SyncEntry*)));

    connect(&saveTimer, SIGNAL(timeout()), SLOT(save()));
    saveTimer.setInterval(1000);
}

void Project::load() {
    xmlDoc = QDomDocument("rekall");
    QFile projectFile(path.absoluteFilePath() + "/rekall_cache/project.xml");
    if((projectFile.exists()) && (QFileInfo(projectFile.fileName()).size() > 0) && (projectFile.open(QFile::ReadOnly))) {
        xmlDoc.setContent(&projectFile);
        projectFile.close();

        xmlProject = xmlDoc.documentElement();
        QDomNode documentNode = xmlProject.firstChild();
        while(!documentNode.isNull()) {
            QDomElement documentElement = documentNode.toElement();
            QCoreApplication::processEvents();
            if((!documentElement.isNull()) && (documentElement.nodeName().toLower() == "document")) {
                Metadatas metadatas;
                metadatas.deserialize(documentElement);
                if((metadatas.contains("Rekall->Folder")) && (metadatas.contains("File->File Name"))) {
                    SyncEntry *file = new SyncEntry(path.absoluteFilePath() + "/" + metadatas["Rekall->Folder"] + "/" + metadatas["File->File Name"]);
                    file->metadatas = metadatas;
                    sync->folders[file->absolutePath()][file->absoluteFilePath()] = file;
                    qDebug("Loading metadatas for %s (%d)", qPrintable(file->absoluteFilePath()), file->metadatas.count());
                }
            }
            else if((!documentElement.isNull()) && (documentElement.nodeName().toLower() == "event")) {
                SyncEntryEvent *event = new SyncEntryEvent(documentElement, this);
                addEvent(event);
                qDebug("Loading event for %s", qPrintable(event->file.absoluteFilePath()));
            }
            documentNode = documentNode.nextSibling();
        }
    }
    else {
        xmlProject = xmlDoc.createElement("project");
        xmlDoc.appendChild(xmlProject);
    }

    updateGUI();
    sync->start();
}
void Project::save() {
    saveTimer.stop();
    if(hasChanged) {
        hasChanged = false;
        qDebug("Sauvegarde de %s", qPrintable(friendlyName));
        QFile projectFile(path.absoluteFilePath() + "/rekall_cache/project.xml");
        projectFile.open(QFile::WriteOnly);
        projectFile.write(xmlDoc.toByteArray(0));
        projectFile.close();
    }
}


void Project::openWebPage() {
    QDesktopServices::openUrl(QUrl(QString("http://%1:%2/%3").arg(Global::http->getLocalHost().ip).arg(Global::http->getPort()).arg(name)));
}
void Project::openFolder() {
    Global::revealInFinder(path);
}
void Project::updateGUI() {
    trayMenuTitle->setText(friendlyName);
    foreach(SyncEntryEvent *event, events)
        event->updateGUI();
    trayMenuEvents->setEnabled((trayMenuEvents->actions().count() > 0));
}

void Project::fileChanged(SyncEntry *file) {
    //qDebug("%d : %s (%d)", file->action, qPrintable(file->baseName()), file->isDir());
    //qDebug("\t%s the %s at %s @ %s", qPrintable(file->userInfos->getAuthor()), qPrintable(file->userInfos->getDateTime().toString()), qPrintable(file->userInfos->getLocationName()), qPrintable(file->userInfos->getLocationGPS()));
    if((file->action == SyncCreate) || (file->action == SyncUpdate)) {
        //Global::qSleep(40);
        Global::analyse->addToQueue(file, this);
    }
}

void Project::projectChanged(SyncEntry *file, bool firstChange) {
    if(!firstChange) {
        SyncEntryEvent *event = new SyncEntryEvent(file, this);
        addEvent(event);
        xmlProject.appendChild(event->serialize(xmlDoc));
    }
    xmlProject.appendChild(file->metadatas.serialize(xmlDoc));
    hasChanged = true;
    saveTimer.stop();
    saveTimer.start();
}
QAction* Project::addEvent(SyncEntryEvent *event) {
    events.prepend(event);
    qSort(events.begin(), events.end(), Project::sortEventFunction);
    addItemToMenu(event->trayProjectAction);
    return event->trayGlobalAction;
}
bool Project::sortEventFunction(SyncEntryEvent *a, SyncEntryEvent *b) {
    return a->dateTime > b->dateTime;
}

void Project::addItemToMenu(QAction *action) {
    trayMenuEvents->addAction(action);
    while(trayMenuEvents->actions().count() > 5)
        trayMenuEvents->removeAction(trayMenuEvents->actions().first());
    updateGUI();
}
