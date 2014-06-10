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

#include "timelinecontrol.h"
#include "ui_timelinecontrol.h"

TimelineControl::TimelineControl(QWidget *parent) :
    QWidget(parent, Qt::Tool | Qt::FramelessWindowHint),
    ui(new Ui::TimelineControl) {
    ui->setupUi(this);

    Global::timeUnitDest              .setAction(ui->hZoom);
    Global::timelineTagHeightDest     .setAction(ui->vZoom);
    ui->hZoom->setValue(13);
    ui->vZoom->setValue(8);
}
void TimelineControl::init() {
    Global::timelineGL->showLinkedTagsDest   .setAction(Global::tagClusterCriteria->getLinkedTags());
    Global::timelineGL->showHashedTagsDest   .setAction(Global::tagClusterCriteria->getHash());
    Global::showHistory.setAction(Global::tagClusterCriteria->getHistory());
    connect(&Global::showHistory, SIGNAL(triggered(bool)), SLOT(action()));
    connect(Global::tagClusterCriteria->getHistory(), SIGNAL(toggled(bool)), SLOT(action()));
}

void TimelineControl::setDuplicates(quint16 nbDuplicates) {
    if(     nbDuplicates == 0)  Global::tagClusterCriteria->getHash()->setText(tr("Reveal duplicate documents") + tr(" (no document concerned)"));
    else if(nbDuplicates == 1)  Global::tagClusterCriteria->getHash()->setText(tr("Reveal duplicate documents") + tr(" (1 document concerned)"));
    else                        Global::tagClusterCriteria->getHash()->setText(tr("Reveal duplicate documents") + tr(" (%1 documents concerned)").arg(nbDuplicates));
}
void TimelineControl::setHistories(quint16 nbHistories) {
    if(     nbHistories == 0)   Global::tagClusterCriteria->getHistory()->setText(tr("Show document modifications history") + tr(" (no document concerned)"));
    else if(nbHistories == 1)   Global::tagClusterCriteria->getHistory()->setText(tr("Show document modifications history") + tr(" (1 document concerned)"));
    else                        Global::tagClusterCriteria->getHistory()->setText(tr("Show document modifications history") + tr(" (%1 documents concerned)").arg(nbHistories));
}

void TimelineControl::showEvent(QShowEvent *) {
    emit(displayed(true));
}
void TimelineControl::closeEvent(QCloseEvent *) {
    emit(displayed(false));
}
void TimelineControl::hideEvent(QHideEvent *) {
    emit(displayed(false));
}
const QString TimelineControl::styleSheet2() const {
    return styleSheet() + ui->globalFrameRed->styleSheet();
}
void TimelineControl::setStyleSheet2(const QString &str) {
    ui->globalFrameRed->setStyleSheet(str);
}

void TimelineControl::action() {
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    //Global::groupes->needCalulation = true;
}

TimelineControl::~TimelineControl() {
    delete ui;
}

void TimelineControl::mouseReleaseEvent(QMouseEvent *) {
    close();
}
