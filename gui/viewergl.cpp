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

#include "viewergl.h"

ViewerGL::ViewerGL(QWidget *parent) :
    GlWidget(QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering | QGL::SampleBuffers), parent) {
    Global::viewerGL = this;
    startTimer(20);
    setMouseTracking(true);
    connect(&mouseTimer, SIGNAL(timeout()), SLOT(mouseMoveLong()));
}

void ViewerGL::timerEvent(QTimerEvent *) {
    if(glReady)
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
    glReady = true;

    //Efface
    GLbitfield clearFlag = GL_COLOR_BUFFER_BIT;
    if(format().stencil())  clearFlag |= GL_STENCIL_BUFFER_BIT;
    if(format().depth())    clearFlag |= GL_DEPTH_BUFFER_BIT;
    glClear(clearFlag);
    qglClearColor(Global::colorTextBlack);

    QRectF _drawingBoundingRect;
    scroll = scroll + (scrollDest - scroll) / Global::inertie;
    visibleRect = QRectF(scroll, size());
    glPushMatrix();
    glTranslatef(qRound(-scroll.x()), qRound(-scroll.y()), 0);
    if(Global::currentProject)  _drawingBoundingRect = _drawingBoundingRect.united(Global::currentProject->paintViewer());
    if(Global::timeline)        _drawingBoundingRect = _drawingBoundingRect.united(Global::timeline      ->paintViewer());
    glPopMatrix();
    drawingBoundingRect = _drawingBoundingRect;
}

void ViewerGL::mousePressEvent(QMouseEvent *e) {
    mouseTimerPos = e->posF();
    mouseTimerOk = true;
    mouseTimer.start(1000);
    mouseMove(e, false, false, true);
}
void ViewerGL::mouseMoveEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false, false);
}
void ViewerGL::mouseReleaseEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false, false);
    Global::selectedTagsInAction.clear();
}
void ViewerGL::mouseDoubleClickEvent(QMouseEvent *e) {
    mouseMove(e, true, false, true);
}
void ViewerGL::leaveEvent(QEvent *) {
    Global::selectedTagsInAction.clear();
    Global::selectedTagHover    = 0;
}
void ViewerGL::mouseMoveLong() {
    if(mouseTimerOk)
        mouseMove(0, false, true, true);
}
void ViewerGL::mouseMove(QMouseEvent *e, bool dbl, bool stay, bool press) {
    QPointF mousePos = scroll;
    if(e)   mousePos += e->posF();
    else    mousePos += mouseTimerPos;
    bool action = true;
    foreach(void *selectedTagInAction, Global::selectedTagsInAction) {
        Tag *selectedTag = (Tag*)selectedTagInAction;
        if(Global::selectedTagMode == TagSelectionMove)
            selectedTag->moveTimeStart(Global::currentProject->getViewerCursorTime(mousePos));
        action = false;
    }
    if(e) {
        bool ok = false;
        if((!ok) && (Global::currentProject))  ok |= Global::currentProject->mouseViewer(mousePos, e, dbl, stay, action, press, false);
        if((!ok) && (Global::timeline))        ok |= Global::timeline      ->mouseViewer(mousePos, e, dbl, stay, action, press, false);
    }
}
void ViewerGL::wheelEvent(QWheelEvent *e) {
    scrollTo(QPointF(scrollDest.x(), scrollDest.y() - e->delta() / 2.));
}
