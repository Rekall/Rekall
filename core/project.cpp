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

#include "project.h"



Project::Project(const QString &_name, const QString &_friendlyName, bool _isPublic, const QFileInfo &_path, QObject *parent) :
    ProjectInterface(parent) {
    name = _name;
    friendlyName = _friendlyName;
    path = _path;
    isLoaded = false;
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

    connect(this, SIGNAL(projectChangedLoopback()), SLOT(projectChanged()));
    timerLoadId = -1;
}

void Project::load(bool existing) {
    if((!existing) || ((QFileInfo(path.absoluteFilePath()).exists()) && (!QFileInfo(path.absoluteFilePath() + "/rekall_cache/project.xml").exists()))) {
        isLoaded = true;
        xmlDoc = QDomDocument("rekall");
        xmlProject = xmlDoc.createElement("project");
        xmlDoc.appendChild(xmlProject);
        sync->start();
    }
    else {
        timerLoadId = startTimer(1000);
    }
    updateGUI();
}
void Project::timerEvent(QTimerEvent *e) {
    if(e->timerId() == timerLoadId) {
        if(!isLoaded) {
            QFile projectFile(path.absoluteFilePath() + "/rekall_cache/project.xml");
            if((projectFile.exists()) && (QFileInfo(projectFile.fileName()).size() > 0) && (projectFile.open(QFile::ReadOnly))) {
                isLoaded = true;

                xmlDoc = QDomDocument("rekall");
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

                updateGUI();
                sync->start();
            }
        }
    }
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
    Global::webWrapper->openWebPage(QUrl(QString("http://%1:%2/%3").arg(Global::http->getLocalHost().ip).arg(Global::http->getPort()).arg(name)), tr("Opening project %1…").arg(friendlyName));
}
void Project::openFolder() {
    Global::revealInFinder(path);
}
void Project::updateGUI() {
    if(isLoaded) {
        trayMenuTitle->setText(friendlyName);
        foreach(SyncEntryEvent *event, events)
            event->updateGUI();
        trayMenuEvents->setEnabled((trayMenuEvents->actions().count() > 0));
        trayMenuWeb->setEnabled(true);
        trayMenuFolder->setEnabled(true);
    }
    else {
        trayMenuTitle->setText(tr("Waiting for %1").arg(friendlyName));
        trayMenuEvents->setEnabled(false);
        trayMenuWeb->setEnabled(false);
        trayMenuFolder->setEnabled(false);
    }
}

void Project::videosRewind(qint64 timecode) {
    emit videoRewind(timecode);
}
void Project::videosPlay(qint64 timecode) {
    emit videoPlay(timecode);
}
void Project::videosPause() {
    emit videoPause();
}
void Project::updateVideo(const QUrl &url, bool askClose, const QString &title, qint64 timecode) {
    VideoPlayerInterface *player = 0;
    foreach(VideoPlayerInterface *videoPlayer, videoPlayers) {
        if(videoPlayer->currentUrl.toString() == url.toString())
            player = videoPlayer;
    }

    //Création si manquant
    if((!askClose) && (!player)) {
        VideoPlayer *playerP = new VideoPlayer();
        videoPlayers << playerP;
        connect(this, SIGNAL(videoPause ()      ), playerP, SLOT(pause()));
        connect(this, SIGNAL(videoPlay  (qint64)), playerP, SLOT(play(qint64)));
        connect(this, SIGNAL(videoRewind(qint64)), playerP, SLOT(rewind(qint64)));
        connect(this, SIGNAL(videoSeek  (qint64)), playerP, SLOT(seek(qint64)));
        player = playerP;
    }
    player->setUrl(url, askClose, title);

    if((askClose) && (player)) {
        videoPlayers.removeOne(player);
    }
    else if(timecode >= 0)
        player->seek(timecode);
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
    projectChanged();
}
void Project::projectChanged(const QString &strChanges) {
    QDomDocument xmlChanges("rekall");
    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    xmlChanges.setContent(strChanges, &errorMsg, &errorLine, &errorColumn);
    if(!errorMsg.isEmpty())
        qDebug("XML ERROR (%d : %d) %s", errorLine, errorColumn, qPrintable(errorMsg));

    QDomElement racine = xmlChanges.documentElement();
    QDomNode xmlNode = racine.firstChild();
    while(!xmlNode.isNull()) {
        QDomElement xmlElement = xmlNode.toElement();
        xmlProject.appendChild(xmlDoc.importNode(xmlElement, true));
        xmlNode = xmlNode.nextSibling();
    }
    emit projectChangedLoopback();
}
void Project::projectChanged() {
    hasChanged = true;
    state++;
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
