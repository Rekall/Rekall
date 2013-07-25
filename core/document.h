#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include "tag.h"

class Document : public DocumentBase {
    Q_OBJECT

public:
    explicit Document(ProjectBase *_project);
    explicit Document(ProjectBase *_project, const QDir &_dirBase);

public:
    QList<Tag*> tags;

private:
    ProjectBase *project;
public:
    void createTagBasedOnPrevious(qint16 forVersion = -1);

public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

signals:
    
public slots:
    
};

#endif // DOCUMENT_H
