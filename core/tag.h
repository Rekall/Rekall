#ifndef TAG_H
#define TAG_H

#include <QObject>
#include "misc/global.h"
#include "metadata.h"


class DocumentBase : public Metadata {
public:
    explicit DocumentBase(QObject *parent = 0) : Metadata(parent) { }
};


class Tag : public QObject, public Nameable {
    Q_OBJECT

public:
    explicit Tag(DocumentBase *_document, qint16 _documentVersion = -1);

private:
    qreal          timeStart, timeEnd;
    qreal          progression;
    qint16         documentVersion;
    QList<QString> linkedRenders;
    DocumentBase  *document;
public:
    QAction       *timelineFilesAction;
    QList<Tag*>    linkedTags;
    TagType        type;
    PlayerVideo   *player;

public:
    QList<Tag*> historyTags, hashTags;

public:
    void create(TagType _type, qreal _timeStart, qreal _duration = 10, bool debug = false);
    const QString getTitle() const;
    inline qint16 getDocumentVersion() const {  return document->getMetadataIndexVersion(documentVersion); }

private:
    bool   timelineWasInside, isInProgress;
    qreal  progressionDest, decounter;
    qreal  blinkTime;
    QColor colorDest;
    QColor realTimeColor;
    bool   breathing;
private:
public:
    inline const QColor getRealTimeColor() const { return realTimeColor; }
    inline void  shouldBreathe(bool _breathing)  { breathing = _breathing; }
    inline bool  contains(qreal time)      const { return (getTimeStart() <= time) && (time <= (getTimeStart() + qMax(1., getDuration()))); }
    inline qreal progress(qreal time)      const { return qBound(0., progressAbs(time), 1.); }
    inline qreal progressAbs(qreal time)   const { return (time - getTimeStart()) / qMax(1., getDuration()); }
    inline qreal getTimeStart()            const { return timeStart; }
    inline qreal getTimeEnd()              const { return timeEnd;   }
    inline qreal getDuration()             const { return timeEnd - timeStart; }
    inline DocumentBase* getDocument()     const { return document; }
    inline bool timelineContains (const QPointF &pos) const {    return (timelineBoundingRect.translated(timelinePos).translated(0, Global::timelineHeaderSize.height()).contains(pos)); }
    inline bool viewerContains   (const QPointF &pos) const {    return (viewerBoundingRect.translated(viewerPos).contains(pos));    }
    inline qreal timelineProgress(const QPointF &pos) const {
        QRectF rect = timelineBoundingRect.translated(timelinePos);
        return qBound(0., (pos.x() - rect.x()) / rect.width(), 1.);
    }
    inline qreal viewerProgress  (const QPointF &pos) const {
        QRectF rect = viewerBoundingRect.translated(viewerPos);
        return qBound(0., (pos.y() - rect.y()) / rect.height(), 1.);
    }
    void snapTime(qreal *time) const;
    void setType(TagType _type, qreal time);
    void setTimeStart(qreal _timeStart);
    void setTimeEnd(qreal _timeEnd);
    void moveTo(qreal _val);
    void fireEvents();

private:
    GlText  viewerTimeText, viewerDocumentText, timelineTimeStartText, timelineTimeEndText, timelineTimeDurationText;
    GlRect  viewerTimePastille;
    qreal   tagScale, tagDestScale;
    bool    timelineFirstPos, timelineFirstPosVisible;
    bool    viewerFirstPos, viewerFirstPosVisible;
    QRectF  timelineBoundingRect, viewerBoundingRect;
public:
    QPointF timelinePos, timelineDestPos;
    QPointF viewerPos,   viewerDestPos;
public:
    inline void setTimelinePos(const QPointF _timelineDestPos) {
        timelineDestPos = _timelineDestPos;
        if(timelineFirstPos) {
            timelinePos = timelineDestPos;
            timelineFirstPos = false;
            timelineFirstPosVisible = true;
        }
    }
    inline void setViewerPos(const QPointF _viewerDestPos) {
        viewerDestPos = _viewerDestPos;
        if(viewerFirstPos) {
            viewerPos = viewerDestPos;
            viewerFirstPos = false;
            viewerFirstPosVisible = true;
        }
    }
    inline const QRectF getTimelineBoundingRect() const { return timelineBoundingRect; }
    inline const QRectF getViewerBoundingRect()   const { return viewerBoundingRect;   }
    const QRectF paintTimeline(bool before = false);
    const QRectF paintViewer(quint16 tagIndex);
    bool  mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool  mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool);

public:
    bool tagHistoryFilters() const;
    bool isAcceptableWithSortFilters      (bool strongCheck) const;
    bool isAcceptableWithColorFilters     (bool strongCheck) const;
    bool isAcceptableWithClusterFilters   (bool strongCheck) const;
    bool isAcceptableWithFilterFilters    (bool strongCheck) const;
    bool isAcceptableWithHorizontalFilters(bool strongCheck) const;
    const QString getAcceptableWithClusterFilters() const;
    static bool sortCriteriaColor(const Tag *first, const Tag *second);
    static bool sortViewer(const Tag *first, const Tag *second);
    static bool sortEvents(const Tag *first, const Tag *second);
    static bool sortAlpha (const Tag *first, const Tag *second);
    static const QString getCriteriaSort              (const Tag *tag);
    static const QString getCriteriaColor             (const Tag *tag);
    static const QString getCriteriaCluster           (const Tag *tag);
    static const QString getCriteriaFilter            (const Tag *tag);
    static const QString getCriteriaHorizontal        (const Tag *tag);
    static const MetadataElement getCriteriaPhase     (const Tag *tag);
    static const QString getCriteriaSortFormated      (const Tag *tag);
    static const QString getCriteriaColorFormated     (const Tag *tag);
    static const QString getCriteriaClusterFormated   (const Tag *tag);
    static const QString getCriteriaFilterFormated    (const Tag *tag);
    static const QString getCriteriaHorizontalFormated(const Tag *tag);
    static       bool    isTagLastVersion             (const Tag *tag);


public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);
};

#endif // TAG_H
