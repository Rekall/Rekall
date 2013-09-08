#include "viewergl.h"

ViewerGL::ViewerGL(QWidget *parent) :
    GlWidget(QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering | QGL::SampleBuffers), parent) {
    Global::viewerGL = this;
    startTimer(20);
    setMouseTracking(true);
    connect(&mouseTimer, SIGNAL(timeout()), SLOT(mouseMoveLong()));
}

void ViewerGL::timerEvent(QTimerEvent *) {
    updateGL();
}

void ViewerGL::initializeGL() {
    //Options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT,  GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT,            GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT,             GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT,          GL_NICEST);
}

void ViewerGL::resizeGL(int, int) {
    //Vue
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(0, width(), height(), 0, 100, 300);
    glTranslatef(0, 0, -100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width(), height());
    scrollTo();
}

void ViewerGL::paintGL() {
    //Efface
    GLbitfield clearFlag = GL_COLOR_BUFFER_BIT;
    if(format().stencil())  clearFlag |= GL_STENCIL_BUFFER_BIT;
    if(format().depth())    clearFlag |= GL_DEPTH_BUFFER_BIT;
    glClear(clearFlag);
    qglClearColor(Global::colorBackground);

    drawingBoundingRect = QRectF();
    scroll = scroll + (scrollDest - scroll) / Global::inertie;
    visibleRect = QRectF(scroll, size());
    glPushMatrix();
    glTranslatef(qRound(-scroll.x()), qRound(-scroll.y()), 0);
    if(Global::currentProject)  drawingBoundingRect = drawingBoundingRect.united(Global::currentProject->paintViewer());
    if(Global::timeline)        drawingBoundingRect = drawingBoundingRect.united(Global::timeline      ->paintViewer());
    glPopMatrix();
}

void ViewerGL::mousePressEvent(QMouseEvent *e) {
    mouseTimerPos = e->posF();
    mouseTimerOk = true;
    mouseTimer.start(1000);
    mouseMove(e, false, false);
}
void ViewerGL::mouseMoveEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false);
}
void ViewerGL::mouseReleaseEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false);
    Global::selectedTag = 0;
}
void ViewerGL::mouseDoubleClickEvent(QMouseEvent *e) {
    mouseMove(e, true, false);
}
void ViewerGL::leaveEvent(QEvent *) {
    Global::selectedTag      = 0;
    Global::selectedTagHover = 0;
}
void ViewerGL::mouseMoveLong() {
    if(mouseTimerOk)
        mouseMove(0, false, true);
}
void ViewerGL::mouseMove(QMouseEvent *e, bool dbl, bool stay) {
    QPointF mousePos = scroll;
    if(e)   mousePos += e->posF();
    else    mousePos += mouseTimerPos;
    bool action = true;
    if(Global::selectedTag) {
        Tag *selectedTag = (Tag*)Global::selectedTag;
        if(Global::selectedTagMode == TagSelectionMove)
            selectedTag->moveTo(Global::currentProject->getViewerCursorTime(mousePos));
        action = false;
    }
    if(e) {
        bool ok = false;
        if((!ok) && (Global::currentProject))  ok |= Global::currentProject->mouseViewer(mousePos, e, dbl, stay, action);
        if((!ok) && (Global::timeline))        ok |= Global::timeline      ->mouseViewer(mousePos, e, dbl, stay, action);
    }
}
void ViewerGL::wheelEvent(QWheelEvent *e) {
    scrollTo(QPointF(scrollDest.x(), scrollDest.y() - e->delta() / 2.));
}
