#include "document.h"

Document::Document(ProjectBase *_project) :
    DocumentBase(_project) {
    project     = _project;
    project->addDocument(this);
}

Tag* Document::createTag(qint16 _versionSource, qint16 versionDest) {
    qint16 versionSource = getMetadataIndexVersion(_versionSource);
    Tag *newTag = 0;
    if(tags.count()) {
        QList<Tag*> tagsSource;
        foreach(Tag *tag, tags) {
            tag->setDocumentVersion(-1);
            if(tag->getDocumentVersion() == (versionSource-1))
                tagsSource << tag;
        }
        foreach(Tag *tagSource, tagsSource)
            createTag(tagSource, tagSource->getDocumentVersion()+1);
    }
    else {
        qreal tS = Global::aleaF(5, 120);
        if(getFunction() == DocumentFunctionRender)
            tS = Global::aleaF(0, 5);
        newTag = createTag(TagTypeContextualTime, tS, -1, versionDest, true);
    }
    return newTag;
}
Tag* Document::createTag(TagType type, qreal timeStart, qreal duration, qint16 version, bool debug) {
    Tag *newTag = new Tag(this, version);
    newTag->init(type, timeStart, duration, debug);
    tags.append(newTag);
    return newTag;
}
Tag* Document::createTag(Tag *tagSource, qint16 version) {
    if(version < -1)
        version = tagSource->getDocumentVersionRaw();
    Tag *newTag = createTag(tagSource->getType(), tagSource->getTimeStart(), tagSource->getDuration(), version);
    newTag->setTimeMediaOffset(tagSource->getTimeMediaOffset());
    newTag->timelinePos     = tagSource->timelinePos;
    newTag->timelineDestPos = tagSource->timelineDestPos;
    newTag->viewerPos       = tagSource->viewerPos;
    newTag->viewerDestPos   = tagSource->viewerDestPos;
    return newTag;
}
void Document::removeTag(void *tag) {
    removeTag((Tag*)tag);
}
void Document::removeTag(Tag *tag) {
    tags.removeOne(tag);
    //delete tag;
}

QDomElement Document::serialize(QDomDocument &xmlDoc) const {
    QDomElement xmlData = xmlDoc.createElement("document");
    if(tags.count()) {
        QDomElement tagsXml = xmlDoc.createElement("tags");
        foreach(Tag *tag, tags)
            tagsXml.appendChild(tag->serialize(xmlDoc));
        xmlData.appendChild(tagsXml);
    }
    xmlData.appendChild(serializeMetadata(xmlDoc));
    return xmlData;
}
void Document::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}

