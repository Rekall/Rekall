#include "timeline.h"
#include "ui_timeline.h"

Timeline::Timeline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Timeline) {
    ui->setupUi(this);
    Global::timeline = this;
    startTimer(40);
    ticksWidth = 50;
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

    if(before) {
        //Ticks
        while(ticks.count() < qFloor(Global::currentProject->totalTime() / Global::timeUnitTick)) {
            GlText text;
            text.setStyle(QSize(50., Global::timelineHeaderSize.height()), Qt::AlignCenter, Global::fontSmall);
            text.setText(Global::timeToString(ticks.count() * Global::timeUnitTick));
            ticks.append(text);
        }
        Global::timelineGL->qglColor(Global::colorAlternate);
        QRectF tickRectOld;
        glBegin(GL_LINES);
        for(quint16 tickIndex = 1 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
            if(!tickRect.intersects(tickRectOld)) {
                glVertex2f(tickRect.x(), Global::timelineGL->scroll.y());
                glVertex2f(tickRect.x(), Global::timelineGL->scroll.y() + Global::timelineGL->height());
                tickRectOld = tickRect;
            }
        }
        glEnd();
    }
    else {
        //Timetext
        Global::timelineGL->qglColor(Global::colorAlternateInv);
        GlRect::drawRect(QRectF(QPointF(Global::timelineHeaderSize.width() + Global::timelineGL->scroll.x(), Global::timelineGL->scroll.y()), QSizeF(Global::timelineGL->width(), Global::timelineHeaderSize.height())));
        Global::timelineGL->qglColor(Global::colorText);
        QRectF tickRectOld;
        for(quint16 tickIndex = 1 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
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
        Global::timelineGL->qglColor(QColor(45, 202, 225, 64));    glVertex2f(timelineBoundingRect.topLeft()    .x(), timelineBoundingRect.topLeft()    .y());
        Global::timelineGL->qglColor(QColor(45, 202, 225, 0));     glVertex2f(timelineBoundingRect.topRight()   .x(), timelineBoundingRect.topRight()   .y());
        Global::timelineGL->qglColor(QColor(45, 202, 225, 0));     glVertex2f(timelineBoundingRect.bottomRight().x(), timelineBoundingRect.bottomRight().y());
        Global::timelineGL->qglColor(QColor(45, 202, 225, 64));    glVertex2f(timelineBoundingRect.bottomLeft() .x(), timelineBoundingRect.bottomLeft() .y());
        glEnd();
        Global::timelineGL->qglColor(Global::colorProgression);
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2f(timelineBoundingRect.topLeft()    .x(), timelineBoundingRect.topLeft()    .y());
        glVertex2f(timelineBoundingRect.bottomLeft() .x(), timelineBoundingRect.bottomLeft() .y());
        glEnd();
        glLineWidth(1);

        //Current
        glDisable(GL_SCISSOR_TEST);
        Global::timelineGL->qglColor(Global::colorProgression);
        QRectF timeTextRect(QPointF(timelineBoundingRect.topLeft().x() - timeText.size.width()/2, timelineBoundingRect.topLeft().y()), timeText.size);
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
        Global::viewerGL->ensureVisible(QPointF(-1, viewerPos.y()), 0.8);

    return viewerBoundingRect;
}


bool Timeline::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool, bool, bool action) {
    if((action) && ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)) {
        seek(Global::currentProject->getTimelineCursorTime(pos));
        return true;
    }
    return false;
}
bool Timeline::mouseViewer(const QPointF &pos, QMouseEvent *, bool dbl, bool, bool) {
    return false;
    if(dbl) {
        seek(Global::currentProject->getViewerCursorTime(pos));
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
}
