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

#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QWidget>
#include <QUrl>
#include <QFileInfo>
#include <QDesktopServices>
#include <Phonon>
#include "misc/global.h"


using namespace Phonon;

namespace Ui {
class Previewer;
}

class Previewer : public QWidget {
    Q_OBJECT
    
public:
    explicit Previewer(QWidget *parent = 0);
    ~Previewer();

protected:
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *);

private slots:
    void action();
    void actionOpen();
    void actionFinished();

private:
    UiBool  timeMode;
    bool    isUpdating;
    qreal   volumeOld;
    QPixmap picture;
    QString filename;
public:
    void setVolume(qreal volume);
    void preview(int documentType, const QString &_filename, const QPixmap &_picture);

private:
    Ui::Previewer *ui;
};

#endif // PREVIEWER_H
