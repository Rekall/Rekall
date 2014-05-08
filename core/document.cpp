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
        qreal tS = Global::alea(5, 120);
        if(getFunction() == DocumentFunctionRender)
            tS = Global::alea(0, 5);
        newTag = createTag(TagTypeContextualTime, tS, -1, versionDest);
    }
    return newTag;
}
Tag* Document::createTag(TagType type, qreal timeStart, qreal duration, qint16 version) {
    Tag *newTag = new Tag(this, version);
    newTag->init(type, timeStart, duration);
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
    deserializeMetadata(xmlElement);
    QDomNode tagsNode = xmlElement.firstChild();
    while(!tagsNode.isNull()) {
        QDomElement tagsElement = tagsNode.toElement();
        if((!tagsElement.isNull()) && (tagsElement.nodeName() == "tags")) {
            QDomNode tagNode = tagsElement.firstChild();
            while(!tagNode.isNull()) {
                QDomElement tagElement = tagNode.toElement();
                if((!tagElement.isNull()) && (tagElement.nodeName() == "tag")) {
                    qreal timeStart = tagElement.attribute("timeStart").toDouble();
                    qreal timeEnd   = tagElement.attribute("timeEnd").toDouble();
                    createTag((TagType)tagElement.attribute("type").toInt(), timeStart, timeEnd-timeStart, tagElement.attribute("documentVersion").toInt());
                }
                tagNode = tagNode.nextSibling();
            }
        }
        tagsNode = tagsNode.nextSibling();
    }
}

