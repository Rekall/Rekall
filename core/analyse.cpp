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

#include "analyse.h"

Analyse::Analyse(QObject *parent) :
    QThread(parent) {
    okThread = true;
    thumbnailThreadsCount = 0;
    paused = false;
    lastAnalyse = QDateTime::currentDateTime();
    start();
}
Analyse::~Analyse() {
}

void Analyse::addToQueue(SyncEntry *file, ProjectInterface *project) {
    if(file->exists()) {
        AnalyseProcess *analyseProcess = new AnalyseProcess(file, project, this);
        connect(analyseProcess, SIGNAL(threadFinished(AnalyseProcess*)), SLOT(threadFinished(AnalyseProcess*)));
        connect(analyseProcess, SIGNAL(projectChanged(SyncEntry*,bool)), project, SLOT(projectChanged(SyncEntry*,bool)));
        metadataQueue.enqueue(analyseProcess);
    }
}
void Analyse::stop() {
    okThread = false;
    if(!wait(2000)) {
        terminate();
        if(!wait(2000))
            quit();
    }
    if(Global::exifToolProcess)
        Global::exifToolProcess->kill();
}

void Analyse::run() {
    while(okThread) {
        if((metadataQueue.count()) && (!paused)) {
            AnalyseProcess *analyseProcess = metadataQueue.dequeue();
            if(!analyseProcess->project->isRemoved) {
                trayIconToOn(10000);

                if(analyseProcess->process())   thumbnailQueue.append(analyseProcess);
                else                            analyseProcess->deleteLater();
            }
            lastAnalyse = QDateTime::currentDateTime();
            //QCoreApplication::processEvents();
            //msleep(5);
        }
        else {
            //trayMenu->setTitle(tr("Recent activity"));
            //trayIconToOff();
            QCoreApplication::processEvents();
            msleep(200);
        }
        if((thumbnailQueue.count()) && (thumbnailThreadsCount < 5) && (!paused)) {
            AnalyseProcess *analyseProcess = thumbnailQueue.dequeue();
            if(!analyseProcess->project->isRemoved) {
                thumbnailThreadsCount++;
                trayIconToOn(10000);
                analyseProcess->start();
                lastAnalyse = QDateTime::currentDateTime();
            }
        }

        QString text;
        bool trayEnable = true;
        if((thumbnailThreadsCount+thumbnailQueue.count()) || (metadataQueue.count())) {
            if((metadataQueue.count()) && (thumbnailThreadsCount+thumbnailQueue.count()))
                text += tr("Analysing metadatas and generating thumbnails… (%1 remaining)…").arg(metadataQueue.count() + thumbnailThreadsCount+thumbnailQueue.count());
            else if(metadataQueue.count())
                text += tr("Analysing metadatas… (%1 remaining)").arg(metadataQueue.count() + thumbnailThreadsCount+thumbnailQueue.count());
            else if(thumbnailThreadsCount+thumbnailQueue.count())
                text += tr("Generating thumbnails… (%1 remaining)").arg(metadataQueue.count() + thumbnailThreadsCount+thumbnailQueue.count());
            if(paused)
                text += " [paused]";
        }
        else {
            trayEnable = false;
            text += tr("Rekall projects are up to date, ") + Global::dateToString(lastAnalyse).toLower();

            emit trayIconToOff();
        }
        emit(trayChanged(text, trayEnable));
    }
}
void Analyse::threadFinished(AnalyseProcess *) {
    thumbnailThreadsCount--;
}


AnalyseProcess::AnalyseProcess(SyncEntry *_file, ProjectInterface *_project, QObject *parent)
    : QThread(parent) {
    file    = _file;
    project = _project;
    noChanges = true;
    firstAnalysis = true;
}
void AnalyseProcess::getMetadatas() {
    previousMeta = file->metadatas;
    firstAnalysis = (previousMeta.count() == 0);

    file->metadatas = Global::getMetadatas(*file, project);
    if(file->isBundleForSure()) {
        file->metadatas["File->File Name"] = file->fileName();
    }

    file->metadatas["File->Hash"] = Global::getFileHash(*file);

    //Bundle
    if(file->isBundleForSure())
        file->metadatas["Rekall->Flag"] = "Bundle";
    else if(file->isIntoBundle())
        file->metadatas["Rekall->Flag"] = "Bundled File";
    else
        file->metadatas["Rekall->Flag"] = "File";
    file->metadatas["Rekall->Import Date"] = QDateTime::currentDateTime().toString("yyyy:MM:dd hh:mm:ss");

    if(file->userInfos) {
        QHashIterator<QString, QString> metadataIterator(*file->userInfos);
        while (metadataIterator.hasNext()) {
            metadataIterator.next();
            file->metadatas["Rekall User Infos->" + metadataIterator.key()] = metadataIterator.value();
        }
    }

    project->sync->folders[file->absolutePath()][file->absoluteFilePath()] = file;

    noChanges = true;
    if(previousMeta.value("File->Hash") == file->metadatas["File->Hash"]) {
        noChanges = true;
        //qDebug("HASH: %s vs. %s", qPrintable(previousMeta.value("File->Hash")), qPrintable(file->metadatas["File->Hash"]));
    }
    else if(previousMeta.count() != file->metadatas.count()) {
        noChanges = false;
        qDebug("COUNT: %d vs. %d", previousMeta.count(), file->metadatas.count());
    }
    else {
        QHashIterator<QString, QString> metadataIterator(file->metadatas);
        while (metadataIterator.hasNext()) {
            metadataIterator.next();
            if((previousMeta.value(metadataIterator.key()) != metadataIterator.value()) && (metadataIterator.key() != "Rekall->Import Date")) {
                noChanges = false;
                qDebug("META %s: %s vs. %s", qPrintable(metadataIterator.key()), qPrintable(previousMeta.value(metadataIterator.key())), qPrintable(metadataIterator.value()));
                break;
            }
        }
    }
}
void AnalyseProcess::getThumbnails() {
    //Path des vignettes
    QCryptographicHash thumbHasher(QCryptographicHash::Sha1);
    thumbHasher.addData(file->metadatas["Rekall->Folder"].toUtf8());
    QString thumbFile = QString(thumbHasher.result().toHex()).toUpper() + "-" + file->metadatas["File->Hash"];
    QString thumbPath = project->path.absoluteFilePath() + "/rekall_cache/" + thumbFile;
    quint16 maxCote = 160;
    bool thumbExists = false;

    //Autoextract de vignettes via QuickLook
#ifdef Q_OS_MAC
    if(!QFileInfo(thumbPath + ".jpg").exists()) {
        QFileInfo tmpThumbPath(project->path.absoluteFilePath() + "/rekall_cache/" + file->fileName() + ".png");
        QProcess quickLookTool;
        quickLookTool.start("qlmanage", QStringList() << "-t" << "-s" << QString::number(maxCote) << "-o" << tmpThumbPath.absolutePath() << file->absoluteFilePath());
        quickLookTool.waitForFinished();
        QImage(tmpThumbPath.absoluteFilePath()).save(thumbPath + ".jpg");
        QFile(tmpThumbPath.absoluteFilePath()).remove();
    }
#endif

    //Autoextract de vignettes
    if(!QFileInfo(thumbPath + ".jpg").exists()) {
        QHashIterator<QString, QString> metadataIterator(file->metadatas);
        while (metadataIterator.hasNext()) {
            metadataIterator.next();
            if(!QFileInfo(thumbPath + ".jpg").exists()) {
                if(metadataIterator.value().toLower().contains("use -b option to extract")) {
                    QStringList exifKeys = metadataIterator.key().split("->");
                    if(exifKeys.count() > 1) {
                        QString exifKey = exifKeys[0].replace(" ", "") + ":" + exifKeys[1].replace(" ", "");
                        QProcess exifTool;
                        exifTool.setStandardOutputFile(thumbPath + ".jpg");
#ifdef Q_OS_WIN
                        exifTool.start(Global::pathApplication.absoluteFilePath() + "/tools/exiftool.exe", QStringList() << "−b" << "-" + exifKey << file->absoluteFilePath());
#else
                        exifTool.start(Global::pathApplication.absoluteFilePath() + "/tools/exiftool", QStringList() << "−b" << "-" + exifKey << file->absoluteFilePath());
#endif
                        exifTool.waitForFinished();
                        if(QFileInfo(thumbPath + ".jpg").size() < 100)
                            QFile(thumbPath + ".jpg").remove();
                        else
                            QImage(thumbPath + ".jpg").scaled(QSize(maxCote, maxCote), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbPath + ".jpg");
                    }
                }
            }
        }
    }

    //Vignettes image
    if(file->metadatas["File->MIME Type"].startsWith("image")) {
        if(!QFileInfo(thumbPath + ".jpg").exists()) {
            QImage(file->absoluteFilePath()).scaled(QSize(maxCote, maxCote), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbPath + ".jpg");
        }
    }
    //Vignettes video
    else if(file->metadatas["File->MIME Type"].startsWith("video")) {
        if(!QFileInfo(thumbPath + "_1.jpg").exists()) {
            quint16 thumbsEach = 5, thumbIndex = 1;
            QProcess ffmpegTool;
#ifdef Q_OS_WIN
            ffmpegTool.start(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg.exe", QStringList() << "-i" << file->absoluteFilePath() << "-f" << "image2" << "-vf" << QString("fps=fps=1/%1").arg(thumbsEach) << thumbPath + "_%d.jpg");
#else
            ffmpegTool.start(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg", QStringList() << "-i" << file->absoluteFilePath() << "-f" << "image2" << "-vf" << QString("fps=fps=1/%1").arg(thumbsEach) << thumbPath + "_%d.jpg");
#endif
            ffmpegTool.waitForFinished();

            QString thumbPathNumbered = QString(thumbPath + "_%1.jpg").arg(thumbIndex);
            while(QFileInfo(thumbPathNumbered).exists()) {
                QImage(thumbPathNumbered).scaled(QSize(maxCote, maxCote), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbPathNumbered);
                thumbPathNumbered = QString(thumbPath + "_%1.jpg").arg(++thumbIndex);
            }
            if((file->metadatas.contains("Rekall->Media Duration    (s.)")) && (file->metadatas["Rekall->Media Duration (s.)"].toDouble() > 600))
                file->metadatas["Rekall->Media Function"] = "Render";
            else
                file->metadatas["Rekall->Media Function"] = "Contextual";
        }
        thumbExists = true;
    }


    if((QFileInfo(thumbPath + ".jpg").exists()) || (thumbExists))
        file->metadatas["File->Thumbnail"] = thumbFile;
}
void AnalyseProcess::run() {
    QDateTime s = QDateTime::currentDateTime();
    getThumbnails();
    qDebug("[%lld ms] Vignettage de %s terminée", s.msecsTo(QDateTime::currentDateTime()), qPrintable(file->fileName()));
    emit(threadFinished(this));
    emit(projectChanged(file, firstAnalysis));
}
bool AnalyseProcess::process() {
    QDateTime s = QDateTime::currentDateTime();
    getMetadatas();
    qDebug("[%lld ms] Analyse de %s terminée avec le code %d", s.msecsTo(QDateTime::currentDateTime()), qPrintable(file->fileName()), noChanges);

    return !noChanges;
}
