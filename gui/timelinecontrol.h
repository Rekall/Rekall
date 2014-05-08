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

#ifndef TIMELINECONTROL_H
#define TIMELINECONTROL_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class TimelineControl;
}

class TimelineControl : public QWidget {
    Q_OBJECT

public:
    explicit TimelineControl(QWidget *parent = 0);
    ~TimelineControl();

private:
    Ui::TimelineControl *ui;

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);

public slots:
    void setDuplicates(quint16 nbDuplicates);
    void setHistories(quint16 nbHistories);
    void action();
protected:
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // TIMELINECONTROL_H
