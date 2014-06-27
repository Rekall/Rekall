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

#include "feedlist.h"
#include "ui_feedlist.h"

FeedList::FeedList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedList) {
    ui->setupUi(this);
    startTimer(5000);
    toolbox      = 0;
    notifCounter = 0;

    Global::feedList = this;

    ui->feeds->setItemDelegateForColumn(0, new HtmlDelegate());
    QString prefix = "<span style='font-family: Calibri, Arial; font-size: 11px; color: #F5F8FA'>", suffix = "</span>";
    agoNow     = new QTreeWidgetItem(QStringList() << prefix+"Now"+suffix);
    agoSecs    = new QTreeWidgetItem(QStringList() << prefix+"A few seconds ago"+suffix);
    agoMin     = new QTreeWidgetItem(QStringList() << prefix+"A few minutes ago"+suffix);
    agoHours   = new QTreeWidgetItem(QStringList() << prefix+"A few hours ago"+suffix);
    agoDays    = new QTreeWidgetItem(QStringList() << prefix+"A few days ago"+suffix);
    agoWeeks   = new QTreeWidgetItem(QStringList() << prefix+"A few weeks ago"+suffix);
    agoMonthes = new QTreeWidgetItem(QStringList() << prefix+"A few monthes ago"+suffix);
    agoYears   = new QTreeWidgetItem(QStringList() << prefix+"A few years ago"+suffix);
    agos << agoNow << agoSecs << agoMin << agoHours << agoDays << agoWeeks << agoMonthes << agoYears;
    ui->feeds->addTopLevelItems(agos);
    ui->feeds->expandAll();
}

FeedList::~FeedList() {
    delete ui;
}

void FeedList::setToolbox(QToolBox *_toolbox) {
    toolbox = _toolbox;
    connect(toolbox, SIGNAL(currentChanged(int)), SLOT(timerEvent()));
}

void FeedList::addFeed(FeedItemBase *feedItem) {
    return;
    agoNow->insertChild(0, feedItem);
    feeds << feedItem;
    if(feedItem->author != Global::userInfos->getInfo("User Name"))
        notifCounter++;
    //timerEvent();
}

void FeedList::timerEvent() {
    //timerEvent(0);
}

void FeedList::timerEvent(QTimerEvent *) {
    QDateTime current = QDateTime::currentDateTime();
    foreach(FeedItemBase *feed, feeds) {
        quint16 daysTo   = feed->date.daysTo(current);
        quint16 secsTo   = feed->date.secsTo(current);
        quint16 minsTo   = secsTo / 60;
        quint16 hoursTo  = secsTo / 3600;
        quint16 weeksTo  = daysTo / 7;
        quint16 monthsTo = daysTo / 30;
        QTreeWidgetItem *newParent = 0;
        if(monthsTo > 12)     newParent = agoYears;
        else if(monthsTo > 1) newParent = agoMonthes;
        else if(weeksTo > 1)  newParent = agoWeeks;
        else if(daysTo > 1)   newParent = agoDays;
        else if(hoursTo > 1)  newParent = agoHours;
        else if(minsTo  > 1)  newParent = agoMin;
        else if(secsTo  > 10) newParent = agoSecs;
        else                  newParent = agoNow;
        if((newParent) && (feed->parent() != newParent)) {
            feed->parent()->removeChild(feed);
            newParent->insertChild(0, feed);
        }
        feed->update();
    }
    foreach(QTreeWidgetItem *ago, agos)
        ago->setHidden(ago->childCount() == 0);

    if(toolbox) {
        if(toolbox->currentIndex() == 1)
            notifCounter = 0;

        if(notifCounter) {
            //toolbox->setItemText(1, tr("EVENTS (%1)").arg(notifCounter));
            toolbox->setItemIcon(1, QIcon(":/icons/res_tray_icon_color.png"));
        }
        else {
            //toolbox->setItemText(1, tr("EVENTS"));
            toolbox->setItemIcon(1, QIcon());
        }
    }
}
