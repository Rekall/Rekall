#include "document.h"

Document::Document(ProjectBase *_project, QObject *parent) :
    DocumentBase(parent) {
    project     = _project;
    chutierItem = 0;
}


void Document::createTagBasedOnPrevious(qint16 _forVersion) {
    qint16 forVersion = getMetadataIndexVersion(_forVersion);
    if(tags.count()) {
        qint16 lastVersionWas = -1;
        foreach(Tag *tag, tags) {
            quint16 version = tag->getDocumentVersion();
            if((lastVersionWas < version) && (version < forVersion))
                lastVersionWas = version;
        }
        QList<Tag*> tagsToDuplicate;
        foreach(Tag *tag, tags) {
            if(tag->getDocumentVersion() == lastVersionWas)
                tagsToDuplicate << tag;
        }
        foreach(Tag *tag, tagsToDuplicate) {
            Tag *newTag = new Tag(this, forVersion);
            newTag->test(tag->timeStart);
            newTag->timelinePos     = tag->timelinePos;
            newTag->timelineDestPos = tag->timelineDestPos;
            newTag->viewerPos       = tag->viewerPos;
            newTag->viewerDestPos   = tag->viewerDestPos;
            newTag->setTimeEnd(tag->timeEnd);
            tags.append(newTag);
        }
    }
    else {
        qreal tS = Global::aleaF(5, 60);
        if(function == DocumentFunctionRender) {
            tS = Global::aleaF(0, 10);
        }
        for(quint16 i = 0 ; i < getMetadataCount() ; i++) {
            if(getMetadataCount() > 1) {
                quint16 nb = Global::alea(1, 1);
                for(quint16 j = 0 ; j < nb ; j++) {
                    Tag *tag = new Tag(this, i);
                    tag->test(tS + Global::aleaF(-5, 15));
                    tags.append(tag);
                }
            }
            else {
                Tag *tag = new Tag(this, forVersion);
                tag->test(tS);
                tags.append(tag);
            }
        }
    }
}


QDomElement Document::serialize(QDomDocument &xmlDoc) {
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
