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

#include "metadata.h"

QStringList Metadata::suffixesTypeVideo;
QStringList Metadata::suffixesTypeDoc;
QStringList Metadata::suffixesTypeImage;
QStringList Metadata::suffixesTypeAudio;
QStringList Metadata::suffixesTypePatches;
QStringList Metadata::suffixesTypePeople;

Metadata::Metadata(QObject *parent, bool createEmpty) :
    QObject(parent) {
    metadataMutex    = false;
    chutierItem      = 0;
    status           = DocumentStatusReady;
    if(!suffixesTypeDoc.count())
        suffixesTypeDoc << "pdf" << "ps" << "doc" << "txt" << "docx";
    if(!suffixesTypeImage.count())
        suffixesTypeImage << "png" << "jpg" << "jpeg" << "tif" << "tiff" << "bmp" << "gif";
    if(!suffixesTypeAudio.count())
        suffixesTypeAudio << "aac" << "aiff" << "aif" << "wav" << "mp3" << "m4a" << "ac3";
    if(!suffixesTypePatches.count())
        suffixesTypePatches << "pde" << "maxpat" << "sc" << "pd" << "iannix";
    if(!suffixesTypePeople.count())
        suffixesTypePeople << "vcf";
    if(!suffixesTypeVideo.count())
        suffixesTypeVideo << "3g2" << "3gp" << "4xm" << "a64" << "act" << "adf" << "adts" << "adx" << "aea" << "aiff" << "alaw" << "amr" << "anm" << "apc" << "ape" << "asf" << "asf_stream" << "ass" << "au" << "avi" << "avm2" << "avs" << "bethsoftvid" << "bfi" << "bin" << "bink" << "bit" << "bmv" << "c93" << "caf" << "cavsvideo" << "cdg" << "cdxl" << "crc" << "daud" << "dfa" << "dirac" << "dnxhd" << "dsicin" << "dts" << "dv" << "dvd" << "dxa" << "ea" << "ea_cdata" << "eac3" << "f32be" << "f32le" << "f4v" << "f64be" << "f64le" << "ffm" << "ffmetadata" << "film_cpk" << "filmstrip" << "flac" << "flic" << "flv" << "framecrc" << "framemd5" << "g722" << "g723_1" << "g729" << "gif" << "gsm" << "gxf" << "h261" << "h263" << "h264" << "hls" << "applehttp" << "ico" << "idcin" << "idf" << "iff" << "ilbc" << "image2" << "image2pipe" << "ingenient" << "ipmovie" << "ipod" << "ismv" << "iss" << "iv8" << "ivf" << "jacosub" << "jv" << "latm" << "lavfi" << "libmodplug" << "lmlm4" << "loas" << "lxf" << "m4v" << "matroska" << "matroska" << "webm" << "md5" << "mgsts" << "microdvd" << "mjpeg" << "mkvtimestamp_v2" << "mlp" << "mm" << "mmf" << "mov" << "mp4" << "3gp" << "mp2"<< "mp4" << "mpc" << "mpc8" << "mpg" << "mpeg" << "mpeg1video" << "mpeg2video" << "mpegts" << "mpegtsraw" << "mpegvideo" << "mpjpeg" << "msnwctcp" << "mtv" << "mulaw" << "mvi" << "mxf" << "mxf_d10" << "mxg" << "nc" << "nsv" << "null" << "nut" << "nuv" << "ogg" << "oma" << "paf" << "pmp" << "psp" << "psxstr" << "pva" << "qcp" << "r3d" << "rawvideo" << "rcv" << "realtext" << "rl2" << "rm" << "roq" << "rpl" << "rso" << "rtp" << "rtsp" << "s16be" << "s16le" << "s24be" << "s24le" << "s32be" << "s32le" << "s8" << "sami" << "sap" << "sbg" << "sdl" << "sdp" << "segment" << "shn" << "siff" << "smjpeg" << "smk" << "smoothstreaming" << "smush" << "sol" << "sox" << "spdif" << "srt" << "stream_segment" << "s" << "subviewer" << "svcd" << "swf" << "thp" << "tiertexseq" << "tmv" << "truehd" << "tta" << "tty" << "txd" << "u16be" << "u16le" << "u24be" << "u24le" << "u32be" << "u32le" << "u8" << "vc1" << "vc1test" << "vcd" << "vmd" << "vob" << "voc" << "vqf" << "w64" << "wav" << "wc3movie" << "webm" << "webvtt" << "wsaud" << "wsvqa" << "wtv" << "wv" << "xa" << "xbin" << "xmv" << "xwma" << "yop" << "yuv4mpegpipe";

    if(createEmpty)
        metadatas.append(QMetaDictionnay());
}

Metadata::~Metadata() {
}

bool Metadata::updateImport(const QString &name, qint16 version) {
    bool anEmptyMetaWasCreated = false;
    if(version < 0) {
        metadatas.append(QMetaDictionnay());
        anEmptyMetaWasCreated = true;
    }

    if(!file.exists())
        setType(DocumentTypeMarker);
    setMetadata("Rekall", "Name",               name, version);
    setMetadata("Rekall", "Comments",           "",   version);
    //setMetadata("Rekall", "Comments (details)", "",   version);
    setMetadata("Rekall", "Author",        Global::userInfos->getInfo("User Name"), version);
    setMetadata("Rekall", "Import Author", Global::userInfos->getInfo("User Name"), version);
    setMetadata("Rekall", "Date/Time",          QDateTime::currentDateTime(), version);
    setMetadata("Rekall", "Import Date/Time",   QDateTime::currentDateTime(), version);
    setMetadata(Global::userInfos->getInfos());

    setFunction(DocumentFunctionContextual, version);
    if(name.toLower().contains("captation"))
        setFunction(DocumentFunctionRender, version);

    return anEmptyMetaWasCreated;
}
bool Metadata::updateFile(const QFileInfo &_file, const QDir &dirBase, qint16 version, quint16 falseInfoForTest) {
    file = _file;
    file.refresh();
    bool anEmptyMetaWasCreated = updateImport(file.baseName(), version);
    setType(DocumentTypeFile);

    QFileInfoList filesContext = file.absoluteDir().entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
    QString fileContextVerbose = "";
    foreach(const QFileInfo &fileContext, filesContext)
        fileContextVerbose += ((fileContext.isDir())?("[DIR] "):("")) + fileContext.fileName() + ", ";
    fileContextVerbose.chop(2);

    setMetadata("File",   "Basename",                file.baseName(), version);
    setMetadata("File",   "Owner",                   file.owner(),    version);
    setMetadata("File",   "File Creation Date/Time", file.created(),  version);
    setMetadata("File",   "File Access Date/Time",   file.lastRead(), version);
    if(falseInfoForTest > 0)    setMetadata("File", "File Modification Date/Time", file.lastModified().addMonths(falseInfoForTest), version);
    else                        setMetadata("File", "File Modification Date/Time", file.lastModified(), version);
    setMetadata("Rekall", "Date/Time", getMetadata("File", "File Modification Date/Time", version), version);
    setMetadata("Rekall", "Extension",                   file.suffix().toUpper(), version);
    //setMetadata("Rekall", "File context",                fileContextVerbose,      version);

    if(dirBase.exists()) {
        QDir dirBaseParent = dirBase;
        dirBaseParent.cdUp();
        setMetadata("Rekall", "Folder", QString(file.absoluteDir().absolutePath() + "/").remove(dirBaseParent.absolutePath() + "/"), version);
    }


    //Type
    if(     suffixesTypeDoc   .contains(file.suffix().toLower()))   setType(DocumentTypeDoc,    version);
    else if(suffixesTypeAudio .contains(file.suffix().toLower()))   setType(DocumentTypeAudio,  version);
    else if(suffixesTypeImage .contains(file.suffix().toLower()))   setType(DocumentTypeImage,  version);
    else if(suffixesTypeVideo .contains(file.suffix().toLower()))   setType(DocumentTypeVideo,  version);
    else if(suffixesTypePeople.contains(file.suffix().toLower()))   setType(DocumentTypePeople, version);
    else                                                            setType(DocumentTypeFile,   version);

    QStringList documentKeywords;
    QStringList fileTags = QDir(Global::pathDocuments.absoluteFilePath() + "/").relativeFilePath(file.absoluteFilePath()).remove(file.suffix()).toLower().replace("-", " ").replace("_", " ").split("/", QString::SkipEmptyParts);
    QStringList forbiddenTags = QStringList() << ".." << "rekall";
    foreach(const QString &fileTag, fileTags) {
        QStringList tags = fileTag.split(QRegExp("\\b"), QString::SkipEmptyParts);
        foreach(QString tag, tags) {
            tag = tag.trimmed();
            if((tag.length() > 2) && (!forbiddenTags.contains(tag)))
                documentKeywords << tag;
        }
    }
    addKeyword(documentKeywords, version);

    if(anEmptyMetaWasCreated) {
        status = DocumentStatusWaiting;
        Global::taskList->addTask(this, TaskProcessTypeMetadata, version);
    }

    return anEmptyMetaWasCreated;
}

bool Metadata::updateCard(const PersonCard &card, qint16 version) {
    bool anEmptyMetaWasCreated = updateImport(card.getFullname(), version);
    setType(DocumentTypePeople, version);

    QString previousLabel = "";
    for(quint16 i = 0 ; i < card.count() ; i++) {
        const QPair<PersonCardHeader, PersonCardValues> cardInfo = card.at(i);
        if(cardInfo.first.category != "photo") {
            QString key = cardInfo.first.toString();
            QString value = cardInfo.second.toString();

            QStringList keySplitType = key.split("(type=");
            QStringList keySplitInfo = keySplitType.at(0).split(".");

            QString intitule = keySplitInfo.at(keySplitInfo.count()-1).trimmed();
            QString intituleDetail;
            if(keySplitType.count() > 1)
                intituleDetail = keySplitType[1].split(",").first().remove(")").trimmed().toLower();

            QMap<QString,QString> intituleRemplacements;
            intituleRemplacements.insert("adr",             "Address");
            intituleRemplacements.insert("email",           "Email");
            intituleRemplacements.insert("org",             "Organization");
            intituleRemplacements.insert("tel",             "Phone Number");
            intituleRemplacements.insert("title",           "Role");
            intituleRemplacements.insert("note",            "Note");
            intituleRemplacements.insert("url",             "URL");
            intituleRemplacements.insert("bday",            "Birthday");
            intituleRemplacements.insert("lang",            "Langage");
            intituleRemplacements.insert("x-socialprofile", "Social Profile");
            intituleRemplacements.insert("x-abadr",         "");
            intituleRemplacements.insert("prodid",          "");
            intituleRemplacements.insert("version",         "");
            intituleRemplacements.insert("n",               "");
            intituleRemplacements.insert("fn",              "");
            intituleRemplacements.insert("impp",            "");
            intituleRemplacements.insert("uid",             "");
            intituleRemplacements.insert("x-abuid",         "");
            intituleRemplacements.insert("x-abshowas",      "");
            intituleRemplacements.insert("catagories",      "");
            if(intituleRemplacements.contains(intitule)) intitule = intituleRemplacements.value(intitule);

            if(!intitule.isEmpty()) {
                if(!previousLabel.isEmpty())        intitule = QString("%1 (%2)").arg(intitule).arg(previousLabel);
                else if(!intituleDetail.isEmpty())  intitule = QString("%1 (%2)").arg(intitule).arg(intituleDetail);
                else                                intitule = QString("%1").arg(intitule);

                if(intitule == "x-ablabel")
                    previousLabel = cardInfo.second.toString().trimmed().remove("_$!<").remove(">!$_").toLower();
                else {
                    setMetadata("Contact details", intitule, value.replace("\\n", " ").replace("\n", " ").replace(";", " ").trimmed(), version);
                    previousLabel.clear();
                }
            }
        }
    }
    photo = card.getPhoto();

    return anEmptyMetaWasCreated;
}

bool Metadata::updateWeb(const QString &url, qint16 version) {
    bool anEmptyMetaWasCreated = updateImport(url, version);
    setMetadata("Rekall", "URL", url, version);
    setType(DocumentTypeWeb, version);

    if(anEmptyMetaWasCreated) {
        status = DocumentStatusWaiting;
        Global::taskList->addTask(this, TaskProcessTypeMetadata, version);
    }

    return anEmptyMetaWasCreated;
}


void Metadata::addKeyword(const QStringList &keywords, qint16 version, const QString &key, const QString &category) {
    if(keywords.count()) {
        QStringList documentKeywords = getMetadata(category, key, version).toString().split(",", QString::SkipEmptyParts);
        documentKeywords << keywords;
        documentKeywords.removeDuplicates();
        QString documentKeywordsStr;
        foreach(const QString &documentKeyword, documentKeywords)
            documentKeywordsStr += documentKeyword.trimmed().toLower() + ", ";
        documentKeywordsStr.chop(2);
        setMetadata(category, key, documentKeywordsStr, version);
    }
}
void Metadata::addKeyword(const QString &keyword, qint16 version, const QString &key, const QString &category) {
    addKeyword(QStringList() << keyword, version, key, category);
}


void Metadata::updateFeed() {
    FeedItemBaseType feedAction = FeedItemBaseTypeCreation;
    if(getMetadataCount() > 1)
        feedAction = FeedItemBaseTypeUpdate;
    Global::feedList->addFeed(new FeedItemBase(feedAction, getUserName(), getName(), getMetadata("Rekall", "Import Date/Time").toDateTime()));
}


const MetadataElement Metadata::getMetadata(const QString &key, qint16 version) const {
    MetadataElement retour;

    while(metadataMutex) {
        qDebug("WAIT MUTEX");
    }

    if(metadatas.count()) {
        QMapIterator<QString, QMetaMap> metaIterator(getMetadata(version));
        while(metaIterator.hasNext()) {
            metaIterator.next();
            if(metaIterator.value().contains(key)) {
                retour = metaIterator.value().value(key);
                break;
            }
        }
    }
    return retour;
}
const MetadataElement Metadata::getMetadata(const QString &category, const QString &key, qint16 version) const {
    if(category.isEmpty())
        return getMetadata(key, version);
    MetadataElement retour;

    while(metadataMutex) {
        qDebug("WAIT MUTEX");
    }

    if(metadatas.count()) {
        if(key == "All") {
            QString retourStr;
            QMapIterator<QString, MetadataElement> metaIterator(getMetadata(version).value(category));
            while(metaIterator.hasNext()) {
                metaIterator.next();
                if(!metaIterator.value().toString().isEmpty())
                    retourStr += metaIterator.value().toString().trimmed().toLower() + ", ";
            }
            retourStr.chop(2);
            retour = retourStr;
        }
        else {
            if((getMetadata(version).contains(category)) && (getMetadata(version).value(category).contains(key)))
                retour = getMetadata(version).value(category).value(key);
        }
    }
    return retour;
}


void Metadata::setMetadata(const QString &category, const QString &key, const QString &value, qint16 version) {
    if(key.toLower().contains("date"))
        setMetadata(category, key, QDateTime::fromString(value, "yyyy:MM:dd hh:mm:ss"), version);
    else if(true) {
        version = getMetadataIndexVersion(version);
        metadataMutex = true;
        metadatas[version][category][key] = value;
        metadataMutex = false;
        getCacheRefreshed(version);
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, const QDateTime &value, qint16 version) {
    if(true) {
        version = getMetadataIndexVersion(version);
        metadataMutex = true;
        metadatas[version][category][key] = value;
        metadataMutex = false;
        getCacheRefreshed(version);
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, const MetadataElement &value, qint16 version) {
    if(true) {
        version = getMetadataIndexVersion(version);
        metadataMutex = true;
        metadatas[version][category][key] = value;
        metadataMutex = false;
        getCacheRefreshed(version);
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, qreal value, qint16 version) {
    setMetadata(category, key, QString::number(value), version);
}
void Metadata::setMetadata(const QMetaDictionnay &metaDictionnay) {
    QMapIterator<QString, QMetaMap> categoryIterator(metaDictionnay);
    while(categoryIterator.hasNext()) {
        categoryIterator.next();
        QMapIterator<QString, MetadataElement> metaIterator(categoryIterator.value());
        while(metaIterator.hasNext()) {
            metaIterator.next();
            setMetadata(categoryIterator.key(), metaIterator.key(), metaIterator.value(), -1);
        }
    }
}



bool Metadata::isAcceptableWithSortFilters(bool strongCheck, qint16 version) const {
    return (getFunction() == DocumentFunctionRender) ||
            (   (Global::phases                  ->isAcceptable(true,        getCriteriaPhase(version)))
                && (Global::tagFilterCriteria    ->isAcceptable(true,        getCriteriaFilter(version)))
                && (Global::tagSortCriteria      ->isAcceptable(strongCheck, getCriteriaSort(version).toLower()))
                && (Global::tagHorizontalCriteria->isAcceptable(true,        getCriteriaHorizontal(version))));
}
bool Metadata::isAcceptableWithColorFilters(bool strongCheck, qint16 version) const {
    return (getFunction() == DocumentFunctionContextual)
            && (Global::phases               ->isAcceptable(true,        getCriteriaPhase(version)))
            && (Global::tagFilterCriteria    ->isAcceptable(true,        getCriteriaFilter(version)))
            && (Global::tagSortCriteria      ->isAcceptable(true,        getCriteriaSort(version).toLower()))
            && (Global::tagColorCriteria     ->isAcceptable(strongCheck, getCriteriaColor(version)))
            && (Global::tagHorizontalCriteria->isAcceptable(true,        getCriteriaHorizontal(version)));
}
bool Metadata::isAcceptableWithTextFilters(bool strongCheck, qint16 version) const {
    return (getFunction() == DocumentFunctionContextual)
            && (Global::phases               ->isAcceptable(true,        getCriteriaPhase(version)))
            && (Global::tagFilterCriteria    ->isAcceptable(true,        getCriteriaFilter(version)))
            && (Global::tagSortCriteria      ->isAcceptable(true,        getCriteriaSort(version).toLower()))
            && (Global::tagTextCriteria      ->isAcceptable(strongCheck, getCriteriaText(version)))
            && (Global::tagHorizontalCriteria->isAcceptable(true,        getCriteriaHorizontal(version)));
}
bool Metadata::isAcceptableWithClusterFilters(bool strongCheck, qint16 version) const {
    return (getFunction() == DocumentFunctionContextual)
            && (Global::phases               ->isAcceptable(true,        getCriteriaPhase(version)))
            && (Global::tagFilterCriteria    ->isAcceptable(true,        getCriteriaFilter(version)))
            && (Global::tagSortCriteria      ->isAcceptable(true,        getCriteriaSort(version).toLower()))
            && (Global::tagClusterCriteria   ->isAcceptable(strongCheck, getCriteriaCluster(version)))
            && (Global::tagHorizontalCriteria->isAcceptable(true,        getCriteriaHorizontal(version)));
}
bool Metadata::isAcceptableWithFilterFilters(bool strongCheck, qint16 version) const {
    return (    Global::phases               ->isAcceptable(true,        getCriteriaPhase(version)))
            && (Global::tagFilterCriteria    ->isAcceptable(strongCheck, getCriteriaFilter(version)))
            && (Global::tagSortCriteria      ->isAcceptable(true,        getCriteriaSort(version).toLower()))
            && (Global::tagHorizontalCriteria->isAcceptable(true,        getCriteriaHorizontal(version)));
}
bool Metadata::isAcceptableWithHorizontalFilters(bool strongCheck, qint16 version) const {
    return (    Global::phases               ->isAcceptable(true,        getCriteriaPhase(version)))
            && (Global::tagFilterCriteria    ->isAcceptable(true,        getCriteriaFilter(version)))
            && (Global::tagSortCriteria      ->isAcceptable(true,        getCriteriaSort(version).toLower()))
            && (Global::tagHorizontalCriteria->isAcceptable(strongCheck, getCriteriaHorizontal(version)));
}



const QString Metadata::getAcceptableWithClusterFilters(qint16 version) const {
    return Global::tagClusterCriteria->getAcceptableWithFilters(getCriteriaCluster(version));
}



const QString Metadata::getCriteriaColor(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagColorCriteria->getCriteria(getMetadata(Global::tagColorCriteria->getTagNameCategory(), Global::tagColorCriteria->getTagName(), version).toString(Global::tagColorCriteria->getTrunctionLeft(), Global::tagColorCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaColorFormated(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagColorCriteria->getCriteriaFormated(getCriteriaColor(version));
}
const QString Metadata::getCriteriaText(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagTextCriteria->getCriteria(getMetadata(Global::tagTextCriteria->getTagNameCategory(), Global::tagTextCriteria->getTagName(), version).toString(Global::tagTextCriteria->getTrunctionLeft(), Global::tagTextCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaTextFormated(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagTextCriteria->getCriteriaFormated(getCriteriaText(version));
}
const QString Metadata::getCriteriaCluster(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagClusterCriteria->getCriteria(getMetadata(Global::tagClusterCriteria->getTagNameCategory(), Global::tagClusterCriteria->getTagName(), version).toString(Global::tagClusterCriteria->getTrunctionLeft(), Global::tagClusterCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaClusterFormated(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagClusterCriteria->getCriteriaFormated(getCriteriaCluster(version));
}
const QString Metadata::getCriteriaFilter(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagFilterCriteria->getCriteria(getMetadata(Global::tagFilterCriteria->getTagNameCategory(), Global::tagFilterCriteria->getTagName(), version).toString(Global::tagFilterCriteria->getTrunctionLeft(), Global::tagFilterCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaFilterFormated(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) return QString();
    return Global::tagFilterCriteria->getCriteriaFormated(getCriteriaFilter(version));
}
const QString Metadata::getCriteriaHorizontal(qint16 version) const {
    return Global::tagHorizontalCriteria->getCriteria(getMetadata(Global::tagHorizontalCriteria->getTagNameCategory(), Global::tagHorizontalCriteria->getTagName(), version).toString(Global::tagHorizontalCriteria->getTrunctionLeft(), Global::tagHorizontalCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaHorizontalFormated(qint16 version) const {
    return Global::tagHorizontalCriteria->getCriteriaFormated(getCriteriaHorizontal(version));
}

const QString Metadata::getCriteriaSort(qint16 version) const {
    if(getFunction() == DocumentFunctionRender) {
        if(Global::tagSortCriteria->isDate())
            return getMetadata(Global::tagSortCriteria->getTagNameCategory(), Global::tagSortCriteria->getTagName(), version).toString(Global::tagSortCriteria->getTrunctionLeft(), Global::tagSortCriteria->getTrunctionLength()) + "\n" + getName(version);
        else
            return "\n" + getName(version);
    }
    else
        return Global::tagSortCriteria->getCriteria(getMetadata(Global::tagSortCriteria->getTagNameCategory(), Global::tagSortCriteria->getTagName(), version).toString(Global::tagSortCriteria->getTrunctionLeft(), Global::tagSortCriteria->getTrunctionLength()));
}
const QString Metadata::getCriteriaSortFormated(qint16 version) const {
    return Global::tagSortCriteria->getCriteriaFormated(getCriteriaSort(version));
}
const MetadataElement Metadata::getCriteriaPhase(qint16 version) const {
    return getMetadata(Global::phases->getTagNameCategory(), Global::phases->getTagName(), version);
}

const QPair<QString, QPixmap> Metadata::getThumbnail(qint16 version) const {
    if(!photo.isNull())
        return qMakePair(QString(), QPixmap::fromImage(photo));
    else if((getType(version) == DocumentTypeWeb) && (thumbnails.count()))
        return qMakePair(getMetadata("Rekall", "URL", version).toString(), QPixmap::fromImage(thumbnails.first().image));
    else if(getSnapshot(version) == "comment")
        return qMakePair(QString("%1_%2.jpg").arg(Global::cacheFile("comment", file)).arg(version), QPixmap(QString("%1_%2.jpg").arg(Global::cacheFile("comment", file)).arg(version)));
    else if(getSnapshot(version) == "note")
        return qMakePair(QString("%1_%2.jpg").arg(Global::cacheFile("note", getMetadata("Rekall", "Note ID").toString())).arg(version), QPixmap(QString("%1_%2.jpg").arg(Global::cacheFile("note", getMetadata("Rekall", "Note ID").toString())).arg(version)));
    else if(getSnapshot(version) == "file")
        return qMakePair(file.absoluteFilePath(), QPixmap(file.absoluteFilePath()));
    else if((thumbnails.count()) && (file.exists()))
        return qMakePair(file.absoluteFilePath(), QPixmap::fromImage(thumbnails.first().image));
    else if(thumbnails.count())
        return qMakePair(thumbnails.first().currentFilename, QPixmap::fromImage(thumbnails.first().image));
    else
        return qMakePair(file.absoluteFilePath(), QPixmap(file.absoluteFilePath()));
}

const QList<QPair<QString, QString> > Metadata::getGps(qint16 version) const {
    if((getType() == DocumentTypePeople) && (getMetadata(version).contains("Contact details"))) {
        QMapIterator<QString, MetadataElement> metaIterator(getMetadata(version).value("Contact details"));
        QList<QPair<QString,QString> > gpsList;
        while(metaIterator.hasNext()) {
            metaIterator.next();
            if(metaIterator.key().contains("Address")) {
                QPair<QString,QString> gpsPeople;
                gpsPeople.first  = metaIterator.value().toString();
                gpsPeople.second = metaIterator.key();
                gpsList << gpsPeople;
            }
        }
        return gpsList;
    }

    QPair<QString,QString> gps;
    gps.first  = getMetadata("GPS Coordinates", version).toString();
    gps.second = getName(version);
    if(gps.first.isEmpty()) {
        gps.first  = getMetadata("Rekall User Infos", "Location GPS", version).toString();
        gps.second = getMetadata("Rekall User Infos", "Location Place", version).toString();
    }
    return (QList< QPair<QString,QString> >() << gps);
}

void Metadata::debug() {
    qDebug("------------------------------");
    foreach(const QMetaDictionnay & metaDictionnay, metadatas) {
        QMapIterator<QString, QMetaMap> categoryIterator(metaDictionnay);
        while(categoryIterator.hasNext()) {
            categoryIterator.next();
            QMapIterator<QString, MetadataElement> metaIterator(categoryIterator.value());
            qDebug("\t%s", qPrintable(categoryIterator.key()));
            while(metaIterator.hasNext()) {
                metaIterator.next();
                qDebug("\t\t%s\t:\t%s", qPrintable(metaIterator.key()), qPrintable(metaIterator.value().toString()));
            }
        }
    }
    qDebug("------------------------------");
}
QDomElement Metadata::serializeMetadata(QDomDocument &xmlDoc) const {
    QDomElement xmlData = xmlDoc.createElement("metadata");
    //xmlData.setAttribute("file", file.absoluteFilePath());
    quint16 version = 0;
    foreach(const QMetaDictionnay & metaDictionnay, metadatas) {
        QMapIterator<QString, QMetaMap> categoryIterator(metaDictionnay);
        while(categoryIterator.hasNext()) {
            categoryIterator.next();
            QMapIterator<QString, MetadataElement> metaIterator(categoryIterator.value());
            while(metaIterator.hasNext()) {
                metaIterator.next();
                QDomElement xmlMeta = xmlDoc.createElement("meta");
                xmlMeta.setAttribute("category", categoryIterator.key());
                xmlMeta.setAttribute("tagname",  metaIterator.key());
                xmlMeta.setAttribute("name",     QString("%1.%2").arg(categoryIterator.key()).arg(metaIterator.key()));
                xmlMeta.setAttribute("content",  metaIterator.value().toString());
                xmlMeta.setAttribute("documentVersion", version);
                xmlData.appendChild(xmlMeta);
            }
        }
        version++;
    }
    return xmlData;
}
void Metadata::deserializeMetadata(const QDomElement &xmlElement) {
    metadatas.append(QMetaDictionnay());
    QDomNode metadataNode = xmlElement.firstChild();
    while(!metadataNode.isNull()) {
        QDomElement metadataElement = metadataNode.toElement();
        if((!metadataElement.isNull()) && (metadataElement.nodeName() == "metadata")) {
            QDomNode metaNode = metadataElement.firstChild();
            while(!metaNode.isNull()) {
                QDomElement metaElement = metaNode.toElement();
                if((!metaElement.isNull()) && (metaElement.nodeName() == "meta")) {
                    setMetadata(metaElement.attribute("category"), metaElement.attribute("tagname"), metaElement.attribute("content"), metaElement.attribute("documentVersion", "-1").toInt());
                }
                metaNode = metaNode.nextSibling();
            }
        }
        metadataNode = metadataNode.nextSibling();
    }
}


