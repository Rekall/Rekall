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

#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <Phonon>
#include "core/project.h"

namespace Ui {
class Player;
}

using namespace Phonon;

class Player : public QWidget, public GlVideo, public Resizable {
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    ~Player();

private:
    QList<Tag*> tags;
    UiBool      timeMode;
    bool        isUpdating;
    qreal       volumeMasterOld, volumeMaster;
public:
    void  load(void* tag);
    void  play(bool state);
    void  unload(void* _tag);
    void  seek(qreal time);
    qreal getCurrentTime() const;

public slots:
    void action();
    void actionDbl();
    void setVolume(qreal _volumeMaster);

protected:
    void timerEvent(QTimerEvent*);
    void globalPlayPause();
    void forceResizeEvent();
    void resizeEvent(QResizeEvent *);

private:
    Ui::Player *ui;
};

#endif // PLAYER_H
