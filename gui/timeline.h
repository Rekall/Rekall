#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include "gui/timelinegl.h"

namespace Ui {
class Timeline;
}

class Timeline : public QWidget, public GlDrawable {
    Q_OBJECT
    
public:
    explicit Timeline(QWidget *parent = 0);
    ~Timeline();

private:
    QPointF timelinePos, timelinePosDest;
    QPointF viewerPos,   viewerPosDest;
    QList<GlText> ticks;
    qreal ticksWidth;
    GlText timeText;
private:
    void seek(qreal _time, bool ensureVisible = true);
protected:
    void timerEvent(QTimerEvent *);
    const QRectF paintTimeline(bool before = false);
    const QRectF paintViewer();
    bool mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool);
    bool mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool);

public slots:
    void action();
    void actionRewind();
    void actionPlay();

    
private:
    Ui::Timeline *ui;
};

#endif // TIMELINE_H
