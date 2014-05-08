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

#include "timeline.h"
#include "ui_timeline.h"

Timeline::Timeline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Timeline) {
    ui->setupUi(this);
    tagHorizontalCriteriaWasTimeline = false;
    Global::timeline = this;
    startTimer(40);
    ticksWidth = 50;
    timelineControl = new TimelineControl();
    Global::timelineGL->showLegendDest.setAction(ui->legend);

    ui->ffButton->setVisible(false);
    ui->playButton->setVisible(false);

    ui->workspace->addItem("HISTORIC VIEW",        "global_work");
    ui->workspace->addItem("YOUR WORK",            "your_work");
    ui->workspace->addItem("YOUR CUES",            "your_cues");
    ui->workspace->addItem("YOUR LAST IMPORTS",    "your_last_imports");
    ui->workspace->addItem("ALL THE CUES",         "global_cues");
    ui->workspace->addItem("ALL THE LAST IMPORTS", "global_last_imports");
    ui->workspace->addItem("TYPE OF DOCUMENTS PUNCHCARD", "global_punchcard_type");
    ui->workspace->addItem("PEOPLE ACTIVITY PUNCHCARD",   "global_punchcard_activity");
}

Timeline::~Timeline() {
    delete ui;
}


void Timeline::timerEvent(QTimerEvent *) {
    if(Global::timerPlay) {
        Global::time += Global::timer.elapsed() / 1000.;
        Global::timer.restart();
    }
    if(Global::timeMarkerAdded)
        ((Tag*)Global::timeMarkerAdded)->setTimeEnd(Global::time);
    Global::currentProject->fireEvents();
    Global::timeUnit          = Global::timeUnit          + (Global::timeUnitDest          - Global::timeUnit)          / Global::inertie;
    Global::timelineTagHeight = Global::timelineTagHeight + (Global::timelineTagHeightDest - Global::timelineTagHeight) / Global::inertie;


    if((tagHorizontalCriteriaWasTimeline) && (!Global::tagHorizontalCriteria->isTimeline())) {
        tagHorizontalCriteriaWasTimeline = Global::tagHorizontalCriteria->isTimeline();
        Global::timelineGL->ensureVisible(QPointF(1, 1));
    }
    else if((!tagHorizontalCriteriaWasTimeline) && (Global::tagHorizontalCriteria->isTimeline())) {
        tagHorizontalCriteriaWasTimeline = Global::tagHorizontalCriteria->isTimeline();
        Global::timelineGL->ensureVisible(QPointF(timelinePosDest.x(), -1));
    }
}

const QRectF Timeline::paintTimeline(bool before) {
    glEnable(GL_SCISSOR_TEST);

    if((0 > Global::thumbnailSlider) || (Global::thumbnailSlider > 1))
        Global::thumbnailSliderStep = -Global::thumbnailSliderStep;
    Global::thumbnailSlider = Global::thumbnailSlider + Global::thumbnailSliderStep;
    glLineWidth(1);

    if(before) {
        //First init
        if(ticks.count() == 0) {
            for(quint16 i = 0 ; i < 100 ; i++) {
                GlText text;
                text.setStyle(QSize(50., Global::timelineHeaderSize.height()), Qt::AlignCenter, Global::fontSmall);
                ticks.append(text);
            }
        }

        //Ticks
        while(ticks.count() < qFloor(Global::currentProject->totalTime() / Global::timeUnitTick)) {
            GlText text;
            text.setStyle(QSize(50., Global::timelineHeaderSize.height()), Qt::AlignCenter, Global::fontSmall);
            ticks.append(text);
        }
        if(Global::ticksChanged) {
            for(quint16 i = 0 ; i < ticks.count() ; i++) {
                QString tickText = Global::tagHorizontalCriteria->getCriteriaFormated(i * Global::timeUnitTick);
                if(ticks.at(i).text != tickText)
                    ticks[i].setText(tickText);
            }
            Global::ticksChanged = false;
        }


        Global::timelineGL->qglColor(Global::colorTicks);
        QRectF tickRectOld(-100, 10, 10, 10);
        glBegin(GL_LINES);
        for(quint16 tickIndex = 0 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
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
        GlRect::drawRect(QRectF(QPointF(Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth + Global::timelineGL->scroll.x(), Global::timelineGL->scroll.y()), QSizeF(Global::timelineGL->width(), Global::timelineHeaderSize.height())));
        Global::timelineGL->qglColor(Global::colorTagDisabled);
        QRectF tickRectOld(-100, 10, 10, 10);
        glDisable(GL_SCISSOR_TEST);
        for(quint16 tickIndex = 0 ; tickIndex < ticks.count() ; tickIndex++) {
            QRectF tickRect(QPointF(Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth + tickIndex * Global::timeUnitTick * Global::timeUnit, 0), QSizeF(ticksWidth, Global::timelineHeaderSize.height()));
            if(!tickRect.intersects(tickRectOld)) {
                ticks[tickIndex].drawText((tickRect.topLeft() + QPointF(-ticksWidth/2, Global::timelineGL->scroll.y())).toPoint());
                tickRectOld = tickRect;
            }
        }
        glEnable(GL_SCISSOR_TEST);

        //Timeline
        timelinePosDest = Global::currentProject->getTimelineCursorPos(Global::time);
        timelinePos = timelinePos + (timelinePosDest - timelinePos) / Global::inertie;
        QRectF timelineBoundingRect(QPointF(timelinePos.x() + 1, Global::timelineGL->scroll.y()), QSizeF(-50, Global::timelineGL->height()));

        if(Global::tagHorizontalCriteria->isTimeline()) {
            glBegin(GL_QUADS);
            qreal opacity = qMin(qAbs(timelinePos.x() - timelinePosDest.x()) / 200., 1.0);
            if(Global::timerPlay)
                opacity += 0.2;
            QRectF timelineBoundingRect2 = timelineBoundingRect;
            if((timelinePos.x() - timelinePosDest.x()) > 0)
                timelineBoundingRect2.setSize(QSizeF(50., timelineBoundingRect2.height()));
            Global::timelineGL->qglColor(QColor(Global::colorTimeline.red(), Global::colorTimeline.green(), Global::colorTimeline.blue(), opacity*64));    glVertex2f(timelineBoundingRect2.topLeft()    .x(), timelineBoundingRect2.topLeft()    .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
            Global::timelineGL->qglColor(QColor(Global::colorTimeline.red(), Global::colorTimeline.green(), Global::colorTimeline.blue(), opacity*0));     glVertex2f(timelineBoundingRect2.topRight()   .x(), timelineBoundingRect2.topRight()   .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
            Global::timelineGL->qglColor(QColor(Global::colorTimeline.red(), Global::colorTimeline.green(), Global::colorTimeline.blue(), opacity*0));     glVertex2f(timelineBoundingRect2.bottomRight().x(), timelineBoundingRect2.bottomRight().y());
            Global::timelineGL->qglColor(QColor(Global::colorTimeline.red(), Global::colorTimeline.green(), Global::colorTimeline.blue(), opacity*64));    glVertex2f(timelineBoundingRect2.bottomLeft() .x(), timelineBoundingRect2.bottomLeft() .y());
            glEnd();
            Global::timelineGL->qglColor(Global::colorTimeline);
            glBegin(GL_LINES);
            glVertex2f(timelineBoundingRect.topLeft()   .x(), timelineBoundingRect.topLeft()   .y() + (Global::timelineHeaderSize.height()/2+timeText.size.height()/2));
            glVertex2f(timelineBoundingRect.bottomLeft().x(), timelineBoundingRect.bottomLeft().y());
            glEnd();

            //Current
            QRectF timeTextRect(QPointF(timelineBoundingRect.topLeft().x() - timeText.size.width()/2, timelineBoundingRect.topLeft().y() + (Global::timelineHeaderSize.height()/2-timeText.size.height()/2)), timeText.size);
            Global::timelineGL->qglColor(Global::colorTimeline);
            GlRect::drawRoundedRect(timeTextRect, false);
            GlRect::drawRoundedRect(timeTextRect, true);
            Global::timelineGL->qglColor(Qt::white);
            timeText.setStyle(QSize(60, Global::timelineHeaderSize.height()*0.7), Qt::AlignCenter, Global::font);
            timeText.drawText(Sorting::timeToString(Global::time, true), timeTextRect.topLeft().toPoint());

            if((Global::timerPlay) && (Global::tagHorizontalCriteria->isTimeline()))
                Global::timelineGL->ensureVisible(QPointF(timelinePos.x(), -1));
        }
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

bool Timeline::jumpTo() {
    bool ok = false;
    qreal time = Sorting::stringToTime(QInputDialog::getText(0, tr("Jump to…"), tr("Jump to specific timecode"), QLineEdit::Normal, Sorting::timeToString(Global::time, true), &ok));
    if(ok)
        seek(time, true, true);
    return ok;
}
bool Timeline::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool dbl, bool, bool action, bool press, bool) {
    if((action) && (dbl)) {
        jumpTo();
        return true;
    }
    if((e) && (press || ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)) && (Global::tagHorizontalCriteria->isTimeline()) && (e->pos().y() <= (Global::timelineHeaderSize.height()*1.1))) {
        seek(Global::currentProject->getTimelineCursorTime(pos), false, true);
        return true;
    }

    return false;
}
bool Timeline::mouseViewer(const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool) {
    return false;
}

void Timeline::seek(qreal time, bool forceVisibleTimeline, bool forceVisibleViewer) {
    Global::seek(time);
    qreal oldInertie = Global::inertie;
    Global::inertie = 1;
    Global::currentProject->fireEvents();
    Global::inertie = oldInertie;
    if((Global::tagHorizontalCriteria->isTimeline()) && ((forceVisibleTimeline) || (Global::timerPlay)))
        Global::timelineGL->ensureVisible(QPointF(timelinePosDest.x(), -1));
    if((forceVisibleViewer) || (Global::timerPlay))
        Global::viewerGL  ->ensureVisible(QPointF(-1, viewerPosDest.y()), 0.8);
}

void Timeline::actionRewind() {
    seek(0, true, true);

}
void Timeline::actionPlay() {
    ui->playButton->setChecked(!ui->playButton->isChecked());
    if(!ui->playButton->isChecked())
        Global::timeMarkerAdded = 0;
}
void Timeline::setDuplicates(quint16 nbDuplicates) {
    timelineControl->setDuplicates(nbDuplicates);
}
void Timeline::setHistories(quint16 nbHistories) {
    timelineControl->setHistories(nbHistories);
}

void Timeline::action() {
    if(sender() == ui->playButton)       Global::play(ui->playButton->isChecked());
    else if(sender() == ui->ffButton)    seek(0);
    else if(sender() == ui->writeNote)   Global::watcher->writeNote();
    else if(sender() == ui->phaseBy) {
        Global::phases->move(ui->phaseBy->parentWidget()->mapToGlobal(ui->phaseBy->pos())                       - QPoint(12, 3 + Global::phases->height()));
        if(ui->phaseBy->isChecked())  Global::phases->show();
        else                          Global::phases->hide();
    }
    else if(sender() == ui->filterBy) {
        Global::tagFilterCriteria->move(ui->filterBy->parentWidget()->mapToGlobal(ui->filterBy->pos())          - QPoint(23, 3 + Global::tagFilterCriteria->height()));
        if(ui->filterBy->isChecked())   Global::tagFilterCriteria->show();
        else                            Global::tagFilterCriteria->hide();
    }
    else if(sender() == ui->sortBy) {
        Global::tagSortCriteria->move(ui->sortBy->parentWidget()->mapToGlobal(ui->sortBy->pos())                - QPoint(23, 3 + Global::tagSortCriteria->height()));
        if(ui->sortBy->isChecked()) Global::tagSortCriteria->show();
        else                        Global::tagSortCriteria->hide();
    }
    else if(sender() == ui->colorBy) {
        Global::tagColorCriteria->move(ui->colorBy->parentWidget()->mapToGlobal(ui->colorBy->pos())             - QPoint(23, 3 + Global::tagColorCriteria->height()));
        if(ui->colorBy->isChecked()) Global::tagColorCriteria->show();
        else                         Global::tagColorCriteria->hide();
    }
    else if(sender() == ui->textBy) {
        Global::tagTextCriteria->move(ui->textBy->parentWidget()->mapToGlobal(ui->textBy->pos())                - QPoint(23, 3 + Global::tagTextCriteria->height()));
        if(ui->textBy->isChecked())  Global::tagTextCriteria->show();
        else                         Global::tagTextCriteria->hide();
    }
    else if(sender() == ui->clusterBy) {
        Global::tagClusterCriteria->move(ui->clusterBy->parentWidget()->mapToGlobal(ui->clusterBy->pos())       - QPoint(23, 3 + Global::tagClusterCriteria->height()));
        if(ui->clusterBy->isChecked())  Global::tagClusterCriteria->show();
        else                            Global::tagClusterCriteria->hide();
    }
    else if(sender() == ui->horizontalBy) {
        Global::tagHorizontalCriteria->move(ui->horizontalBy->parentWidget()->mapToGlobal(ui->horizontalBy->pos()) - QPoint(23, 3 + Global::tagHorizontalCriteria->height()));
        if(ui->horizontalBy->isChecked())  Global::tagHorizontalCriteria->show();
        else                               Global::tagHorizontalCriteria->hide();
    }
    else if(sender() == ui->viewOption) {
        timelineControl->move(ui->viewOption->parentWidget()->mapToGlobal(ui->viewOption->pos())                - QPoint(23, 3 + timelineControl->height()));
        if(ui->viewOption->isChecked()) timelineControl->show();
        else                            timelineControl->hide();
    }
    else if((sender() == ui->workspace) && ((Global::tagFilterCriteria) && (Global::tagSortCriteria) && (Global::tagColorCriteria) && (Global::tagTextCriteria) && (Global::tagClusterCriteria) && (Global::tagHorizontalCriteria))) {
        QString workspace = ui->workspace->itemData(ui->workspace->currentIndex()).toString();
        /*
        ui->filter->addItem(1"date (year)",  "Rekall->Date/Time | 0,4");
        ui->filter->addItem(2"date (month)", "Rekall->Date/Time | 0,7");
        ui->filter->addItem(3"date (day)",   "Rekall->Date/Time | 0,10");  // 1234:67:90 23:56:89
        ui->filter->addItem(4"time (hours)", "Rekall->Date/Time | 11,2");  // 0123:56:89012:45:67
        ui->filter->addItem(5"type",         "Rekall->Type");
        ui->filter->addItem(6"author",       "Rekall->Author");
        ui->filter->addItem(7"keywords",     "Rekall->All");
        ui->filter->addItem(8"name",         "Rekall->Name");
        ui->filter->addItem(9"import date",  "Rekall->Import Date/Time | 0,16");
        ui->filter->addItem(10"import author","Rekall->Import Author");
        ui->filter->addItem(11"first letter (name)", "Rekall->Name | 0,1");
        ui->filter->addItem("Composite->Light Value");
        ui->filter->addItem("Rekall->Size");
        */

        if(workspace == "global_work") {
            Global::tagFilterCriteria    ->reset("author");
            Global::tagSortCriteria      ->reset("date (month)");
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "your_work") {
            Global::tagFilterCriteria    ->reset("author", Global::userInfos->getInfo("User Name"));
            Global::tagSortCriteria      ->reset("date (month)");
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "your_cues") {
            Global::tagFilterCriteria    ->reset("type", "cue");
            Global::tagSortCriteria      ->reset("author", Global::userInfos->getInfo("User Name"));
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "global_cues") {
            Global::tagFilterCriteria    ->reset("type", "cue");
            Global::tagSortCriteria      ->reset("author");
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "your_last_imports") {
            Global::tagFilterCriteria    ->reset("import author", Global::userInfos->getInfo("User Name"));
            Global::tagSortCriteria      ->reset("import date");
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "global_last_imports") {
            Global::tagFilterCriteria    ->reset("import author");
            Global::tagSortCriteria      ->reset("import date");
            Global::tagHorizontalCriteria->reset("time");
        }
        else if(workspace == "global_punchcard_type") {
            Global::tagFilterCriteria    ->reset("type");
            Global::tagSortCriteria      ->reset("author");
            Global::tagHorizontalCriteria->reset("type");
        }
        else if(workspace == "global_punchcard_activity") {
            Global::tagFilterCriteria    ->reset("type");
            Global::tagSortCriteria      ->reset("author");
            Global::tagHorizontalCriteria->reset("date (month)");
        }
    }
}

void Timeline::actionMarkerAddStart() {
    if(Global::timeMarkerAdded)
        actionMarkerAddEnd();
    else {
        Document *marker = new Document(Global::currentProject);
        marker->updateImport(tr("Marker"));
        Global::timeMarkerAdded = marker->createTag(TagTypeContextualTime, Global::time);
        marker->updateFeed();

        actionMarkerAddStarted.restart();
        ui->marker->setStyleSheet("background-color: rgb(255,84,79); border-color: rgb(255,84,79);");
        ui->marker->setText("Keep pressed for long marker");
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    }
}
void Timeline::actionMarkerAddEnd() {
    if(Global::timeMarkerAdded) {
        Tag *tag = (Tag*)Global::timeMarkerAdded;
        if(actionMarkerAddStarted.elapsed() < 1000)
            tag->setType(TagTypeContextualMilestone, tag->getTimeStart());
        Global::timeMarkerAdded = 0;
        ui->marker->setText("+");
        ui->marker->setStyleSheet("");
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    }
}

void Timeline::actionDisplayed(bool val) {
    if(sender() == Global::tagFilterCriteria)
        ui->filterBy->setChecked(val);
    else if(sender() == Global::tagSortCriteria)
        ui->sortBy->setChecked(val);
    else if(sender() == Global::tagColorCriteria)
        ui->colorBy->setChecked(val);
    else if(sender() == Global::tagTextCriteria)
        ui->textBy->setChecked(val);
    else if(sender() == Global::tagClusterCriteria)
        ui->clusterBy->setChecked(val);
    else if(sender() == Global::tagHorizontalCriteria)
        ui->horizontalBy->setChecked(val);
    else if(sender() == Global::phases)
        ui->phaseBy->setChecked(val);
    else if(sender() == timelineControl)
        ui->viewOption->setChecked(val);
}

void Timeline::actionChanged(QString text, QString text2) {
    QString buttonOrange = "QPushButton { background-color: rgb(255,147,102); border-color: rgb(255,147,102); } QPushButton:hover { border-color: rgb(255,255,255); } QPushButton:disabled { background-color: rgb(41, 44, 45);  border-color: rgb(41, 44, 45); }";

    if(text != "nothing") {
        if(sender() == Global::phases) {
            if(text2.isEmpty()) {
                ui->phaseBy->setText(text);
                ui->phaseBy->setStyleSheet("");
            }
            else {
                ui->phaseBy->setText(text);
                ui->phaseBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagSortCriteria) {
            if(text2.isEmpty()) {
                ui->sortBy->setText(text);
                ui->sortBy->setStyleSheet("");
            }
            else {
                ui->sortBy->setText(text2);
                ui->sortBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagColorCriteria) {
            if(text2.isEmpty()) {
                ui->colorBy->setText(text);
                ui->colorBy->setStyleSheet("");
            }
            else {
                ui->colorBy->setText(text2);
                ui->colorBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagTextCriteria) {
            if(text2.isEmpty()) {
                ui->textBy->setText(text);
                ui->textBy->setStyleSheet("");
            }
            else {
                ui->textBy->setText(text2);
                ui->textBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagFilterCriteria) {
            if(text2.isEmpty()) {
                ui->filterBy->setText("");
                ui->filterBy->setStyleSheet("");
            }
            else {
                ui->filterBy->setText(text2);
                ui->filterBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagClusterCriteria) {
            if(text2.isEmpty()) {
                ui->clusterBy->setText("");
                ui->clusterBy->setStyleSheet("");
            }
            else {
                ui->clusterBy->setText(text2);
                ui->clusterBy->setStyleSheet(buttonOrange);
            }
        }
        else if(sender() == Global::tagHorizontalCriteria) {
            if(text2.isEmpty()) {
                ui->horizontalBy->setText(text);
                ui->horizontalBy->setStyleSheet("");
            }
            else {
                ui->horizontalBy->setText(text2);
                ui->horizontalBy->setStyleSheet(buttonOrange);
            }
        }
    }
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::phases->needCalulation = true;
    Global::timelineGL->scrollTo();
    Global::viewerGL  ->scrollTo();
}

void Timeline::closePopups() {
    Global::tagSortCriteria->close();
    Global::tagColorCriteria->close();
    Global::tagTextCriteria->close();
    Global::tagFilterCriteria->close();
    Global::tagClusterCriteria->close();
    Global::tagHorizontalCriteria->close();
    Global::phases->close();
    timelineControl->close();
}
