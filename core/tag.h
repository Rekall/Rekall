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
    inline const QColor getColor() {
        if(Global::colorForMeta.contains(getCriteriaColorFormated()))
            return Global::colorForMeta.value(getCriteriaColorFormated()).first;
        else
            return Qt::lightGray;
    }
};

class Tag : public QObject {
    Q_OBJECT


public:
    explicit Tag(DocumentBase *_document, qint16 _documentVersion);


public:
    QAction *timelineFilesAction;
    QList<Tag*> linkedTags;
    qreal   timeStart, timeEnd;
    qint16  documentVersion;
    TagType type;


public:
    void test(qreal tS);
    QList<Tag*> historyTags;
private:
    bool timelineWasInside;
    qreal progression, progressionDest;
    QColor colorDest;
    QList<QString> linkedRenders;
    DocumentBase *document;
public:
    QColor color;
    bool  mouseHover, mouseHoverPreview, breathing;
    bool  contains(qreal time) const;
    qreal progress(qreal time) const;
    qreal progressAbs(qreal time) const;
    qreal getDuration() const;
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
    void moveTo(qreal _val);
    VideoPlayer *videoPlayer;
    void fireEvents();
private slots:
    void renderActiveChanged();

public:
    QRectF  timelineBoundingRect;
    QPointF timelinePos, timelineDestPos;
public:
    void  setTimelinePos(const QPointF _timelineDestPos)    { timelineDestPos = _timelineDestPos;  }
    const QRectF getTimelineBoundingRect() const            { return timelineBoundingRect; }
    const QRectF paintTimeline(bool before = false);
    bool  mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool);
    bool  timelineContains(const QPointF &pos);
    qreal timelineProgress(const QPointF &pos);

private:
    QRectF  viewerBoundingRect;
public:
    QPointF viewerPos, viewerDestPos;
public:
    void  setViewerPos(const QPointF _viewerDestPos)  { viewerDestPos = _viewerDestPos;  }
    const QRectF getViewerBoundingRect() const        { return viewerBoundingRect; }
    const QRectF paintViewer(quint16 tagIndex);
    bool  mouseViewer(const QPointF &, QMouseEvent *, bool, bool, bool);
    bool  viewerContains(const QPointF &pos);
    qreal viewerProgress(const QPointF &pos);

private:
    bool isAcceptableWithFilters();
public:
    bool isAcceptableWithSortFilters();
    bool isAcceptableWithColorFilters();
    bool isAcceptableWithClusterFilters();
    const QString getAcceptableWithClusterFilters();
    static bool sortCriteria(Tag *first, Tag *second);
    static bool sortViewer(Tag *first, Tag *second);
    static bool sortEvents(Tag *first, Tag *second);
    static const QString getCriteriaSort(Tag *tag);
    static const QString getCriteriaColor(Tag *tag);
    static const QString getCriteriaCluster(Tag *tag);
    static const MetadataElement getCriteriaSortRaw(Tag *tag);
    static const QString getCriteriaSortFormated(Tag *tag);
    static const QString getCriteriaColorFormated(Tag *tag);
    static const QString getCriteriaClusterFormated(Tag *tag);
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
