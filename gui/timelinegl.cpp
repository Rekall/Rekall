#include "timelinegl.h"

TimelineGL::TimelineGL(QWidget *parent) :
    GlWidget(QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering | QGL::SampleBuffers), parent) {
    Global::timelineGL = this;
    startTimer(20);
    setMouseTracking(true);
    setAcceptDrops(true);
    grabGesture(Qt::PinchGesture);
    gestureZoomInitial = Global::timeUnitDest;
    showLegend = 0;
    showLegendDest = true;
    connect(&mouseTimer, SIGNAL(timeout()), SLOT(mouseMoveLong()));
}

bool TimelineGL::event(QEvent *event) {
    if(event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

void TimelineGL::timerEvent(QTimerEvent *) {
    if(glReady)
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
    glReady = true;

    if(!Global::viewerGL->glReady) {
        Global::viewerSortChanged = true;
        Global::viewerGL->glReady = true;
    }

    //Efface
    GLbitfield clearFlag = GL_COLOR_BUFFER_BIT;
    if(format().stencil())  clearFlag |= GL_STENCIL_BUFFER_BIT;
    if(format().depth())    clearFlag |= GL_DEPTH_BUFFER_BIT;
    glClear(clearFlag);
    qglClearColor(Global::colorTextBlack);

    QRectF _drawingBoundingRect;
    scroll            = scroll            + (       scrollDest            - scroll)            / (Global::inertie);
    showLegend        = showLegend        + ((qreal)showLegendDest        - showLegend)        / (Global::inertie);
    showLinkedRenders = showLinkedRenders + ((qreal)showLinkedRendersDest - showLinkedRenders) / (Global::inertie);
    showLinkedTags    = showLinkedTags    + ((qreal)showLinkedTagsDest    - showLinkedTags)    / (Global::inertie);
    showHashedTags    = showHashedTags    + ((qreal)showHashedTagsDest    - showHashedTags)    / (Global::inertie);
    tagSnap           = tagSnap           + ((qreal)tagSnapDest           - tagSnap)           / (Global::inertie);
    tagSnapSlow       = tagSnapSlow       + ((qreal)tagSnapSlowDest       - tagSnapSlow)       / (Global::inertie * 2);

    visibleRect = QRectF(scroll, size());
    glScissor(Global::timelineHeaderSize.width(), 0, width() - Global::timelineHeaderSize.width(), height());
    glPushMatrix();
    glTranslatef(qRound(-scroll.x()), qRound(-scroll.y()), 0);
    if(Global::timeline)        _drawingBoundingRect = _drawingBoundingRect.united(Global::timeline      ->paintTimeline(true));
    if(Global::currentProject)  _drawingBoundingRect = _drawingBoundingRect.united(Global::currentProject->paintTimeline(true));
    if(Global::timeline)        _drawingBoundingRect = _drawingBoundingRect.united(Global::timeline      ->paintTimeline());
    if(Global::currentProject)  _drawingBoundingRect = _drawingBoundingRect.united(Global::currentProject->paintTimeline());
    glPopMatrix();
    drawingBoundingRect = _drawingBoundingRect;


    Global::breathing = Global::breathing + (Global::breathingDest - Global::breathing) / 50.;
    if((     Global::breathing > 0.90) && (Global::breathingDest == 1))    Global::breathingDest = 0;
    else if((Global::breathing < 0.10) && (Global::breathingDest == 0))    Global::breathingDest = 1;
    Global::breathingFast = Global::breathingFast + (Global::breathingFastDest - Global::breathingFast) / 20.;
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

            QString tagCategory = QString("%1 (%2%)").arg(colorForMetaIteratorText.key().toUpper()).arg(qFloor(colorForMetaIteratorText.value().second * 100));
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

bool TimelineGL::gestureEvent(QGestureEvent *event) {
    if(QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        QPinchGesture *pinchGesture = (static_cast<QPinchGesture *>(pinch));
        if(pinchGesture->state() == Qt::GestureStarted)
            gestureZoomInitial = Global::timeUnitDest;
        Global::timeUnitDest = gestureZoomInitial * pinchGesture->scaleFactor();
    }
    return true;
}

void TimelineGL::mousePressEvent(QMouseEvent *e) {
    mouseTimerPos = e->posF();
    mouseTimerOk = true;
    mouseTimer.start(1000);
    mouseMove(e, false, false, true, false);
}
void TimelineGL::mouseMoveEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false, false, false);
}
void TimelineGL::mouseReleaseEvent(QMouseEvent *e) {
    mouseTimerOk = false;
    mouseTimer.stop();
    mouseMove(e, false, false, false, true);
    Global::selectedTagInAction = 0;
}
void TimelineGL::mouseDoubleClickEvent(QMouseEvent *e) {
    mouseMove(e, true, false, true, false);
}
void TimelineGL::leaveEvent(QEvent *) {
    Global::selectedTagInAction = 0;
    Global::selectedTagHover    = 0;
}
void TimelineGL::mouseMoveLong() {
    if(mouseTimerOk)
        mouseMove(0, false, true, true, false);
}
void TimelineGL::mouseMove(QMouseEvent *e, bool dbl, bool stay, bool press, bool release) {
    QPointF mousePos = scroll;
    if(e)   mousePos += e->posF();
    else    mousePos += mouseTimerPos;
    bool action = true;
    QCursor cursor = Qt::ArrowCursor;

    if(Global::selectedTagHover)
        cursor = Qt::PointingHandCursor;
    if(Global::selectedTagInAction) {
        Tag *tag = (Tag*)Global::selectedTagInAction;

        if(Global::selectedTagMode == TagSelectionStart) {
            tag->setTimeStart(Global::currentProject->getTimelineCursorTime(mousePos));
            cursor = Qt::SizeHorCursor;
        }
        else if(Global::selectedTagMode == TagSelectionEnd) {
            tag->setTimeEnd(Global::currentProject->getTimelineCursorTime(mousePos));
            cursor = Qt::SizeHorCursor;
        }
        else if(Global::selectedTagMode == TagSelectionMediaOffset) {
            tag->addTimeMediaOffset(Global::selectedTagStartDrag - Global::currentProject->getTimelineCursorTime(mousePos));
            Global::selectedTagStartDrag = Global::currentProject->getTimelineCursorTime(mousePos);
            cursor = Qt::ClosedHandCursor;
        }
        else if(Global::selectedTagMode == TagSelectionDuplicate) {
            Global::selectedTagHover = Global::selectedTagInAction = Global::selectedTag = ((Document*)tag->getDocument())->createTag(tag, tag->getDocumentVersionRaw());
            Global::selectedTagMode  = TagSelectionMove;
            Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
            //selectedTag->addTimeMediaOffset(Global::selectedTagStartDrag - Global::currentProject->getTimelineCursorTime(mousePos));
            //Global::selectedTagStartDrag = Global::currentProject->getTimelineCursorTime(mousePos);
            //cursor = Qt::ClosedHandCursor;
        }
        else if(Global::selectedTagMode == TagSelectionMove) {
            if(stay) {
                if(     tag->getType() == TagTypeContextualMilestone) tag->setType(TagTypeContextualTime,      Global::currentProject->getTimelineCursorTime(mousePos));
                else if(tag->getType() == TagTypeContextualTime)      tag->setType(TagTypeContextualMilestone, Global::currentProject->getTimelineCursorTime(mousePos));

                Global::selectedTagStartDrag = (tag->getTimeEnd() - tag->getTimeStart()) / 2;
            }
            if((e) && (mouseTimerPos != e->pos())) {
                qreal mousePosSnappedOrNot = Global::currentProject->getTimelineCursorTime(mousePos);
                if((release) && ((Global::selectedTagHoverSnapped.first >= 0) || (Global::selectedTagHoverSnapped.second >= 0))) {
                    if((Global::selectedTagHoverSnapped.first >= 0) && (Global::selectedTagHoverSnapped.second >= 0)) {
                        tag->setTimeStart(Global::selectedTagHoverSnapped.first);
                        tag->setTimeEnd  (Global::selectedTagHoverSnapped.second);
                    }
                    else if(Global::selectedTagHoverSnapped.first >= 0)
                        tag->moveTimeEnd(Global::selectedTagHoverSnapped.first);
                    else if(Global::selectedTagHoverSnapped.second >= 0)
                        tag->moveTimeStart(Global::selectedTagHoverSnapped.second);
                }
                else
                    tag->moveTimeStart(mousePosSnappedOrNot - Global::selectedTagStartDrag);
                cursor = Qt::ClosedHandCursor;
            }
        }

        action = false;
    }
    if(e) {
        bool ok = false;
        if((!ok) && (Global::currentProject))  ok |= Global::currentProject->mouseTimeline(mousePos, e, dbl, stay, action, press);
        if((!ok) && (Global::timeline))        ok |= Global::timeline      ->mouseTimeline(mousePos, e, dbl, stay, action, press);
    }
    setCursor(cursor);
}
void TimelineGL::wheelEvent(QWheelEvent *e) {
    if(e->orientation() == Qt::Horizontal)  scrollTo(QPointF(scrollDest.x() - e->delta() / 2., scrollDest.y()));
    else                                    scrollTo(QPointF(scrollDest.x(), scrollDest.y() - e->delta() / 2.));
}


void TimelineGL::dragEnterEvent(QDragEnterEvent *event) {
    if(Global::mainWindow->parseMimeData(event->mimeData(), "timeline", true))
        return event->acceptProposedAction();
}
void TimelineGL::dropEvent(QDropEvent *event) {
    if(Global::mainWindow->parseMimeData(event->mimeData(), "timeline"))
        return event->acceptProposedAction();
}

