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


#include "global.h"
#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

QFileInfo Global::pathApplication;
QFileInfo Global::pathDocuments;
QFileInfo Global::pathCurrent;
QProcess *Global::exifToolProcess = 0;
QString   Global::configFileName;
RekallInterface* Global::rekall = 0;
Udp* Global::udp = 0;
AnalyseInterface* Global::analyse = 0;
QList<ProjectInterface*> Global::projects;
UserInfosInterface* Global::userInfos = 0;
HttpInterface* Global::http = 0;

QPair<QString,QString> Global::seperateMetadata(const QString &metaline, const QString &separator) {
    QPair<QString,QString> retour;
    qint16 index = metaline.indexOf(separator);
    if(index > 0) {
        retour.first = metaline.left(index).trimmed();
        retour.second = metaline.right(metaline.length() - index - separator.length()).trimmed();
    }
    return retour;
}
QStringList Global::splitQuotes(const QString &metaline, const QString &separator) {
    bool inside = metaline.startsWith("\"");
    QStringList tmpList = metaline.split(QRegExp("\""), QString::SkipEmptyParts);
    QStringList retour;
    foreach (const QString &s, tmpList) {
        if (inside)
            retour.append(s);
        else
            retour.append(s.split(separator, QString::SkipEmptyParts));
        inside = !inside;
    }
    return retour;
}
QString Global::unicodeEscape(const QString &str) {
    QString outputStr = str;
    if (outputStr.contains("\\U")) {
        do {
            int idx = outputStr.indexOf("\\U");
            QString strHex = outputStr.mid(idx, 6);
            strHex = strHex.replace("\\U", QString());
            int nHex = strHex.toInt(0, 16);
            outputStr.replace(idx, 6, QChar(nHex));
        } while (outputStr.indexOf("\\U") != -1);
    }
    return outputStr;
}

Metadatas Global::getMetadatas(QFileInfo file, ProjectInterface *project, bool debug) {
    Metadatas retour;
    file.refresh();
    if(file.exists()) {
        //Champs par défault
        retour["Rekall->Author"]   = "";
        retour["Rekall->Comments"] = "";
        retour["Rekall->Keywords"] = "";
        qreal duration = 0;
        QStringList keywords;

        //ExifTool
        if(!exifToolProcess) {
            exifToolProcess = new QProcess();
#ifdef Q_OS_MAC
            exifToolProcess->start(Global::pathApplication.absoluteFilePath() + "/tools/exiftool", QStringList() << "-stay_open" << "True" << "-@" << "-");
#endif
            exifToolProcess->waitForStarted();
        }

        qDebug("===> %s", qPrintable(file.absoluteFilePath() + "\n"));
        exifToolProcess->write(qPrintable(file.absoluteFilePath() + "\n"));
        exifToolProcess->write("-G\n");
        exifToolProcess->write("-c\n%+.6f\n");
        exifToolProcess->write("-d\n%Y:%m:%d %H:%M:%S\n");
        exifToolProcess->write("-t\n");
        exifToolProcess->write("-execute\n");
        exifToolProcess->waitForBytesWritten();

        QByteArray linesString;
        while(linesString.endsWith("{ready}\n") == false) {
            exifToolProcess->waitForReadyRead();
            linesString += exifToolProcess->readAllStandardOutput();
        }
        /*
        QProcess exifTool;
        //QDateTime s = QDateTime::currentDateTime();
#ifdef Q_OS_MAC
        exifTool.start(Global::pathApplication.absoluteFilePath() + "/tools/exiftool", QStringList() << "−c" << "%+.6f" << "-d" << "%Y:%m:%d %H:%M:%S" << "-G" << "-t" << file.absoluteFilePath());
#endif
        exifTool.waitForFinished();
        linesString = exifTool.readAllStandardOutput().trimmed();
         */
        QStringList lines = QString(linesString).split('\n');
        foreach(const QString &line, lines) {
            qDebug("===> %s", qPrintable(line));
            QStringList metadatas = line.split('\t');
            if(metadatas.length() > 2) {
                QString metadataKey   = metadatas.at(0) + "->" + metadatas.at(1);
                QString metadataValue = metadatas.at(2);
                retour[metadataKey] = metadataValue;

                if((metadataKey == "Composite->Duration") || (metadataKey == "QuickTime->Duration"))
                    duration = getDuration(metadataValue);
                if(metadataKey.toLower().contains("author"))
                    retour["Rekall->Author"]         = metadataValue;
                if(metadataKey == "File->File Size") {
                    retour["Rekall->File Size"]      = metadataValue;
                    retour["Rekall->File Size (MB)"] = QString::number(file.size()/(1024.*1024.));
                }
                if(metadataKey.toLower().contains("keywords"))
                    keywords << Global::splitQuotes(metadataValue);
            }
            //qDebug("\tEXIF %lld ms", s.msecsTo(QDateTime::currentDateTime()));
        }


        //XAttr
#ifdef Q_OS_MAC
        if(true) {
            QProcess xAttrTool1, xAttrTool2, xAttrTool3;
            xAttrTool1.setStandardOutputProcess(&xAttrTool2);
            xAttrTool1.start("xattr", QStringList() << "-p" << "com.apple.metadata:_kMDItemUserTags" << file.absoluteFilePath());
            xAttrTool1.waitForFinished();
            xAttrTool2.setStandardOutputProcess(&xAttrTool3);
            xAttrTool2.start("xxd",   QStringList() << "-r" << "-p");
            xAttrTool2.waitForFinished();
            xAttrTool3.start("plutil", QStringList() << "-convert" << "xml1" << "-" << "-o" << "-");
            xAttrTool3.waitForFinished();
            QDomDocument xmlDoc = QDomDocument("plist");
            xmlDoc.setContent(xAttrTool3.readAllStandardOutput());
            QDomElement xmlPlist = xmlDoc.documentElement();
            QDomNode plistNode = xmlPlist.firstChild();
            while(!plistNode.isNull()) {
                QDomElement arrayElement = plistNode.toElement();
                if((!arrayElement.isNull()) && (arrayElement.nodeName().toLower() == "array")) {
                    QDomNode stringNode = arrayElement.firstChild();
                    while(!stringNode.isNull()) {
                        QDomElement stringElement = stringNode.toElement();
                        if((!stringElement.isNull()) && (stringElement.nodeName().toLower() == "string"))
                            keywords << stringElement.text().split("\n").first().trimmed();
                        stringNode = stringNode.nextSibling();
                    }
                }
                plistNode = plistNode.nextSibling();
            }
        }
        if(true) {
            QProcess xAttrTool1, xAttrTool2, xAttrTool3;
            xAttrTool1.setStandardOutputProcess(&xAttrTool2);
            xAttrTool1.start("xattr", QStringList() << "-p" << "com.apple.metadata:kMDItemFinderComment" << file.absoluteFilePath());
            xAttrTool1.waitForFinished();
            xAttrTool2.setStandardOutputProcess(&xAttrTool3);
            xAttrTool2.start("xxd",   QStringList() << "-r" << "-p");
            xAttrTool2.waitForFinished();
            xAttrTool3.start("plutil", QStringList() << "-convert" << "xml1" << "-" << "-o" << "-");
            xAttrTool3.waitForFinished();
            QDomDocument xmlDoc = QDomDocument("plist");
            xmlDoc.setContent(xAttrTool3.readAllStandardOutput());
            QDomElement xmlPlist = xmlDoc.documentElement();
            QDomNode plistNode = xmlPlist.firstChild();
            while(!plistNode.isNull()) {
                QDomElement stringElement = plistNode.toElement();
                if((!stringElement.isNull()) && (stringElement.nodeName().toLower() == "string"))
                    retour["Rekall->Comments"] = stringElement.text().split("\n").first().trimmed();
                plistNode = plistNode.nextSibling();
            }
        }
#endif


        //MDLS for Mac
#ifdef Q_OS_MAC
        if(false) {
            //s = QDateTime::currentDateTime();
            QProcess mdlsTool;
            mdlsTool.start("mdls", QStringList() << file.absoluteFilePath());
            mdlsTool.waitForFinished();
            QStringList lines = Global::unicodeEscape(QString(mdlsTool.readAllStandardOutput()).trimmed()).split('\n');
            bool lineArray = false, process = false;
            QString currentCategory = "", currentValue;
            foreach(const QString &line, lines) {
                process = false;
                if(!lineArray) {
                    QPair<QString,QString> lineItems = Global::seperateMetadata(line, " = ");
                    if((!lineItems.first.isEmpty()) && (!lineItems.second.isEmpty())) {
                        currentCategory = lineItems.first;
                        currentValue    = lineItems.second;
                        if(currentValue == "(") {
                            lineArray = true;
                            currentValue.clear();
                        }
                        else
                            process = true;
                    }
                    else
                        lineArray = false;
                }
                else {
                    if(line.trimmed() == ")") {
                        process = true;
                        lineArray = false;
                    }
                    else
                        currentValue += line.trimmed();
                }

                if((process) && (!currentValue.isEmpty())) {
                    QStringList values = Global::splitQuotes(currentValue);
                    currentValue.clear();
                    foreach(const QString &value, values)
                        currentValue += value.trimmed() + ",";
                    currentValue.chop(1);
                    if(!currentValue.isEmpty()) {
                        if((currentCategory == "kMDItemUserTags") || (currentCategory == "kMDItemKeywords"))
                            keywords << values;
                        else if(currentCategory == "kMDItemAuthors")
                            retour["Rekall->Author"] = currentValue;
                        else if(currentCategory == "kMDItemComment")
                            retour["Rekall->Comments"] = currentValue;
                        else if(currentCategory == "kMDItemKind")
                            retour["Rekall->Type"] = currentValue;
                        else if(currentCategory == "kMDItemTitle")
                            retour["Rekall->Title"] = currentValue;
                    }
                }
            }
            //qDebug("\tMDLS %lld ms", s.msecsTo(QDateTime::currentDateTime()));
        }
#endif


        //Type Mime
        if(true) {
#ifdef Q_OS_MAC
            //QDateTime s = QDateTime::currentDateTime();
            QProcess fileTool;
            fileTool.start("file", QStringList() << "-b" << "--mime-type" << file.absoluteFilePath());
            fileTool.waitForFinished();
            QStringList lines = QString(fileTool.readAllStandardOutput().trimmed()).split('\n');
            if((lines.count()) && (!lines.first().trimmed().isEmpty()))
                retour["File->MIME Type"] = lines.first().trimmed();
#endif
            //qDebug("\tFile %lld ms", s.msecsTo(QDateTime::currentDateTime()));
        }

        //Types
        if(!retour.contains("File->File Type"))
            retour["File->File Type"] = retour["File->Extension"];

        if(!retour.contains("Rekall->Type")) {
            if     (retour.contains("File->MIME Type")) retour["Rekall->Type"] = retour["File->MIME Type"];
            else                                        retour["Rekall->Type"] = retour["File->File Type"];
        }

        //Mots clefs
        retour["Rekall->Keywords"] = "";
        foreach(const QString &keyword, keywords)
            retour["Rekall->Keywords"] += keyword.trimmed() + ",";
        retour["Rekall->Keywords"].chop(1);

        //Durée
        if((retour.contains("AIFF->Num Sample Frames")) && (retour.contains("AIFF->Sample Rate")))
            duration = retour["AIFF->Num Sample Frames"].toDouble() / retour["AIFF->Sample Rate"].toDouble();
        if(duration > 0) {
            retour["Rekall->Media Duration"]      = timeToString(duration);
            retour["Rekall->Media Duration (s.)"] = QString::number(duration);
        }

        //Autres champs
        retour["File->Basename"]  = file.baseName();
        retour["File->Owner"]     = file.owner();
        retour["File->Extension"] = file.suffix();
        retour["File->File Creation Date/Time"]     = file.created()     .toString("yyyy:MM:dd hh:mm:ss");
        retour["File->File Access Date/Time"]       = file.lastRead()    .toString("yyyy:MM:dd hh:mm:ss");
        retour["File->File Modification Date/Time"] = file.lastModified().toString("yyyy:MM:dd hh:mm:ss");

        //Champs Rekalliens
        retour["Rekall->Extension"] = retour["File->Extension"].toUpper();
        retour["Rekall->Date/Time"] = retour["File->File Modification Date/Time"];
        retour["Rekall->Name"]      = retour["File->Basename"];
        if(project)
            retour["Rekall->Folder"] = QString(file.dir().absolutePath() + "/").remove(project->path.absoluteFilePath() + "/");


        /*
        //Type
        if(     suffixesTypeDoc   .contains(file.suffix().toLower()))   setType(DocumentTypeDoc,    version);
        else if(suffixesTypeAudio .contains(file.suffix().toLower()))   setType(DocumentTypeAudio,  version);
        else if(suffixesTypeImage .contains(file.suffix().toLower()))   setType(DocumentTypeImage,  version);
        else if(suffixesTypeVideo .contains(file.suffix().toLower()))   setType(DocumentTypeVideo,  version);
        else if(suffixesTypePeople.contains(file.suffix().toLower()))   setType(DocumentTypePeople, version);
        else                                                            setType(DocumentTypeFile,   version);
        */

        if(debug) {
            QHashIterator<QString, QString> i(retour);
            while (i.hasNext()) {
                i.next();
                qDebug("%s\t\t%s", qPrintable(i.key()), qPrintable(i.value()));
            }
        }
    }
    return retour;
}
QString Global::getFileHash(const QFileInfo &file) {
    QCryptographicHash fileHasher(QCryptographicHash::Sha1);
    QFile fileToHash(file.absoluteFilePath());
    fileToHash.open(QFile::ReadOnly);
    while(!fileToHash.atEnd())
        fileHasher.addData(fileToHash.read(8192));
    return QString(fileHasher.result().toHex()).toUpper();
}

void Global::revealInFinder(const QFileInfo &filename) {
#ifdef Q_OS_MAC
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(filename.absoluteFilePath());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
}
void Global::openFile(const QFileInfo &filename) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename.absoluteFilePath()));
}



qreal Global::getDuration(const QString &duration) {
    qreal durationReal = 0;
    QString durationCleaned = duration.split("(").first().trimmed();
    if(durationCleaned.contains(":")) {
        QStringList durationsCleaned = durationCleaned.split(":");
        if(durationsCleaned.length() > 2)
            durationReal = durationsCleaned.at(0).toDouble() * 3600. + durationsCleaned.at(1).toDouble() * 60. + durationsCleaned.at(2).toDouble();
    }
    else {
        durationCleaned = durationCleaned.split(" ").first().trimmed();
        durationReal = durationCleaned.toDouble();
    }
    return durationReal;
}

const QString Global::timeToString(qreal time, bool millisec) {
    QString timeStr = "";

    quint16 min = time / 60;
    if(min < 10)        timeStr += "0";
    else if(min < 100)  timeStr += "";
    timeStr += QString::number(min) + ":";

    quint8 sec = qFloor(time) % 60;
    if(sec < 10) timeStr += "0";
    timeStr += QString::number(sec);

    if(millisec)
        timeStr += QString(":%1").arg((qint16)((time - (qreal)sec - (qreal)min * 60) * 1000), 3, 10, QChar('0'));

    return timeStr;
}
qreal Global::stringToTime(const QString &timeStr) {
    qreal time = 0;
    if(timeStr.count()) {
        QStringList timeParts = timeStr.split(":");
        if(timeParts.count() > 2) {
            qreal millisec = timeParts.at(2).toDouble();
            if(timeParts.at(2).count() == 1)
                millisec *= 100;
            else if(timeParts.at(2).count() == 2)
                millisec *= 10;
            time = timeParts.at(0).toDouble() * 60 + timeParts.at(1).toDouble() + millisec / 1000.;
        }
        else if(timeParts.count() > 1)
            time = timeParts.at(0).toDouble() * 60 + timeParts.at(1).toDouble();
        else if(timeParts.count() > 0)
            time = timeParts.at(0).toDouble() * 60;
    }
    return time;
}

const QString Global::dateToString(const QDateTime &date, bool addExactTime) {
    quint16 daysTo   = date.daysTo(QDateTime::currentDateTime());
    quint16 secsTo   = date.secsTo(QDateTime::currentDateTime());
    quint16 minsTo   = secsTo / 60;
    quint16 hoursTo  = secsTo / 3600;
    quint16 weeksTo  = daysTo / 7;
    quint16 monthsTo = daysTo / 30;

    if(addExactTime) {
        if(monthsTo > 12)     return QString("on %1").arg(date.toString("dddd dd MM yyyy, hh:mm"));
        else if(monthsTo > 1) return QString("%1 ago, on %2").arg(plurial(monthsTo, "month" )).arg(date.toString("dddd dd MM hh:mm"));
        else if(weeksTo > 1)  return QString("%1 ago, on %2").arg(plurial(weeksTo,  "week"  )).arg(date.toString("dddd dd MM hh:mm"));
        else if(daysTo > 1)   return QString("%1 ago, on %2").arg(plurial(daysTo,   "day"   )).arg(date.toString("dddd, hh:mm"));
        else if(hoursTo > 1)  return QString("%1 ago, on %2").arg(plurial(hoursTo,  "hour"  )).arg(date.toString("hh:mm"));
        else if(minsTo  > 1)  return QString("%1 ago, on %2").arg(plurial(minsTo,   "minute")).arg(date.toString("hh:mm"));
        else if(secsTo  > 10) return QString("%1 ago, on %2").arg(plurial(secsTo,   "second")).arg(date.toString("hh:mm:ss"));
        else                  return QString("A few seconds ago, on %1").arg(date.toString("hh:mm:ss"));
    }
    else {
        if(monthsTo > 12)     return QString("on %1").arg(date.toString("dddd dd MM yyyy, hh:mm"));
        else if(monthsTo > 1) return QString("%1 ago").arg(plurial(monthsTo, "month" ));
        else if(weeksTo > 1)  return QString("%1 ago").arg(plurial(weeksTo,  "week"  ));
        else if(daysTo > 1)   return QString("%1 ago").arg(plurial(daysTo,   "day"   ));
        else if(hoursTo > 1)  return QString("%1 ago").arg(plurial(hoursTo,  "hour"  ));
        else if(minsTo  > 1)  return QString("%1 ago").arg(plurial(minsTo,   "minute"));
        else if(secsTo  > 10) return QString("%1 ago").arg(plurial(secsTo,   "second"));
        else                  return QString("A few seconds ago");
    }
}
const QString Global::plurial(qint16 value, const QString &text) {
    if(qAbs(value) > 1) return QString("%1 %2s").arg(value).arg(text);
    else                return QString("%1 %2") .arg(value).arg(text);
}

void Global::qSleep(int ms) {
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}




SyncEntryEvent::SyncEntryEvent(SyncEntry *_file, QObject *parent) : QObject(parent) {
    action       = _file->action;
    file         = *_file;
    author       = _file->userInfos->getAuthor();
    locationName = _file->userInfos->getLocationName();
    locationGPS  = _file->userInfos->getLocationGPS();
    dateTime     = _file->userInfos->getDateTime();
    trayGlobalAction  = new QAction("action", this);
    trayProjectAction = new QAction("action", this);
    connect(trayGlobalAction,  SIGNAL(triggered()), SLOT(trayMenuTriggered()));
    connect(trayProjectAction, SIGNAL(triggered()), SLOT(trayMenuTriggered()));
    updateGUI();
}
SyncEntryEvent::SyncEntryEvent(const QDomElement &xmlElement, QObject *parent) : QObject(parent) {
    deserialize(xmlElement);
    trayGlobalAction  = new QAction("action", this);
    trayProjectAction = new QAction("action", this);
    connect(trayGlobalAction,  SIGNAL(triggered()), SLOT(trayMenuTriggered()));
    connect(trayProjectAction, SIGNAL(triggered()), SLOT(trayMenuTriggered()));
    updateGUI();
}
void SyncEntryEvent::updateGUI() {
    verbose = QString("%1").arg(file.baseName());
    if     (action == SyncCreate)    verbose += " added";
    else if(action == SyncDelete)    verbose += " removed";
    else                             verbose += " updated";
    if(!author.isEmpty())
        verbose += tr(" by %1").arg(author);
    if(!locationName.isEmpty())
        verbose += tr(" at %1").arg(locationName);
    verbose += tr(", %1").arg(Global::dateToString(dateTime));
    trayGlobalAction ->setEnabled(file.exists());
    trayProjectAction->setEnabled(file.exists());
    trayGlobalAction ->setText(verbose);
    trayProjectAction->setText(verbose);
}
QDomElement SyncEntryEvent::serialize(QDomDocument &xmlDoc) const {
    QDomElement xmlEvent = xmlDoc.createElement("event");

    xmlEvent.setAttribute("action",       action);
    xmlEvent.setAttribute("path",         file.absoluteFilePath());
    xmlEvent.setAttribute("author",       author);
    xmlEvent.setAttribute("locationName", locationName);
    xmlEvent.setAttribute("locationGPS",  locationGPS);
    xmlEvent.setAttribute("dateTime",     dateTime.toString("yyyy:MM:dd hh:mm:ss"));
    return xmlEvent;
}
void SyncEntryEvent::deserialize(const QDomElement &xmlElement) {
    action       = (SyncAction)xmlElement.attribute("action").toInt();
    file         = QFileInfo(xmlElement.attribute("path"));
    author       = xmlElement.attribute("author");
    locationName = xmlElement.attribute("locationName");
    locationGPS  = xmlElement.attribute("locationGPS");
    dateTime     = QDateTime::fromString(xmlElement.attribute("dateTime"), "yyyy:MM:dd hh:mm:ss");
}
void SyncEntryEvent::trayMenuTriggered() {
    Global::revealInFinder(file);
}



