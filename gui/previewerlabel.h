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

#ifndef PREVIEWERLABEL_H
#define PREVIEWERLABEL_H

#include <QLabel>
#include <Phonon>
#include <QPushButton>

using namespace Phonon;

class PreviewerLabel : public QLabel {
    Q_OBJECT
public:
    explicit PreviewerLabel(QWidget *parent = 0);

signals:
    void mousePressed();
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};


class PreviewerVideoPlayer : public Phonon::VideoPlayer {
    Q_OBJECT
public:
    explicit PreviewerVideoPlayer(QWidget *parent = 0);

signals:
    void mousePressed();
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};


class PreviewerPushButton : public QPushButton {
    Q_OBJECT
public:
    explicit PreviewerPushButton(QWidget *parent = 0);

signals:
    void mousePressed();
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};


#endif // PREVIEWERLABEL_H
