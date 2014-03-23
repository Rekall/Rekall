#include "taskprocess.h"

TaskProcess::TaskProcess(const TaskProcessData &_data, QTreeWidgetItem *parentItem, QObject *parent) :
    QThread(parent), QTreeWidgetItem(parentItem) {
    document = _data;
    if(document.metadata->file.exists())
        file = document.metadata->file;
    name = document.metadata->getName(document.version);
}
void TaskProcess::init() {
    started = false;
    if(document.type == TaskProcessTypeMetadata) {
        document.metadata->status = DocumentStatusWaiting;
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Waiting for analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
    }
}

void TaskProcess::run() {
    started = true;
    if(document.type == TaskProcessTypeMetadata) {
        document.metadata->status = DocumentStatusProcessing;
        emit(updateList(this, FeedItemBaseTypeProcessingStart));
        emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Starting analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
        QDir().mkpath(Global::pathCurrent.absoluteFilePath() + "/rekall_cache");
        bool shouldSendFinishedSignal = true;

        if((document.needCompleteScan) && (document.metadata->getType(document.version) == DocumentTypeWeb)) {
            thumbFilepath = Global::cacheFile("thumb", QCryptographicHash::hash(qPrintable(document.metadata->getMetadata("Rekall", "URL", document.version).toString()), QCryptographicHash::Sha1).toHex().toUpper());
            thumbFilename = thumbFilepath + ".jpg";
            if(!QFile(thumbFilename).exists()) {
                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Loading web page <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                emit(analyseWebContent(this));
                shouldSendFinishedSignal = false;
            }
        }
        else {
            if((document.needCompleteScan) && (document.metadata->getType(document.version) == DocumentTypePeople)) {
                //People
                QList<Person*> persons = Person::fromFile(file.absoluteFilePath());
                foreach(Person *person, persons)
                    Global::currentProject->addPerson(person);
            }

            thumbFilepath = Global::cacheFile("thumb", file);
            thumbFilename = thumbFilepath + ".jpg";

            //Hash
            if(file.exists()) {
                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Calculating hash of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                QString documentHash = Global::getFileHash(file);
                if(documentHash != document.metadata->getMetadata("File", "Hash", document.version).toString()) {
                    document.needCompleteScan = true;
                    document.metadata->setMetadata("File", "Hash", documentHash, document.version);
                }
            }

            //Extract meta with ExifTool
            if(document.needCompleteScan) {
                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Extracting metadatas of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                QStringList exifDatas = launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/exiftool", Global::pathApplication.absoluteFilePath() + "/tools", QStringList() << "−c" << "%+.6f" << "-d" << "%Y:%m:%d %H:%M:%S" << "-G" << file.absoluteFilePath())).second.split("\n");
                foreach(const QString &exifData, exifDatas) {
                    QPair<QString, QPair<QString,QString> > meta = Global::seperateMetadataAndGroup(exifData);
                    if((!meta.first.isEmpty()) && (!meta.second.first.isEmpty()) && (!meta.second.second.isEmpty())) {
                        if(meta.second.first == "File Type")
                            document.metadata->setMetadata(meta.first, meta.second.first, meta.second.second.toUpper(), document.version);
                        else if((meta.second.first == "File Inode Change Date/Time") || (meta.second.first == "File Modification Date/Time") || (meta.second.first == "File Creation Date/Time") || (meta.second.first == "File Access Date/Time")) {}
                        else if((meta.first != "ExifTool") && (!meta.second.second.contains("use -b option to extract"))) {
                            QString metaTitle = meta.second.first;
                            if(metaTitle == "GPS Position")
                                metaTitle = "GPS Coordinates";
                            document.metadata->setMetadata(meta.first, metaTitle, meta.second.second, document.version);
                        }
                        if((meta.second.first.toLower().contains("duration")) && (!(meta.second.first.toLower().contains("value")))) {
                            qreal duration = Global::getDurationFromString(meta.second.second);
                            if(duration)
                                document.metadata->setMetadata("Rekall", "Media Duration", duration, document.version);
                        }
                        if(meta.second.first.toLower().contains("author"))
                            document.metadata->setMetadata("Rekall", "Author", meta.second.second, document.version);
                        if(meta.second.first.toLower().contains("file size"))
                            document.metadata->setMetadata("Rekall", "Size",   meta.second.second, document.version);
                        if(meta.second.first.toLower().contains("keywords"))
                            document.metadata->addKeyword(meta.second.second, document.version);
                    }
                }
            }

            //Image thumb
            if(document.metadata->getType() == DocumentTypeImage) {
                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating picture thumbnail of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                if(!QFileInfo(thumbFilename).exists()) {
                    QImage thumbnail(file.absoluteFilePath());
                    quint16 maxCote = 160;
                    if((thumbnail.width() > maxCote) || (thumbnail.height() > maxCote))
                        QImage(file.absoluteFilePath()).scaled(QSize(maxCote, maxCote), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(thumbFilename);
                }
                if(QFileInfo(thumbFilename).exists()) {
                    document.metadata->setMetadata("Rekall", "Snapshot", "File", document.version);
                    document.metadata->thumbnails.append(GlRect(thumbFilename));
                }
            }

            //Waveform
            if((document.metadata->getType() == DocumentTypeAudio) || (document.metadata->getType() == DocumentTypeVideo)) {
                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating audio waveform of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                QString thumbFilenameIntermediate = thumbFilepath + ".raw", thumbFilename = thumbFilepath + ".peak";

                if(!QFileInfo(thumbFilename).exists()) {
                    launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg", thumbFilepath, QStringList()
                                                  << "-i" << file.absoluteFilePath()
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
                            document.metadata->waveform.append(qMakePair(valMin, valMax));
                        }
                        thumbnailFile.close();
                    }
                    document.metadata->waveform.normalisation = 1. / valMaxAbs;
                }
            }

            //Video thumbnails
            if(document.metadata->getType() == DocumentTypeVideo) {
                qreal mediaDuration = document.metadata->getMetadata("Rekall", "Media Duration", document.version).toDouble();

                emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Creating video thumbnails of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
                //qDebug("===> %s", qPrintable(QDir(Global::pathCurrent.absoluteFilePath() + "/").relativeFilePath(data.metadata->file.absoluteFilePath())));
                quint16 thumbsNumber = qCeil(mediaDuration / Global::thumbsEach);
                if(!QFileInfo(thumbFilepath + "_1.jpg").exists()) {
                    launchCommand(TaskProcessData(Global::pathApplication.absoluteFilePath() + "/tools/ffmpeg", thumbFilepath, QStringList()
                                                  << "-i" << file.absoluteFilePath()
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
                document.metadata->thumbnails.clear();
                for(quint16 thumbIndex = 0 ; thumbIndex < thumbsNumber ; thumbIndex++) {
                    QString thumbFilename = QString(thumbFilepath + "_%1.jpg").arg(thumbIndex+1);
                    if(QFileInfo(thumbFilename).exists())
                        document.metadata->thumbnails.append(GlRect(thumbFilename));
                }
            }
        }

        if(shouldSendFinishedSignal)
            sendFinishedSignal();
    }
}
void TaskProcess::sendFinishedSignal() {
    document.metadata->status = DocumentStatusReady;
    emit(updateList(this, FeedItemBaseTypeProcessingEnd));
    emit(updateList(this, tr("<span style='font-family: Calibri, Arial; font-size: 11px; color: #A1A5A7'>Finishing analysis of <span style='color: #F5F8FA'>%1</span></span>").arg(name)));
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

