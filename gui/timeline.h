#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QInputDialog>
#include "gui/timelinecontrol.h"
#include "gui/timelinegl.h"

namespace Ui {
class Timeline;
}

class Timeline : public QWidget, public GlDrawable {
    Q_OBJECT
    
public:
    explicit Timeline(QWidget *parent = 0);
    ~Timeline();

public:
    TimelineControl *timelineControl;
private:
    QPointF timelinePos, timelinePosDest;
    QPointF viewerPos,   viewerPosDest;
    QList<GlText> ticks;
    qreal ticksWidth;
    GlText timeText;
    QTime actionMarkerAddStarted;
private:
    void seek(qreal _time, bool forceVisibleTimeline = false, bool forceVisibleViewer = false);
protected:
    void timerEvent(QTimerEvent *);
    const QRectF paintTimeline(bool before = false);
    const QRectF paintViewer();
    bool mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool);
public:
    bool jumpTo();

public slots:
    void action();
    void actionMarkerAddStart();
    void actionMarkerAddEnd();
    void actionDisplayed(bool);
    void actionChanged(QString,QString);
    void actionRewind();
    void actionPlay();
    void setDuplicates(quint16 nbDuplicates);
    void setHistories(quint16 nbHistories);

    
private:
    Ui::Timeline *ui;
};

#endif // TIMELINE_H
