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
