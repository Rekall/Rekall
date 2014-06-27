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

#include "watchersnapshot.h"
#include "ui_watchersnapshot.h"

WatcherSnapshot::WatcherSnapshot(Metadata *_document, qint16 _version, QWidget *parent) :
    QWidget(parent, Qt::Widget | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    ui(new Ui::WatcherSnapshot) {
    ui->setupUi(this);
    document = _document;
    version  = _version;
    qDebug("%d", version);
    setStyleSheet(Global::mainWindow->styleSheet());
    move(QApplication::desktop()->screenGeometry().topRight() + QPoint(-width()-5, 5));
    timer = startTimer(2000);
    timerEvent(0);
}

WatcherSnapshot::~WatcherSnapshot() {
    delete ui;
}

void WatcherSnapshot::action() {
    killTimer(timer);
    if(sender() == ui->snap) {
        QString filename = "";
        if(document->file.exists()) {
            filename = QString("%1_%2.jpg").arg(Global::cacheFile("comment", document->file)).arg(version);
            document->setMetadata("Rekall", "Snapshot", "Comment", version);
        }
        Global::temporaryScreenshot.save(filename, "jpeg", 70);
        Global::metaChanged = true;
    }
    close();
    deleteLater();
}

void WatcherSnapshot::timerEvent(QTimerEvent *) {
    hide();
    QApplication::processEvents();
    Global::watcher->takeTemporarySnapshot();
    QApplication::processEvents();
    show();
    ui->preview->setPixmap(QPixmap::fromImage(Global::temporaryScreenshot).scaled(ui->preview->size(), Qt::KeepAspectRatio));
}
