#include "metadata.h"

QStringList Metadata::suffixesTypeVideo;
QStringList Metadata::suffixesTypeDoc;
QStringList Metadata::suffixesTypeImage;
QStringList Metadata::suffixesTypeAudio;
QStringList Metadata::suffixesTypePatches;

Metadata::Metadata(QObject *parent, bool createEmpty) :
    QObject(parent) {
    type     = DocumentTypeMarker;
    function = DocumentFunctionContextual;
    mediaDuration = 0;
    if(!suffixesTypeDoc.count())
        suffixesTypeDoc << "pdf" << "ps" << "doc" << "txt" << "docx";
    if(!suffixesTypeImage.count())
        suffixesTypeImage << "png" << "jpg" << "jpeg" << "tif" << "tiff" << "bmp" << "gif";
    if(!suffixesTypeAudio.count())
        suffixesTypeAudio << "aac" << "aiff" << "aif" << "wav" << "mp3" << "m4a" << "ac3";
    if(!suffixesTypePatches.count())
        suffixesTypePatches << "pde" << "maxpat" << "sc" << "pd" << "iannix";
    if(!suffixesTypeVideo.count())
        suffixesTypeVideo << "3g2" << "3gp" << "4xm" << "a64" << "act" << "adf" << "adts" << "adx" << "aea" << "aiff" << "alaw" << "amr" << "anm" << "apc" << "ape" << "asf" << "asf_stream" << "ass" << "au" << "avi" << "avm2" << "avs" << "bethsoftvid" << "bfi" << "bin" << "bink" << "bit" << "bmv" << "c93" << "caf" << "cavsvideo" << "cdg" << "cdxl" << "crc" << "daud" << "dfa" << "dirac" << "dnxhd" << "dsicin" << "dts" << "dv" << "dvd" << "dxa" << "ea" << "ea_cdata" << "eac3" << "f32be" << "f32le" << "f4v" << "f64be" << "f64le" << "ffm" << "ffmetadata" << "film_cpk" << "filmstrip" << "flac" << "flic" << "flv" << "framecrc" << "framemd5" << "g722" << "g723_1" << "g729" << "gif" << "gsm" << "gxf" << "h261" << "h263" << "h264" << "hls" << "applehttp" << "ico" << "idcin" << "idf" << "iff" << "ilbc" << "image2" << "image2pipe" << "ingenient" << "ipmovie" << "ipod" << "ismv" << "iss" << "iv8" << "ivf" << "jacosub" << "jv" << "latm" << "lavfi" << "libmodplug" << "lmlm4" << "loas" << "lxf" << "m4v" << "matroska" << "matroska" << "webm" << "md5" << "mgsts" << "microdvd" << "mjpeg" << "mkvtimestamp_v2" << "mlp" << "mm" << "mmf" << "mov" << "mov" << "mp4" << "3gp" << "mp2"<< "mp4" << "mpc" << "mpc8" << "mpeg" << "mpeg1video" << "mpeg2video" << "mpegts" << "mpegtsraw" << "mpegvideo" << "mpjpeg" << "msnwctcp" << "mtv" << "mulaw" << "mvi" << "mxf" << "mxf_d10" << "mxg" << "nc" << "nsv" << "null" << "nut" << "nuv" << "ogg" << "oma" << "paf" << "pmp" << "psp" << "psxstr" << "pva" << "qcp" << "r3d" << "rawvideo" << "rcv" << "realtext" << "rl2" << "rm" << "roq" << "rpl" << "rso" << "rtp" << "rtsp" << "s16be" << "s16le" << "s24be" << "s24le" << "s32be" << "s32le" << "s8" << "sami" << "sap" << "sbg" << "sdl" << "sdp" << "segment" << "shn" << "siff" << "smjpeg" << "smk" << "smoothstreaming" << "smush" << "sol" << "sox" << "spdif" << "srt" << "stream_segment" << "s" << "subviewer" << "svcd" << "swf" << "thp" << "tiertexseq" << "tmv" << "truehd" << "tta" << "tty" << "txd" << "u16be" << "u16le" << "u24be" << "u24le" << "u32be" << "u32le" << "u8" << "vc1" << "vc1test" << "vcd" << "vmd" << "vob" << "voc" << "vqf" << "w64" << "wav" << "wc3movie" << "webm" << "webvtt" << "wsaud" << "wsvqa" << "wtv" << "wv" << "xa" << "xbin" << "xmv" << "xwma" << "yop" << "yuv4mpegpipe";

    if(createEmpty)
        metadatas.append(QMetaDictionnay());
}

Metadata::~Metadata() {
}

bool Metadata::updateFile(const QFileInfo &_file, qint16 version, quint16 falseInfoForTest) {
    bool creation = false;
    file = _file;
    file.refresh();
    type = DocumentTypeFile;
    creation = updateImport(file.baseName(), version);
    if(creation)
        Global::taskList->addTask(this, TaskProcessTypeMetadata, version);

    QFileInfoList filesContext = file.absoluteDir().entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
    QString fileContextVerbose = "";
    foreach(const QFileInfo &fileContext, filesContext)
        fileContextVerbose += ((fileContext.isDir())?("[DIR] "):("")) + fileContext.fileName() + ", ";
    fileContextVerbose.chop(2);

    setMetadata("File",   "Basename",                    file.baseName(),         version);
    setMetadata("File",   "Owner",                       file.owner(),            version);
    setMetadata("File",   "File Type",                   file.suffix().toUpper(), version);
    setMetadata("File",   "File Raw Size",               file.size(),             version);
    setMetadata("File",   "File Creation Date/Time",     file.created(),          version);
    setMetadata("File",   "File Access Date/Time",       file.lastRead(),         version);
    setMetadata("Rekall", "File context",                fileContextVerbose,      version);
    if(falseInfoForTest > 0)    setMetadata("File", "File Modification Date/Time", file.lastModified().addMonths(falseInfoForTest), version);
    else                        setMetadata("File", "File Modification Date/Time", file.lastModified(), version);
    setMetadata("Rekall", "Document Date/Time", getMetadata("File", "File Modification Date/Time", version), version);

    QString documentTags;
    QStringList fileTags = QDir(Global::pathDocuments.absoluteFilePath() + "/").relativeFilePath(file.absoluteFilePath()).remove(file.suffix()).toLower().replace("-", " ").replace("_", " ").split("/", QString::SkipEmptyParts);
    QStringList forbiddenTags = QStringList() << ".." << "rekall";
    foreach(const QString &fileTag, fileTags) {
        QStringList tags = fileTag.split(QRegExp("\\b"), QString::SkipEmptyParts);
        foreach(QString tag, tags) {
            tag = tag.trimmed();
            if((tag.length() > 2) && (!forbiddenTags.contains(tag)))
                documentTags += tag + ", ";
        }
    }
    documentTags.chop(2);
    setMetadata("Rekall", "Document Tags", documentTags, version);

    if(suffixesTypeDoc.contains(file.suffix().toLower())) {
        type = DocumentTypeDoc;
        setMetadata("Rekall", "Document Category", "Document", version);
    }
    else if(suffixesTypeAudio.contains(file.suffix().toLower())) {
        type = DocumentTypeAudio;
        setMetadata("Rekall", "Document Category", "Audio",    version);
    }
    else if(suffixesTypeImage.contains(file.suffix().toLower())) {
        type = DocumentTypeImage;
        setMetadata("Rekall", "Document Category", "Image",    version);
    }
    else if(suffixesTypeVideo.contains(file.suffix().toLower())) {
        type = DocumentTypeVideo;
        setMetadata("Rekall", "Document Category", "Video",    version);
    }
    else if(suffixesTypePatches.contains(file.suffix().toLower())) {
        type = DocumentTypeFile;
        setMetadata("Rekall", "Document Category", "Patches",  version);
    }
    else {
        type = DocumentTypeFile;
        setMetadata("Rekall", "Document Category", "Other",    version);
    }
    if(function == DocumentFunctionRender) {
        setMetadata("Rekall", "Document Category", "Recording", version);
    }

    return creation;
}
bool Metadata::updateImport(const QString &name, qint16 version) {
    bool creation = false;
    if(version < 0) {
        metadatas.append(QMetaDictionnay());
        creation = true;
    }

    if(!file.exists())
        setMetadata("Rekall", "Document Category",  "Marker", version);
    setMetadata("Rekall", "Document Name",         name, version);
    setMetadata("Rekall", "Document Author",       Global::userInfos->getInfo("User Name"), version);
    setMetadata("Rekall", "Document Date/Time",    QDateTime::currentDateTime(), version);
    setMetadata("Rekall", "Import Date/Time",      QDateTime::currentDateTime(), version);
    setMetadata("Rekall", "Import User Name",           Global::userInfos->getInfo("User Name"), version);
    setMetadata("Rekall", "Import Location GPS",        Global::userInfos->getInfo("Location GPS"), version);
    setMetadata("Rekall", "Import Location Place",      Global::userInfos->getInfo("Location Place"), version);
    setMetadata("Rekall", "Import Weather Temperature", Global::userInfos->getInfo("Weather Temperature"), version);
    setMetadata("Rekall", "Import Weather Sky",         Global::userInfos->getInfo("Weather Sky"),  version);
    setMetadata("Rekall", "Import Weather Sky Icon",    Global::userInfos->getInfo("Weather Sky Icon"), version);

    //setMetadata("Rekall", "Document Danger", version);

    if(name.toLower().contains("captation"))
        function = DocumentFunctionRender;

    return creation;
}

const MetadataElement Metadata::getMetadata(const QString &key, qint16 version) {
    MetadataElement retour;
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
const MetadataElement Metadata::getMetadata(const QString &category, const QString &key, qint16 version) {
    MetadataElement retour;
    if(metadatas.count()) {
        if((getMetadata(version).contains(category)) && (getMetadata(version).value(category).contains(key)))
            retour = getMetadata(version).value(category).value(key);
    }
    return retour;
}


void Metadata::setMetadata(const QString &category, const QString &key, const QString &value, qint16 version) {
    if(key.toLower().contains("date"))
        setMetadata(category, key, QDateTime::fromString(value, "yyyy:MM:dd hh:mm:ss"), version);
    else if(!value.isEmpty()) {
        metadataMutex.lock();
        metadatas[getMetadataIndexVersion(version)][category][key] = value;
        metadataMutex.unlock();
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, const QDateTime &value, qint16 version) {
    if(!value.isNull()) {
        metadataMutex.lock();
        metadatas[getMetadataIndexVersion(version)][category][key] = value;
        metadataMutex.unlock();
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, const MetadataElement &value, qint16 version) {
    if(!value.toString().isEmpty()) {
        metadataMutex.lock();
        metadatas[getMetadataIndexVersion(version)][category][key] = value;
        metadataMutex.unlock();
    }
}
void Metadata::setMetadata(const QString &category, const QString &key, qreal value, qint16 version) {
    setMetadata(category, key, QString::number(value), version);
}


bool Metadata::isAcceptableWithSortFilters(qint16 version) {
    return (function == DocumentFunctionRender) || (Global::tagSortCriteria->isAcceptableWithFilters(getCriteriaSort(version)));
}
bool Metadata::isAcceptableWithColorFilters(qint16 version) {
    return (function == DocumentFunctionRender) || (Global::tagColorCriteria->isAcceptableWithFilters(getCriteriaColor(version)));
}
bool Metadata::isAcceptableWithClusterFilters(qint16 version) {
    return Global::tagClusterCriteria->isAcceptableWithFilters(getCriteriaCluster(version));
}
const QString Metadata::getAcceptableWithClusterFilters(qint16 version) {
    return Global::tagClusterCriteria->getAcceptableWithFilters(getCriteriaCluster(version));
}




const QString Metadata::getCriteriaSortFormated(qint16 version) {
    return Global::tagSortCriteria->getCriteriaFormated(getCriteriaSort(version));
}
const QString Metadata::getCriteriaColorFormated(qint16 version) {
    return Global::tagColorCriteria->getCriteriaFormated(getCriteriaColor(version));
}
const QString Metadata::getCriteriaClusterFormated(qint16 version) {
    return Global::tagClusterCriteria->getCriteriaFormated(getCriteriaCluster(version));
}
const QString Metadata::getCriteriaColor(qint16 version) {
    return getMetadata(Global::tagColorCriteria->tagName, version).toString(Global::tagColorCriteria->left);
}
const QString Metadata::getCriteriaSort(qint16 version) {
    if(function == DocumentFunctionRender) {
        if(Global::tagSortCriteria->asDate)
            return getMetadata(Global::tagSortCriteria->tagName, version).toString(Global::tagSortCriteria->left);//.arg(getMetadata("Rekall", "Document Name", version).toString());
        else
            return "\n" + getMetadata("Rekall", "Document Name", version).toString();
    }
    else
        return getMetadata(Global::tagSortCriteria->tagName, version).toString(Global::tagSortCriteria->left);
}
const QString Metadata::getCriteriaCluster(qint16 version) {
    return getMetadata(Global::tagClusterCriteria->tagName, version).toString(Global::tagClusterCriteria->left);
}
const MetadataElement Metadata::getCriteriaSortRaw(qint16 version) {
    return getMetadata(Global::tagSortCriteria->tagName, version);
}
const QPair<QString, QPixmap> Metadata::getThumbnail(qint16 version) {
    QPair<QString, QPixmap> retour;
    if(getMetadata("Rekall", "Snapshot", version).toString() == "Comment")
        return qMakePair(QString("%1_%2.jpg").arg(Global::cacheFile("comment", file)).arg(version), QPixmap(QString("%1_%2.jpg").arg(Global::cacheFile("comment", file)).arg(version)));
    else if(getMetadata("Rekall", "Snapshot", version).toString() == "Note")
        return qMakePair(QString("%1_%2.jpg").arg(Global::cacheFile("note", getMetadata("Rekall", "Note ID").toString())).arg(version), QPixmap(QString("%1_%2.jpg").arg(Global::cacheFile("note", getMetadata("Rekall", "Note ID").toString())).arg(version)));
    else if(getMetadata("Rekall", "Snapshot", version).toString() == "File")
        return qMakePair(file.absoluteFilePath(), QPixmap(file.absoluteFilePath()));
    else if(thumbnails.count())
        return qMakePair(file.absoluteFilePath(), QPixmap::fromImage(thumbnails.first().image));
    else
        return retour;
}

const QPair<QString, QString> Metadata::getGps() {
    QPair<QString,QString> gps;
    gps.first  = getMetadata("GPS Coordinates").toString();
    gps.second = getMetadata("Rekall", "Document Name").toString();
    if(gps.first.isEmpty()) {
        gps.first  = getMetadata("Rekall", "Import Location GPS").toString();
        gps.second = getMetadata("Rekall", "Import Location Place").toString();
    }
    if(gps.second.isEmpty())
        gps.second = getMetadata("Rekall", "Import Location Place").toString();
    return gps;
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
QDomElement Metadata::serializeMetadata(QDomDocument &xmlDoc) {
    QDomElement xmlData = xmlDoc.createElement("metadata");
    xmlData.setAttribute("function", function);
    xmlData.setAttribute("file", file.absoluteFilePath());
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
                xmlData.appendChild(xmlMeta);
            }
        }
    }
    return xmlData;
}
void Metadata::deserializeMetadata(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}


