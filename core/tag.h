#ifndef TAG_H
#define TAG_H

#include <QObject>
#include "misc/global.h"
#include "metadata.h"
#include "items/uifileitem.h"


class DocumentBase : public Metadata {
public:
    explicit DocumentBase(QObject *parent = 0) : Metadata(parent) {
        renderActive   = true;
    }
public:
    UiFileItem *chutierItem;
    UiBool renderActive;
};

class Tag : public QObject, public Nameable {
    Q_OBJECT


public:
    explicit Tag(DocumentBase *_document, qint16 _documentVersion = -1);


public:
    QAction *timelineFilesAction;
    QList<Tag*> linkedTags;
    qint16  documentVersion;
    TagType type;
protected:
    qreal   timeStart, timeEnd;


public:
    void create(TagType _type, qreal _timeStart, qreal _duration = 10, bool debug = false);
    const QString getTitle() const;
    QList<Tag*> historyTags, hashTags;
    qreal progression;
private:
    bool timelineWasInside;
    qreal progressionDest, decounter;
    QColor colorDest;
    QList<QString> linkedRenders;
    DocumentBase *document;
public:
    QColor color;
    bool  mouseHover, breathing;
    bool  contains(qreal time) const;
    qreal progress(qreal time) const;
    qreal progressAbs(qreal time) const;
    DocumentBase* getDocument() const { return document; }
    void snapTime(qreal *time);
public:
    qreal   blinkTime;
    GlText  viewerTimeText, viewerDocumentText, timelineTimeStartText, timelineTimeEndText, timelineTimeDurationText;
    GlRect  viewerTimePastille;
public:
    void setType(TagType _type, qreal time);
    void setTimeStart(qreal _timeStart);
    void setTimeEnd(qreal _timeEnd);
    inline qreal getTimeStart() const { return timeStart; }
    inline qreal getTimeEnd() const   { return timeEnd;   }
    inline qreal getDuration() const  { return timeEnd - timeStart; }
    void moveTo(qreal _val);
    PlayerVideo *player;
    void fireEvents();
private slots:
    void renderActiveChanged();

public:
    QRectF  timelineBoundingRect;
    QPointF timelinePos, timelineDestPos;
    qreal   tagScale, tagDestScale;
public:
    void  setTimelinePos(const QPointF _timelineDestPos)    { timelineDestPos = _timelineDestPos;  }
    const QRectF getTimelineBoundingRect() const            { return timelineBoundingRect; }
    const QRectF paintTimeline(bool before = false);
    bool  mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool  timelineContains(const QPointF &pos);
    qreal timelineProgress(const QPointF &pos);

public:
    QRectF  viewerBoundingRect;
    QPointF viewerPos, viewerDestPos;
    bool isInProgress;
public:
    void  setViewerPos(const QPointF _viewerDestPos)  { viewerDestPos = _viewerDestPos;  }
    const QRectF getViewerBoundingRect() const        { return viewerBoundingRect; }
    const QRectF paintViewer(quint16 tagIndex);
    bool  mouseViewer(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool  viewerContains(const QPointF &pos);
    qreal viewerProgress(const QPointF &pos);

public:
    bool tagHistoryFilters();
    bool isAcceptableWithSortFilters(bool strongCheck);
    bool isAcceptableWithColorFilters(bool strongCheck);
    bool isAcceptableWithClusterFilters(bool strongCheck);
    bool isAcceptableWithFilterFilters(bool strongCheck);
    bool isAcceptableWithHorizontalFilters(bool strongCheck);
    const QString getAcceptableWithClusterFilters();
    static bool sortCriteriaColor(Tag *first, Tag *second);
    static bool sortViewer(Tag *first, Tag *second);
    static bool sortEvents(Tag *first, Tag *second);
    static bool sortAlpha(Tag *first, Tag *second);;
    static const QString getCriteriaSort(Tag *tag);
    static const QString getCriteriaColor(Tag *tag);
    static const QString getCriteriaCluster(Tag *tag);
    static const QString getCriteriaFilter(Tag *tag);
    static const QString getCriteriaHorizontal(Tag *tag);
    static const MetadataElement getCriteriaPhase(Tag *tag);
    static const QString getCriteriaSortFormated(Tag *tag);
    static const QString getCriteriaColorFormated(Tag *tag);
    static const QString getCriteriaClusterFormated(Tag *tag);
    static const QString getCriteriaFilterFormated(Tag *tag);
    static const QString getCriteriaHorizontalFormated(Tag *tag);
    static bool isTagLastVersion(Tag *tag);
    inline qint16 getDocumentVersion() const {
        return document->getMetadataIndexVersion(documentVersion);
    }


public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

signals:
    
public slots:
    
};

#endif // TAG_H
