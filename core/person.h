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

#ifndef PERSON_H
#define PERSON_H

#include <QTreeWidgetItem>
#include <QObject>
#include <QImage>
#include <QLabel>
#include <QMap>
#include <QFile>
#include <QDateTime>
#include "core/metadata.h"

class Person : public Metadata, public PersonCard, public QTreeWidgetItem {
    Q_OBJECT

public:
    explicit Person(const PersonCard &card, QObject *parent = 0);

private:
    bool updateGUINeeded;
public:
    void updateGUI();

public:
    static QList<Person*> fromString(const QString &text, QObject *parent = 0);
    static QList<Person*> fromFile  (const QString &file, QObject *parent = 0);

};

#endif // PERSON_H
