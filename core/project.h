#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QMenu>
#include <QImage>
#include "document.h"
#include "cluster.h"
#include "person.h"

class Project : public ProjectBase {
    Q_OBJECT

public:
    explicit Project(QWidget *parent = 0);

private:
    void open(const QDir &dir, const QDir &dirBase, bool debug = false);
public:
    void open(const QFileInfoList &file, UiTreeView *view, bool debug = false);

public:
    QList<Document*> documents;
    QList<Person*>   persons;
public:
    void addDocument(void *_document) {
        Document *document = (Document*)_document;
        documents.append(document);
    }
    void addPerson(void* _person) {
        Person *person = (Person*)_person;
        Global::mainWindow->personsTreeWidget->addTopLevelItem(person);
        persons.append(person);
    }

private:
    QList<Tag*> viewerTags, eventsTags;
    QMap<QString, QMap<QString, QMap<QString, QList<Tag*> > > > timelineSortTags;
    QMap< QPair<QString, QString>, Cluster*> timelineClusters;
public:
    Document* getDocument         (const QString &name) const;
    Document* getDocumentAndSelect(const QString &name) const;

public:
    void fireEvents();
    qreal totalTime() const;

private:
    QList< QPair<QRectF, QPair<QString, QString> > > guiCategories;
    QList<GlText> timelineCategories, timelinePhases;
    qreal categoryColorOpacity, categoryColorOpacityDest;
    QMenu *timelineFilesMenu;
    GlRect textureStrips;
public:
    const QRectF paintTimeline(bool before = false);
    const QRectF paintViewer();
    bool mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool);

public:
    const QPointF getTimelineCursorPos(qreal) const;
    const QPointF getViewerCursorPos  (qreal) const;
    qreal         getTimelineCursorTime(const QPointF &) const;
    qreal         getViewerCursorTime  (const QPointF &) const;

public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);
    void save();

signals:
    void refreshMetadata();
};

#endif // PROJECT_H
