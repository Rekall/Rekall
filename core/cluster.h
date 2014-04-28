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

#ifndef CLUSTER_H
#define CLUSTER_H

#include <QObject>
#include "tag.h"

class Cluster : public QObject {
    Q_OBJECT

public:
    explicit Cluster(QObject *parent = 0);

private:
    const Cluster* linkedCluster;
public:
    inline void setLinkedCluster(const Cluster* _linkedCluster) { linkedCluster = _linkedCluster; }

private:
    QList<Tag*> tags;
public:
    void clear();
    void add(Tag *tag);

private:
    QRectF timelineBoundingRect;
    qreal  animation, animationDest;
public:
    void paintTimeline();

};

#endif // CLUSTER_H
