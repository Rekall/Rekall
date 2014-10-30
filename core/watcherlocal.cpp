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


#include "watcherlocal.h"

WatcherLocal::WatcherLocal(ProjectInterface *parent) :
    WatcherInterface(parent) {
    watcher = 0;
}

WatcherInterface* WatcherLocal::monitor(const QFileInfo &path) {
    monitoredPath = path;
    watcher = new Mirall::FolderWatcher(monitoredPath.absoluteFilePath(), this);
    connect(watcher, SIGNAL(folderChanged(QString)), SLOT(directoryChanged(QString)));
    return this;
}
void WatcherLocal::start() {
    scanFolder(new SyncEntry(monitoredPath));
}

const QFileInfoList WatcherLocal::scanFolder(SyncEntry *path, bool returnsOnly) {
    if((path->exists()) && (path->isDir()) && (!path->absoluteFilePath().contains("rekall_cache"))) {
        QCoreApplication::processEvents();
        QString pathName = path->absoluteFilePath();
        QFileInfoList files = QDir(pathName).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        if(returnsOnly)
            return files;

        if(!folders.contains(pathName))
            folders.insert(pathName, SyncEntry());

        foreach(const QFileInfo &file, files) {
            SyncEntry *fileEntry = 0;
            if(folders[pathName].contains(file.absoluteFilePath()))
                fileEntry = folders[pathName][file.absoluteFilePath()];
            else {
                fileEntry = new SyncEntry(file);
                folders[pathName].insert(file.absoluteFilePath(), fileEntry);
            }
            fileEntry->isInMacOsBundle = path->isBundle2();

#ifdef Q_OS_MAC
            if((false) && (fileEntry->isDir())) {
                fileEntry->isMacOsBundle = fileEntry->isBundle();
                if(!fileEntry->isMacOsBundle) {
                    QProcess mdlsTool;
                    mdlsTool.start("mdls", QStringList() << "-name" << "kMDItemContentTypeTree" << file.absoluteFilePath());
                    mdlsTool.waitForFinished();
                    fileEntry->isMacOsBundle = (mdlsTool.readAllStandardOutput().contains("com.apple.package") == true);
                }
            }
#endif

            if(fileEntry->isDir())
                scanFolder(fileEntry, returnsOnly);
            else
                Global::analyse->addToQueue(fileEntry, project);
        }
    }
    return QFileInfoList();
}

void WatcherLocal::directoryChanged(const QString &path) {
    if((!path.isEmpty()) && (!path.contains("rekall_cache"))) {
        bool debug = false;

        //Préparation du process
        SyncEntry *pathFile = new SyncEntry(path);
        if(debug) {
            qDebug("====================================================================================");
            qDebug("Changes in folder %s", qPrintable(pathFile->absoluteFilePath()));
        }

        //Compare le cache et le réel
        if(!folders.contains(pathFile->absoluteFilePath())) {
            if(debug)
                qDebug("New folder %s", qPrintable(pathFile->absoluteFilePath()));
            pathFile->action = SyncCreate;
            emit(fileChanged(pathFile));
            folders.insert(pathFile->absoluteFilePath(), SyncEntry());
            folders[pathFile->parentDir()][pathFile->absoluteFilePath()] = pathFile;
        }

        //Copie du cache pour manipulations
        SyncEntry filesCache = folders[pathFile->absoluteFilePath()];

        //Liste des différences Now > Cache
        QFileInfoList filesNow = scanFolder(pathFile, true);
        foreach(const QFileInfo &_fileNow, filesNow) {
            SyncEntry *fileNow = new SyncEntry(_fileNow);
            QString log = "\t" + fileNow->baseName() + "\n";
            if(filesCache.contains(fileNow->absoluteFilePath())) {
                SyncEntry *fileCache = filesCache.value(fileNow->absoluteFilePath());
                fileNow->metadatas = fileCache->metadatas;

                if((!fileCache->isDir()) && ((fileCache->lastModified() != fileNow->lastModified()) || (fileCache->QFileInfo::size() != fileNow->QFileInfo::size()))) {
                    log += "(file up)\t" + fileNow->baseName();
                    fileNow->action = SyncUpdate;
                    emit(fileChanged(fileNow));
                    folders[pathFile->absoluteFilePath()][fileNow->absoluteFilePath()] = fileNow;
                }
                else
                    log += "\t" + fileCache->baseName();
            }
            else if(fileNow->isFile()) {
                log += "(file new)\t-";
                fileNow->action = SyncCreate;
                emit(fileChanged(fileNow));
                folders[pathFile->absoluteFilePath()][fileNow->absoluteFilePath()] = fileNow;
            }
            filesCache.remove(fileNow->absoluteFilePath());
            if(debug)
                qDebug("%s", qPrintable(log));
        }

        //Liste des différences Cache > Now
        QHashIterator<QString, SyncEntry*> fileCacheIterator(filesCache);
        while (fileCacheIterator.hasNext()) {
            fileCacheIterator.next();
            SyncEntry *fileCache = fileCacheIterator.value();
            QString log = "\t-\n";

            if(fileCache->isFile()) {
                log += "(file rem)\t" + fileCache->baseName();
                fileCache->action = SyncDelete;
                emit(fileChanged(fileCache));
                folders[pathFile->absoluteFilePath()].remove(fileCache->absoluteFilePath());
            }
            else {
                log += "(fold rem)\t" + fileCache->baseName();
                fileCache->action = SyncDelete;
                emit(fileChanged(fileCache));
                folders.remove(fileCache->absoluteFilePath());
                folders[pathFile->absoluteFilePath()].remove(fileCache->absoluteFilePath());
            }
            if(debug)
                qDebug("%s", qPrintable(log));
        }
    }
}
