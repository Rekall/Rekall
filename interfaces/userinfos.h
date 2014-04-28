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

#ifndef USERINFOS_H
#define USERINFOS_H

#include <QObject>
#include <QProcess>
#include <QPixmap>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include "misc/global.h"
#include "core/metadata.h"

class UserInfos : public Metadata, public UserInfosBase {
    Q_OBJECT

#ifdef Q_OS_MAC
public:
    void start_mac();
    const QString getGPS_mac();
private:
    void *locationManager;
#endif

public:
    explicit UserInfos(QObject *parent = 0);

private:
    QNetworkAccessManager *weatherManager;

public:
    void update();

public:
    const QString getInfo(const QString &key);
    const QMetaDictionnay getInfos();
private:
    void setInfo(const QString &key, const QString &value);


private:
    void getWeather();
    void getPlateformInfos();
private slots:
    void getWeatherFinished(QNetworkReply *reply);
};

#endif // USERINFOS_H
