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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include "tag.h"

class Document : public DocumentBase {
    Q_OBJECT

public:
    explicit Document(ProjectBase *_project);

public:
    QList<Tag*> tags;
private:
    ProjectBase *project;
public:
    Tag* createTag(qint16 versionSource = -1, qint16 versionDest = -2);
    Tag* createTag(TagType type, qreal timeStart = 0, qreal duration = 0, qint16 version = -1, bool debug = false);
    Tag* createTag(Tag *tagSource, qint16 version = -2);
    void removeTag(void *tag);
    void removeTag(Tag *tag);

public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);
};

#endif // DOCUMENT_H
