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

#include "previewerlabel.h"

PreviewerLabel::PreviewerLabel(QWidget *parent) :
    QLabel(parent) {
}

void PreviewerLabel::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerLabel::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerLabel::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}


PreviewerPushButton::PreviewerPushButton(QWidget *parent) :
    QPushButton(parent) {
}

void PreviewerPushButton::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerPushButton::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerPushButton::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}

PreviewerVideoPlayer::PreviewerVideoPlayer(QWidget *parent) :
    Phonon::VideoPlayer(parent) {
}

void PreviewerVideoPlayer::mousePressEvent(QMouseEvent*) {
    emit(mousePressed());
}
void PreviewerVideoPlayer::mouseReleaseEvent(QMouseEvent*) {
    emit(mouseReleased());
}
void PreviewerVideoPlayer::mouseDoubleClickEvent(QMouseEvent*) {
    emit(mouseDoubleClick());
}
