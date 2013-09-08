#include "timelinegl.h"

TimelineGL::TimelineGL(QWidget *parent) :
    GlWidget(QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering | QGL::SampleBuffers), parent) {
    Global::timelineGL = this;
    startTimer(20);
    setMouseTracking(true);
    setAcceptDrops(true);
    showLegend = 0;
    showLegendDest = true;
    connect(&mouseTimer, SIGNAL(timeout()), SLOT(mouseMoveLong()));

    tagMenu = new QMenu(Global::mainWindow);
}

void TimelineGL::timerEvent(QTimerEvent *) {
    updateGL();
}

void TimelineGL::initializeGL() {
    //Options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
}

void TimelineGL::resizeGL(int, int) {
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

void TimelineGL::paintGL() {
    //Efface
    GLbitfield clearFlag = GL_COLOR_BUFFER_BIT;
    if(format().stencil())  clearFlag |= GL_STENCIL_BUFFER_BIT;
    if(format().depth())    clearFlag |= GL_DEPTH_BUFFER_BIT;
    glClear(clearFlag);
    qglClearColor(Global::colorBackground);

    drawingBoundingRect = QRectF();
    scroll = scroll + (scrollDest - scroll) / Global::inertie;
    visibleRect = QRectF(scroll, size());
    glScissor(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth, 0, width() - Global::timelineHeaderSize.width() - Global::timelineNegativeHeaderWidth, height());
    glPushMatrix();
    glTranslatef(qRound(-scroll.x()), qRound(-scroll.y()), 0);
    if(Global::timeline)        drawingBoundingRect = drawingBoundingRect.united(Global::timeline      ->paintTimeline(true));
    if(Global::currentProject)  drawingBoundingRect = drawingBoundingRect.united(Global::currentProject->paintTimeline(true));
    if(Global::timeline)        Global::timeline      ->paintTimeline();
    if(Global::currentProject)  Global::currentProject->paintTimeline();
    glPopMatrix();

    showLegend = showLegend + ((qreal)showLegendDest - showLegend) / Global::inertie;
    showLinkedRenders = showLinkedRenders + ((qreal)showLinkedRendersDest - showLinkedRenders) / Global::inertie;
    showLinkedTags    = showLinkedTags    + ((qreal)showLinkedTagsDest    - showLinkedTags)    / Global::inertie;
    Global::breathing = Global::breathing + (Global::breathingDest - Global::breathing) / 50.;
    if((     Global::breathing > 0.90) && (Global::breathingDest == 1))    Global::breathingDest = 0;
    else if((Global::breathing < 0.10) && (Global::breathingDest == 0))    Global::breathingDest = 1;
    Global::breathingFast = Global::breathingFast + (Global::breathingFastDest - Global::breathingFast) / 10.;
    if((     Global::breathingFast > 0.90) && (Global::breathingFastDest == 1))    Global::breathingFastDest = 0;
    else if((Global::breathingFast < 0.10) && (Global::breathingFastDest == 0))    Global::breathingFastDest = 1;
    Global::breathingPics = Global::breathingPics + (Global::breathingPicsDest - Global::breathingPics) / 200.;
    if((     Global::breathingPics > 0.90) && (Global::breathingPicsDest == 1))    Global::breathingPicsDest = 0;
    else if((Global::breathingPics < 0.10) && (Global::breathingPicsDest == 0))    Global::breathingPicsDest = 1;

    if(showLegend > 0.01) {
        //Background
        qreal legendBaseSize = qBound(200., height() * 0.75, 400.);
        QRectF legendRect = QRectF(width() - legendBaseSize * 1.2, height() - showLegend * legendBaseSize, legendBaseSize * 1.2, legendBaseSize);
        qglColor(Global::colorAlternateStrong);
        GlRect::drawRect(legendRect);

        //Circles
        legendBaseSize *= 0.8;
        glPushMatrix();
        glTranslatef(qRound(legendRect.center().x()), qRound(legendRect.center().y()), 0);
        qreal angle = -M_PI/2, angleStep = 0;
        glBegin(GL_QUAD_STRIP);
        QMapIterator<QString, QPair<QColor, qreal> > colorForMetaIterator(Global::colorForMeta);
        while(colorForMetaIterator.hasNext()) {
            colorForMetaIterator.next();
            angleStep = 2 * M_PI * colorForMetaIterator.value().second * showLegend;

            qreal legendRadiusL = legendBaseSize;
            qreal legendRadiusS = legendBaseSize * 0.8;

            qglColor(colorForMetaIterator.value().first);
            for(qreal angleSmooth = angle ; angleSmooth <= angle + angleStep ; angleSmooth += 0.01) {
                glVertex2f(qCos(angleSmooth) * legendRadiusL/2, qSin(angleSmooth) * legendRadiusL/2);
                glVertex2f(qCos(angleSmooth) * legendRadiusS/2, qSin(angleSmooth) * legendRadiusS/2);
            }
            angle += angleStep;
        }
        glEnd();

        //Text
        legendBaseSize *= 1.1;
        angle = -M_PI/2;
        QMapIterator<QString, QPair<QColor, qreal> > colorForMetaIteratorText(Global::colorForMeta);
        while(colorForMetaIteratorText.hasNext()) {
            colorForMetaIteratorText.next();
            angleStep = 2*M_PI * colorForMetaIteratorText.value().second * showLegend;
            angle += angleStep/2;

            QString tagCategory = colorForMetaIteratorText.key().toUpper();
            QPoint pt(qCos(angle) * legendBaseSize/2, qSin(angle) * legendBaseSize/2);

            QColor color = Qt::white;
            color.setAlpha(colorForMetaIteratorText.value().first.alpha());

            //Draw text
            qglColor(color);
            bool textFound = false;
            if(categories.count() > 1000)
                categories.clear();
            foreach(GlText category, categories) {
                if(category.text == tagCategory) {
                    category.drawText(pt - QPoint(category.size.width() / 2, 0));
                    textFound = true;
                }
            }
            if(!textFound) {
                GlText category;
                category.setStyle(QFontMetrics(Global::font).boundingRect(tagCategory).size(), Qt::AlignLeft, Global::font);
                category.drawText(tagCategory, pt - QPoint(category.size.width() / 2, 0));
                categories << category;
            }
            angle += angleStep/2;
        }
        glPopMatrix();
    }
}

void TimelineGL::mousePressEvent(QMouseEvent *e) {
    mouseTimerPos = e->posF();
    mouseTimerOk = true;
    mouseTimer.start(1000);
    mouseMove(e, false, false);
}
void TimelineGL::mouseMoveEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false);
}
void TimelineGL::mouseReleaseEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false);
    Global::selectedTag = 0;
}
void TimelineGL::mouseDoubleClickEvent(QMouseEvent *e) {
    mouseMove(e, true, false);
}
void TimelineGL::leaveEvent(QEvent *) {
    Global::selectedTag      = 0;
    Global::selectedTagHover = 0;
}
void TimelineGL::mouseMoveLong() {
    if(mouseTimerOk)
        mouseMove(0, false, true);
}
void TimelineGL::mouseMove(QMouseEvent *e, bool dbl, bool stay) {
    QPointF mousePos = scroll;
    if(e)   mousePos += e->posF();
    else    mousePos += mouseTimerPos;
    bool action = true;

    if(Global::selectedTag) {
        Tag *selectedTag = (Tag*)Global::selectedTag;

        if((e->button() & Qt::RightButton) == Qt::RightButton) {
            /*
            tagMenu->clear();
            QAction *toGlobal = 0, *toContextual = 0;
            if(selectedTag->type == TagTypeGlobal)  toGlobal     = tagMenu->addAction("Convert to global document");
            else                                    toContextual = tagMenu->addAction("Convert to contextual document");
            QAction *retour = tagMenu->exec(QCursor::pos());
            if(retour) {
                if(retour == toGlobal) {

                }
                else if(retour == toContextual) {

                }
            }
            */
        }
        else {
            if(Global::selectedTagMode == TagSelectionStart) {
                selectedTag->setTimeStart(Global::currentProject->getTimelineCursorTime(mousePos));
                setCursor(Qt::SizeHorCursor);
            }
            else if(Global::selectedTagMode == TagSelectionEnd) {
                selectedTag->setTimeEnd(Global::currentProject->getTimelineCursorTime(mousePos));
                setCursor(Qt::SizeHorCursor);
            }
            else {
                if(stay) {
                    if(selectedTag->type == TagTypeContextualMilestone) selectedTag->setType(TagTypeContextualTime,      Global::currentProject->getTimelineCursorTime(mousePos));
                    else if(selectedTag->type == TagTypeContextualTime) selectedTag->setType(TagTypeContextualMilestone, Global::currentProject->getTimelineCursorTime(mousePos));
                    Global::selectedTagStartDrag = (selectedTag->timeEnd - selectedTag->timeStart) / 2;
                }
                if((e) && (mouseTimerPos != e->pos())) {
                    selectedTag->moveTo(Global::currentProject->getTimelineCursorTime(mousePos) - Global::selectedTagStartDrag);
                    setCursor(Qt::ClosedHandCursor);
                }
            }
        }
        action = false;
    }
    if(e) {
        bool ok = false;
        if((!ok) && (Global::currentProject))  ok |= Global::currentProject->mouseTimeline(mousePos, e, dbl, stay, action);
        if((!ok) && (Global::timeline))        ok |= Global::timeline      ->mouseTimeline(mousePos, e, dbl, stay, action);
        setCursor(Qt::ArrowCursor);
    }
}
void TimelineGL::wheelEvent(QWheelEvent *e) {
    if(e->pos().x() < Global::timelineHeaderSize.width())  scrollTo(QPointF(scrollDest.x(), scrollDest.y() - e->delta() / 2.));
    else                                                   scrollTo(QPointF(scrollDest.x() - e->delta() / 2., scrollDest.y()));
}


void TimelineGL::dragEnterEvent(QDragEnterEvent *event) {
    if(Global::mainWindow->parseMimeData(event->mimeData(), "timeline", true))
        return event->acceptProposedAction();
}
void TimelineGL::dropEvent(QDropEvent *event) {
    if(Global::mainWindow->parseMimeData(event->mimeData(), "timeline"))
        return event->acceptProposedAction();
}

