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

#ifndef PLAYERVIDEO_H
#define PLAYERVIDEO_H

#include <QWidget>
#include <Phonon>
#include "misc/global.h"

using namespace Phonon;

namespace Ui {
class PlayerVideo;
}


class Nameable {
public:
    virtual const QString getTitle() const = 0;
};
class Resizable {
public:
    virtual void globalPlayPause()  = 0;
    virtual void forceResizeEvent() = 0;
};


class PlayerVideo : public QWidget {
    Q_OBJECT
    
public:
    explicit PlayerVideo(QWidget *parent = 0);
    ~PlayerVideo();

public:
    bool       isVideo;
    Nameable  *tag;
    Resizable *window;
    qreal      masterVolume, volume;
private:
    qreal      volumeOld;
public:
    void load(Nameable *_tag, bool _isVideo, const QString &filename);
    void play();
    void pause();
    void seek(qint64 time);
    bool isDisplayed() const;
    qint64 currentTime() const;
    qint64 totalTime() const;
    void setVolume(qreal _volume, qreal masterVolume = -1);
    void refreshControls();

private slots:
    void action();

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    Ui::PlayerVideo *ui;
};

#endif // PLAYERVIDEO_H
