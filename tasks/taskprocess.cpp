#include "taskprocess.h"

TaskProcess::TaskProcess(const TaskProcessData &_data, QTreeWidgetItem *parentItem, QObject *parent) :
    QThread(parent), QTreeWidgetItem(parentItem) {
    processedDocument = _data;
}
void TaskProcess::init() {
    taskStarted = false;
    if(processedDocument.type == TaskProcessTypeMetadata) {
        processedDocument.metadata->status = DocumentStatusWaiting;
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Waiting for analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
    }
}

void TaskProcess::run() {
    taskStarted = true;
    if(processedDocument.type == TaskProcessTypeMetadata) {
        processedDocument.metadata->status = DocumentStatusProcessing;
        emit(updateList(this, FeedItemBaseTypeProcessingStart));
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Starting analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
        QDir().mkpath(Global::pathCurrent.absoluteFilePath() + "/rekall_cache");

        //Hash
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Calculating hash of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
        processedDocument.metadata->setMetadata("File", "Hash", Global::getFileHash(processedDocument.metadata->file), processedDocument.version);

        //Extract meta with ExifTool
        if(true) {
            emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Extracting metadatas of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
            QStringList exifDatas = launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/exiftool", Global::pathApplication.absoluteFilePath() + "/tools", QStringList() << "−c" << "%+.6f" << "-d" << "%Y:%m:%d %H:%M:%S" << "-G" << processedDocument.metadata->file.absoluteFilePath())).second.split("\n");
            foreach(const QString &exifData, exifDatas) {
                QPair<QString, QPair<QString,QString> > meta = Global::seperateMetadataAndGroup(exifData);
                if(meta.second.first == "File Type")
                    processedDocument.metadata->setMetadata(meta.first, meta.second.first, meta.second.second.toUpper(), processedDocument.version);
                else if((meta.second.first == "File Inode Change Date/Time") || (meta.second.first == "File Modification Date/Time") || (meta.second.first == "File Creation Date/Time") || (meta.second.first == "File Access Date/Time")) {}
                else if((meta.first != "ExifTool") && (!meta.second.second.contains("use -b option to extract"))) {
                    QString metaTitle = meta.second.first;
                    if(metaTitle == "GPS Position")
                        metaTitle = "GPS Coordinates";
                    processedDocument.metadata->setMetadata(meta.first, metaTitle, meta.second.second, processedDocument.version);
                }
                if(meta.second.first.toLower().contains("duration")) {
                    qreal duration = Global::getDurationFromString(meta.second.second);
                    if(duration) {
                        processedDocument.metadata->mediaDuration = duration;
                        processedDocument.metadata->setMetadata("Rekall", "Media Duration", duration, processedDocument.version);
                    }
                }
                if(meta.second.first.toLower().contains("author"))
                    processedDocument.metadata->setMetadata("Rekall", "Author",   meta.second.second, processedDocument.version);
                if(meta.second.first.toLower().contains("file size"))
                    processedDocument.metadata->setMetadata("Rekall", "Size",     meta.second.second, processedDocument.version);
                if(meta.second.first.toLower().contains("keywords"))
                    processedDocument.metadata->addKeyword(meta.second.second, processedDocument.version);
            }
        }

        QString thumbFilepath = Global::cacheFile("thumb", processedDocument.metadata->file);

        //Image thumb
        if(processedDocument.metadata->type == DocumentTypeImage) {
            emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating picture thumbnail of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
            QString thumbFilename = thumbFilepath + ".jpg";
            if(!QFileInfo(thumbFilename).exists()) {
                QImage thumbnail(processedDocument.metadata->file.absoluteFilePath());
                quint16 maxCote = 160;
                if((thumbnail.width() > maxCote) || (thumbnail.height() > maxCote))
                    QImage(processedDocument.metadata->file.absoluteFilePath()).scaled(QSize(maxCote, maxCote), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbFilename);
            }
            if(QFileInfo(thumbFilename).exists()) {
                processedDocument.metadata->setMetadata("Rekall", "Snapshot", "File", processedDocument.version);
                processedDocument.metadata->thumbnails.append(GlRect(thumbFilename));
            }
        }

        //Waveform
        if((processedDocument.metadata->type == DocumentTypeAudio) || (processedDocument.metadata->type == DocumentTypeVideo)) {
            emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating audio waveform of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
            QString thumbFilenameIntermediate = thumbFilepath + ".raw", thumbFilename = thumbFilepath + ".peak";

            if(!QFileInfo(thumbFilename).exists()) {
                launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg", thumbFilepath, QStringList()
                                              << "-i" << processedDocument.metadata->file.absoluteFilePath()
                                              << "-f" << "s16le" << "-ac" << "1" << "-ar" << "22050" << "-acodec" << "pcm_s16le"
                                              << thumbFilenameIntermediate));
                if(QFileInfo(thumbFilenameIntermediate).exists()) {

                    QFile thumbFileIntermediate(thumbFilenameIntermediate), thumbFile(thumbFilename);
                    if((thumbFileIntermediate.open(QFile::ReadOnly)) && (thumbFile.open(QFile::WriteOnly))) {
                        QByteArray samples = thumbFileIntermediate.readAll();
                        qint32 sampleStep = 22050 * Global::waveEach;
                        for(qint32 sampleIndexStart = 0 ; sampleIndexStart < samples.count() ; sampleIndexStart += sampleStep) {
                            qint16 valMax = 0, valMin = 1;
                            qint32 sampleIndexEnd = qMin(samples.count(), sampleIndexStart + sampleStep);
                            if(sampleIndexStart % 2)
                                sampleIndexStart++;

                            for(qint32 sampleIndex = sampleIndexStart ; sampleIndex < sampleIndexEnd ; sampleIndex+=2) {
                                union { qint16 i; char ch[2]; } u;
                                u.ch[0] = samples.at(sampleIndex);
                                u.ch[1] = samples.at(sampleIndex+1);
                                valMax = qMax(valMax, u.i);
                                valMin = qMin(valMin, u.i);
                            }
                            union { qint16 i; char ch[2]; } u1;
                            union { qint16 i; char ch[2]; } u2;
                            u1.i = valMin;
                            u2.i = valMax;
                            QByteArray byteArray;
                            byteArray.append(u1.ch[0]);
                            byteArray.append(u1.ch[1]);
                            byteArray.append(u2.ch[0]);
                            byteArray.append(u2.ch[1]);
                            thumbFile.write(byteArray);
                        }
                        thumbFileIntermediate.close();
                        thumbFile.close();
                        thumbFileIntermediate.remove();
                    }
                }
            }

            if(QFileInfo(thumbFilename).exists()) {
                qreal valMaxAbs = 0;
                QFile thumbnailFile(thumbFilename);
                if(thumbnailFile.open(QFile::ReadOnly)) {
                    QByteArray samples = thumbnailFile.readAll();
                    for(qint32 sampleIndex = 0 ; sampleIndex < samples.count() ; sampleIndex += 4) {
                        qreal valMax = 0, valMin = 1;
                        union { qint16 i; char ch[2]; } u;
                        u.ch[0] = samples.at(sampleIndex);
                        u.ch[1] = samples.at(sampleIndex+1);
                        valMin = (qreal)u.i / 32768.;
                        u.ch[0] = samples.at(sampleIndex+2);
                        u.ch[1] = samples.at(sampleIndex+3);
                        valMax = (qreal)u.i / 32768.;
                        valMaxAbs = qMax(qMax(valMaxAbs, qAbs(valMax)), qAbs(valMin));
                        processedDocument.metadata->waveform.append(qMakePair(valMin, valMax));
                    }
                    thumbnailFile.close();
                }
                processedDocument.metadata->waveform.normalisation = 1. / valMaxAbs;
            }
        }

        //Video thumbnails
        if(processedDocument.metadata->type == DocumentTypeVideo) {
            emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating video thumbnails of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
            //qDebug("===> %s", qPrintable(QDir(Global::pathCurrent.absoluteFilePath() + "/").relativeFilePath(data.metadata->file.absoluteFilePath())));
            quint16 thumbsNumber = qCeil(processedDocument.metadata->mediaDuration / Global::thumbsEach);
            if(!QFileInfo(thumbFilepath + "_1.jpg").exists()) {
                launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg", thumbFilepath, QStringList()
                                              << "-i" << processedDocument.metadata->file.absoluteFilePath()
                                              << "-f" << "image2"
                                              << "-vframes" << QString::number(thumbsNumber)
                                              << "-vf" << QString("fps=fps=1/%1").arg(Global::thumbsEach)
                                              << thumbFilepath + "_%d.jpg"
                                              ));

                //Rescale
                for(quint16 thumbIndex = 0 ; thumbIndex < thumbsNumber ; thumbIndex++) {
                    QString thumbFilename = QString(thumbFilepath + "_%1.jpg").arg(thumbIndex+1);
                    if(QFileInfo(thumbFilename).exists())
                        QImage(thumbFilename).scaled(QSize(160, 120), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbFilename);
                }
            }
            //Add to meta
            processedDocument.metadata->thumbnails.clear();
            for(quint16 thumbIndex = 0 ; thumbIndex < thumbsNumber ; thumbIndex++) {
                QString thumbFilename = QString(thumbFilepath + "_%1.jpg").arg(thumbIndex+1);
                if(QFileInfo(thumbFilename).exists())
                    processedDocument.metadata->thumbnails.append(GlRect(thumbFilename));
            }
        }

        //People
        if(processedDocument.metadata->type == DocumentTypePeople) {
            QList<Person*> persons = Person::fromFile(processedDocument.metadata->file.absoluteFilePath());
            foreach(Person *person, persons)
                Global::currentProject->addPerson(person);
        }

        processedDocument.metadata->status = DocumentStatusReady;
        emit(updateList(this, FeedItemBaseTypeProcessingEnd));
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Finishing analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(processedDocument.metadata->file.baseName())));
    }
    emit(finished(this));
}




QProcessOutput TaskProcess::launchCommand(const TaskProcessData &processData) {
    QProcessOutput retour;
    QString fullCommand = processData.command;
    foreach(const QString &argument, processData.arguments)
        fullCommand += " " + argument;

    //qDebug("[PROCESS] %s in %s", qPrintable(fullCommand), qPrintable(processData.workingDirectory));
    QProcess process;
    process.setWorkingDirectory(processData.workingDirectory);
    process.start(processData.command, processData.arguments);
    process.waitForStarted(-1);
    process.waitForFinished(-1);
    retour.second += process.readAllStandardOutput().trimmed();
    retour.second += process.readAllStandardError().trimmed();
    retour.first = fullCommand;
    //qDebug("[PROCESS] => %s", qPrintable(retour.second));
    return retour;
}


void TaskProcessData::parseOutput(const QProcessOutput &result) {
    resultCommand = result.first;
    resultOutput  = result.second.split("\n");
}

