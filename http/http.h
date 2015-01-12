/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

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

#ifndef HTTP_H
#define HTTP_H

#include <QDir>
#include <QSettings>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "global.h"
#include "templatecache.h"
#include "httpsessionstore.h"
#include "httplistener.h"
#include "dualfilelogger.h"
#include "httplistener.h"
#include "http/filecontroller.h"
#include "http/requestmapper.h"
#include "http/static.h"


class Http : public HttpInterface {
    Q_OBJECT

public:
    Http(QObject *parent = 0);

public:
    inline quint16 getPort() const { return listener->serverPort(); }
    HttpHost getLocalHost() const;
    QList<HttpHost> getRemoteHosts() const;
    inline HttpHost getExternalHost() const { return externalHost; }

private:
    HttpListener* listener;

private:
    HttpHost externalHost;
    QNetworkAccessManager *ipResolver;
private slots:
    void ipResolverStart();
    void ipResolverFinished(QNetworkReply *reply);

};

#endif // HTTP_H
