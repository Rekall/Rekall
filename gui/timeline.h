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
    bool tagHorizontalCriteriaWasTimeline;
    QPointF timelinePos, timelinePosDest;
    QPointF viewerPos,   viewerPosDest;
    QTime   actionMarkerAddStarted;
    QList<GlText> ticks;
    qreal         ticksWidth;
    GlText        timeText;
private:
    void seek(qreal _time, bool forceVisibleTimeline = false, bool forceVisibleViewer = false);
protected:
    void timerEvent(QTimerEvent *);
    const QRectF paintTimeline(bool before = false);
    const QRectF paintViewer();
    bool mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool);
    bool mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool);
public:
    bool jumpTo();
    void closePopups();

public slots:
    void action();
    void actionMarkerAddStart();
    void actionMarkerAddEnd();
    void actionDisplayed(bool);
    void actionChanged(QString,QString);
    void actionRewind();
    void actionPlay();
    void setDuplicates(quint16 nbDuplicates);
    void setHistories (quint16 nbHistories);

    
private:
    Ui::Timeline *ui;
};

#endif // TIMELINE_H
