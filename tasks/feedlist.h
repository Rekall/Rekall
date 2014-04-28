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

#ifndef FEEDLIST_H
#define FEEDLIST_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class FeedList;
}

class FeedList : public QWidget, public FeedListBase {
    Q_OBJECT
    
public:
    explicit FeedList(QWidget *parent = 0);
    ~FeedList();

private:
    QToolBox *toolbox;
    quint16   notifCounter;
    QList<QTreeWidgetItem*> agos;
    QList<FeedItemBase*>    feeds;
    QTreeWidgetItem *agoNow, *agoSecs, *agoMin, *agoHours, *agoDays, *agoWeeks, *agoMonthes, *agoYears;

public:
    void setToolbox(QToolBox*);
    void addFeed(FeedItemBase *feedItem);

protected:
    void timerEvent(QTimerEvent *);
private slots:
    void timerEvent();
    
private:
    Ui::FeedList *ui;
};

#endif // HISTORYLIST_H
