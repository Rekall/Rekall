#ifndef ViewGL_H
#define ViewGL_H

#include <QGLWidget>
#include <QMouseEvent>
#include <misc/global.h>
#include "core/project.h"

class ViewerGL : public GlWidget {
    Q_OBJECT
public:
    explicit ViewerGL(QWidget *parent = 0);

protected:
    void timerEvent(QTimerEvent *);
    void initializeGL();
    void resizeGL(int, int);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMove(QMouseEvent *, bool, bool);
    void wheelEvent(QWheelEvent *);
    void leaveEvent(QEvent *);
    void paintGL();
public slots:
    void mouseMoveLong();

signals:
    
public slots:
    
};

#endif // ViewGL_H
