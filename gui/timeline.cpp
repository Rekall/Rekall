#include "timeline.h"
#include "ui_timeline.h"

Timeline::Timeline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Timeline) {
    ui->setupUi(this);
    Global::timeline = this;
    startTimer(40);
    ticksWidth = 50;

    Global::timeUnitDest         .setAction(ui->hZoom);
    Global::timelineTagHeightDest.setAction(ui->vZoom);
    ui->hZoom->setValue(13);
    ui->vZoom->setValue(8);
}

Timeline::~Timeline() {
    delete ui;
}


void Timeline::timerEvent(QTimerEvent *) {
    if(Global::timerPlay) {
        Global::time += Global::timer.elapsed() / 1000.;
        Global::timer.restart();
    }
    Global::currentProject->fireEvents();
    Global::timeUnit          = Global::timeUnit          + (Global::timeUnitDest          - Global::timeUnit)          / Global::inertie;
    Global::timelineTagHeight = Global::timelineTagHeight + (Global::timelineTagHeightDest - Global::timelineTagHeight) / Global::inertie;
}

const QRectF Timeline::paintTimeline(bool before) {
    glEnable(GL_SCISSOR_TEST);

    if((0 > Global::thumbnailSlider) || (Global::thumbnailSlider > 1))
        Global::thumbnailSliderStep = -Global::thumbnailSliderStep;
    Global::thumbnailSlider = Global::thumbnailSlider + Global::thumbnailSliderStep;
    glLineWidth(1);

    if(before) {
        //Ticks
        while(ticks.count() < qFloor(Global::currentProject->totalTime() / Global::timeUnitTick)) {
            GlText text;
            text.setStyle(QSize(50., Global::timelineHeaderSize.height()), Qt::AlignCenter, Global::fontSmall);
            text.setText(Global::timeToString(ticks.count() * Global::timeUnitTick));
            ticks.append(text);
        }
        Global::timelineGL->qglColor(Global::colorTicks);
        QRectF tickRectOld;
        glBegin(GL_LINES);
        for(quint16 tickIndex = 0 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
            if(!tickRect.intersects(tickRectOld)) {
                glVertex2f(tickRect.x(), Global::timelineGL->scroll.y() + Global::timelineHeaderSize.height());
                glVertex2f(tickRect.x(), Global::timelineGL->scroll.y() + Global::timelineHeaderSize.height() + Global::timelineGL->height());
                tickRectOld = tickRect;
            }
        }
        glEnd();
    }
    else {
        //Timetext
        Global::timelineGL->qglColor(Global::colorBackground);
        GlRect::drawRect(QRectF(QPointF(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth + Global::timelineGL->scroll.x(), Global::timelineGL->scroll.y()), QSizeF(Global::timelineGL->width(), Global::timelineHeaderSize.height())));
        Global::timelineGL->qglColor(Global::colorTextBlack);
        QRectF tickRectOld;
        for(quint16 tickIndex = 1 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
            if(!tickRect.intersects(tickRectOld)) {
                ticks[tickIndex].drawText((tickRect.topLeft() + QPointF(-ticksWidth/2, Global::timelineGL->scroll.y())).toPoint());
                tickRectOld = tickRect;
            }
        }

        //Timeline
        timelinePosDest = Global::currentProject->getTimelineCursorPos(Global::time);
        timelinePos = timelinePos + (timelinePosDest - timelinePos) / Global::inertie;
        QRectF timelineBoundingRect(QPointF(timelinePos.x() + 1, Global::timelineGL->scroll.y()), QSizeF(-50, Global::timelineGL->height()));

        glBegin(GL_QUADS);
        qreal opacity = qMin(qAbs(timelinePos.x() - timelinePosDest.x()) / 200., 1.0);
        if(Global::timerPlay)
            opacity += 0.2;
        QRectF timelineBoundingRect2 = timelineBoundingRect;
        if((timelinePos.x() - timelinePosDest.x()) > 0)
            timelineBoundingRect2.setSize(QSizeF(50., timelineBoundingRect2.height()));
        Global::timelineGL->qglColor(QColor(50, 221, 255, opacity*64));    glVertex2f(timelineBoundingRect2.topLeft()    .x(), timelineBoundingRect2.topLeft()    .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
        Global::timelineGL->qglColor(QColor(50, 221, 255, opacity*0));     glVertex2f(timelineBoundingRect2.topRight()   .x(), timelineBoundingRect2.topRight()   .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
        Global::timelineGL->qglColor(QColor(50, 221, 255, opacity*0));     glVertex2f(timelineBoundingRect2.bottomRight().x(), timelineBoundingRect2.bottomRight().y());
        Global::timelineGL->qglColor(QColor(50, 221, 255, opacity*64));    glVertex2f(timelineBoundingRect2.bottomLeft() .x(), timelineBoundingRect2.bottomLeft() .y());
        glEnd();
        Global::timelineGL->qglColor(Global::colorProgression);
        glBegin(GL_LINES);
        glVertex2f(timelineBoundingRect.topLeft()   .x(), timelineBoundingRect.topLeft()   .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
        glVertex2f(timelineBoundingRect.bottomLeft().x(), timelineBoundingRect.bottomLeft().y());
        glEnd();

        //Current
        glDisable(GL_SCISSOR_TEST);
        QRectF timeTextRect(QPointF(timelineBoundingRect.topLeft().x() - timeText.size.width()/2, timelineBoundingRect.topLeft().y() + (Global::timelineHeaderSize.height()/2-timeText.size.height()/2)), timeText.size);
        Global::timelineGL->qglColor(Global::colorProgression);
        GlRect::drawRoundedRect(timeTextRect, false);
        GlRect::drawRoundedRect(timeTextRect, true);
        Global::timelineGL->qglColor(Qt::white);
        timeText.setStyle(QSize(50, Global::timelineTagHeight*1.2), Qt::AlignCenter, Global::font);
        timeText.drawText(Global::timeToString(Global::time), timeTextRect.topLeft().toPoint());

        //Play/pause buttons
        /*
        Global::timelineGL->qglColor(Global::colorProgression);
        QRectF playButtonRect(QPointF(Global::timelineHeaderSize.height(), 0), QSizeF(Global::timelineHeaderSize.height(), Global::timelineHeaderSize.height()));
        GlRect::drawRoundedRect(playButtonRect, false);
        GlRect::drawRoundedRect(playButtonRect, true);
        */

        if(Global::timerPlay)
            Global::timelineGL->ensureVisible(QPointF(timelinePos.x(), -1));
    }
    glDisable(GL_SCISSOR_TEST);
    return QRectF(timelinePos, QSizeF(2, Global::timelineGL->height()));
}
const QRectF Timeline::paintViewer() {
    viewerPosDest = Global::currentProject->getViewerCursorPos(Global::time);
    viewerPos = viewerPos + (viewerPosDest - viewerPos) / Global::inertie;
    QRectF viewerBoundingRect(QPointF(0, viewerPos.y()), QSizeF(Global::viewerGL->width(), -50));

    if(Global::timerPlay)
        Global::viewerGL->ensureVisible(QPointF(-1, viewerPos.y()), 0.5);

    return viewerBoundingRect;
}


bool Timeline::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool, bool, bool action, bool) {
    if((action) && ((e->button() & Qt::LeftButton) == Qt::LeftButton)) {
        seek(Global::currentProject->getTimelineCursorTime(pos));
        Global::selectedTag = 0;
        Global::selectedTagHover = 0;
        return true;
    }
    return false;
}
bool Timeline::mouseViewer(const QPointF &pos, QMouseEvent *, bool dbl, bool, bool, bool) {
    return false;
    if(dbl) {
        seek(Global::currentProject->getViewerCursorTime(pos));
        Global::selectedTag = 0;
        Global::selectedTagHover = 0;
        return true;
    }
}

void Timeline::seek(qreal time) {
    Global::seek(time);
    qreal oldInertie = Global::inertie;
    Global::inertie = 1;
    Global::currentProject->fireEvents();
    //paintTimeline(true);
    //paintViewer();
    //paintTimeline();
    Global::inertie = oldInertie;
    Global::timelineGL->ensureVisible(QPointF(timelinePosDest.x(), -1));
    Global::viewerGL  ->ensureVisible(QPointF(-1, viewerPosDest.y()), 0.8);
}


void Timeline::actionRewind() {
    seek(0);
}
void Timeline::actionPlay() {
    ui->playButton->setChecked(!ui->playButton->isChecked());
}

void Timeline::action() {
    if(sender() == ui->playButton)       Global::play(ui->playButton->isChecked());
    else if(sender() == ui->ffButton)    seek(0);
    else if(sender() == ui->writeNote)   Global::watcher->writeNote();
}
