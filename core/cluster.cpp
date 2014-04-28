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

#include "cluster.h"

Cluster::Cluster(QObject *parent) :
    QObject(parent) {
    linkedCluster = 0;
    animation = 0;
    animationDest = 1;
}

void Cluster::add(Tag *tag) {
    tags.append(tag);
    tag->shouldBreathe(true);
}
void Cluster::clear() {
    for(quint16 i = 0 ; i < tags.count() ; i++)
        tags[i]->shouldBreathe(false);
    tags.clear();
}
void Cluster::paintTimeline() {
    if(tags.count()) {
        animation = animation + (animationDest - animation) / Global::inertie;

        timelineBoundingRect = QRectF(QPointF(-1, -1), QSizeF(0, 0));
        foreach(Tag *tag, tags) {
            QRectF tagRect = tag->getTimelineBoundingRect().translated(tag->timelinePos);
            if((timelineBoundingRect.x() < 0) && (timelineBoundingRect.y() < 0))
                timelineBoundingRect = tagRect;
            else
                timelineBoundingRect = timelineBoundingRect.united(tagRect);
        }
        QColor color = Global::colorCluster;
        color.setAlphaF(color.alphaF() - Global::breathing/5.);
        Global::timelineGL->qglColor(color);
        GlRect::drawRect(timelineBoundingRect);

        if((linkedCluster) && (linkedCluster->tags.count())) {
            QPointF linkedChordBeg, linkedChordEnd;
            GLfloat ctrlpoints[4][4][3] = { {{0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}},
                                            {{0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}},
                                            {{0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}},
                                            {{0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}} };
            for(quint16 index = 0 ; index < 4 ; index++) {
                if(     index == 0)  { linkedChordBeg = timelineBoundingRect.topLeft();  linkedChordEnd = linkedCluster->timelineBoundingRect.bottomLeft(); }
                else if(index == 1)  { linkedChordBeg = timelineBoundingRect.topLeft();  linkedChordEnd = linkedCluster->timelineBoundingRect.bottomLeft(); }
                else if(index == 2)  { linkedChordBeg = timelineBoundingRect.topRight(); linkedChordEnd = linkedCluster->timelineBoundingRect.bottomRight(); }
                else if(index == 3)  { linkedChordBeg = timelineBoundingRect.topRight(); linkedChordEnd = linkedCluster->timelineBoundingRect.bottomRight(); }
                ctrlpoints[index][0][0] = linkedChordBeg.x(); ctrlpoints[index][0][1] = linkedChordBeg.y();
                ctrlpoints[index][1][0] = linkedChordBeg.x(); ctrlpoints[index][1][1] = linkedChordBeg.y() + (linkedChordEnd.y() - linkedChordBeg.y()) * 0.66;
                ctrlpoints[index][2][0] = linkedChordEnd.x(); ctrlpoints[index][2][1] = linkedChordEnd.y() + (linkedChordBeg.y() - linkedChordEnd.y()) * 0.66;
                ctrlpoints[index][3][0] = linkedChordEnd.x(); ctrlpoints[index][3][1] = linkedChordEnd.y();
            }
            quint16 resolution = 40;
            glEnable(GL_MAP2_VERTEX_3);
            glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
            glMapGrid2f(resolution, 1.-animation, 1.0, resolution, 0.0, 1.0);
            glEvalMesh2(GL_FILL, 0, resolution, 0, resolution);
            glDisable(GL_MAP2_VERTEX_3);
        }
    }
}
