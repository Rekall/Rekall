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
    QMenu        *tagMenu;

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
    void mouseMove(QMouseEvent *, bool, bool, bool press);
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
