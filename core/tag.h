#ifndef TAG_H
#define TAG_H

#include <QObject>
#include <QMenu>
#include "misc/global.h"
#include "metadata.h"


class DocumentBase : public Metadata {
public:
    explicit DocumentBase(QObject *parent = 0) : Metadata(parent) { }
    virtual void removeTag(void *tag) = 0;
};


class Tag : public QObject, public Nameable {
    Q_OBJECT

public:
    explicit Tag(DocumentBase *_document, qint16 _documentVersion = -1);

private:
    qreal   timeStart, timeEnd, timeMediaOffset;
    TagType type;
    qint16  version;
public:
    QString        displayText;
    QList<QString> linkedRenders;
    QList<Tag*>    linkedTags;
public:
    inline qreal   getTimeStart()       const { return timeStart; }
    inline qreal   getTimeEnd()         const { return timeEnd;   }
    inline qreal   getTimeMediaOffset() const { return timeMediaOffset; }
    inline TagType getType()            const { return type;      }
    void setTimeStart      (qreal _timeStart);
    void setTimeEnd        (qreal _timeEnd);
    void setTimeMediaOffset(qreal _timeMediaOffset);
    void setType           (TagType _type, qreal time = -1);
    void moveTimeStart(qreal _timeStart);
    void moveTimeEnd  (qreal _timeEnd);
    void addTimeMediaOffset(qreal offset);


private:
    qreal          progression;
    DocumentBase  *document;
public:
    QAction       *timelineFilesAction;
    PlayerVideo   *player;

public:
    QList<Tag*> historyTags, hashTags;

public:
    void init(TagType _type, qreal _timeStart, qreal _duration = 0, bool debug = false);
    void init();
    const QString getTitle() const { return document->getName(version); }
    inline qint16 getDocumentVersion() const    {  return document->getMetadataIndexVersion(version); }
    inline qint16 getDocumentVersionRaw() const {  return version; }
    inline void   setDocumentVersion(qint16 versionShift = 0) {
        if(version < 0)
            version = getDocumentVersion() + versionShift;
    }

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
    inline qreal getDuration(bool drawable = false) const {
        if((drawable) && (getType() == TagTypeContextualMilestone))
            return 0;
        return timeEnd - timeStart;
    }
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
    bool snapTime(qreal *time) const;
    void fireEvents();

private:
    GlText  viewerTimeText, viewerDocumentText, timelineTimeStartText, timelineTimeEndText, timelineTimeDurationText, timelineDocumentText;
    GlRect  viewerTimePastille;
    bool    timelineFirstPos, timelineFirstPosVisible;
    bool    viewerFirstPos, viewerFirstPosVisible;
    QRectF  timelineBoundingRect, viewerBoundingRect;
    qreal   tagDestScale;
public:
    qreal   tagScale;
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
    bool  mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool);
    bool  mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool);

public:
    bool tagHistoryFilters() const;
    bool isAcceptableWithSortFilters      (bool strongCheck) const;
    bool isAcceptableWithColorFilters     (bool strongCheck) const;
    bool isAcceptableWithTextFilters      (bool strongCheck) const;
    bool isAcceptableWithClusterFilters   (bool strongCheck) const;
    bool isAcceptableWithFilterFilters    (bool strongCheck) const;
    bool isAcceptableWithHorizontalFilters(bool strongCheck) const;
    const QString getAcceptableWithClusterFilters() const;
    static bool sortColor (const Tag *first, const Tag *second);
    static bool sortViewer(const Tag *first, const Tag *second);
    static bool sortEvents(const Tag *first, const Tag *second);
    static bool sortAlpha (const Tag *first, const Tag *second);
    static const QString getCriteriaSort              (const Tag *tag);
    static const QString getCriteriaColor             (const Tag *tag);
    static const QString getCriteriaText              (const Tag *tag);
    static const QString getCriteriaCluster           (const Tag *tag);
    static const QString getCriteriaFilter            (const Tag *tag);
    static const QString getCriteriaHorizontal        (const Tag *tag);
    static const MetadataElement getCriteriaPhase     (const Tag *tag);
    static const QString getCriteriaSortFormated      (const Tag *tag);
    static const QString getCriteriaColorFormated     (const Tag *tag);
    static const QString getCriteriaTextFormated      (const Tag *tag);
    static const QString getCriteriaClusterFormated   (const Tag *tag);
    static const QString getCriteriaFilterFormated    (const Tag *tag);
    static const QString getCriteriaHorizontalFormated(const Tag *tag);
    static       bool    isTagLastVersion             (const Tag *tag);


public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);
};

#endif // TAG_H
