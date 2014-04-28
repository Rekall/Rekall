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

#ifndef TIMELINEGL_H
#define TIMELINEGL_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QPinchGesture>
#include <misc/global.h>
#include "items/uifileitem.h"
#include "core/project.h"

class TimelineGL : public GlWidget {
    Q_OBJECT
public:
    explicit TimelineGL(QWidget *parent = 0);

private:
    qreal         showLegend;
    qreal         gestureZoomInitial;
    QList<GlText> categories;

protected:
    void timerEvent(QTimerEvent *);
    void initializeGL();
    void resizeGL(int, int);
    void wheelEvent(QWheelEvent *);
    void paintGL();
    bool event(QEvent *);

protected:
    bool gestureEvent(QGestureEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMove(QMouseEvent *, bool, bool, bool, bool);
    void leaveEvent(QEvent *);
public slots:
    void mouseMoveLong();

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

signals:
    
public slots:
    
};

#endif // TIMELINEGL_H
