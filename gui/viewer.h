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

#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class Viewer;
}

class Viewer : public QWidget {
    Q_OBJECT
    
public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();
    
private:
    Ui::Viewer *ui;
};

#endif // VIEWER_H
