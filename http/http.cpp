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

#include "http.h"

Http::Http(QObject *parent)
    : HttpInterface(parent) {
    Global::configFileName = QFileInfo(Global::pathApplication.absoluteFilePath() + "/http/webapp.ini").absoluteFilePath();
    if(Global::pathApplication.absoluteFilePath().contains("buzzinglight/Projets/Rekall"))
        Global::configFileName = QFileInfo(Global::pathApplication.absoluteFilePath() + "/http/webappDev.ini").absoluteFilePath();
    qDebug("Config file: %s", qPrintable(Global::configFileName));

    externalHost.type = "wan";
    externalHost.name = "Externe";


    /*
    // Configure logging
    QSettings* logSettings = new QSettings(Global::configFileName, QSettings::IniFormat, this);
    logSettings->beginGroup("logging");
    Logger* logger = new FileLogger(logSettings,10000);
    logger->installMsgHandler();
    if (logSettings->value("bufferSize",0).toInt()>0 && logSettings->value("minLevel",0).toInt()>0)
        qDebug("You see these debug messages because the logging buffer is enabled");
    */

    // Configure template loader and cache
    QSettings* templateSettings = new QSettings(Global::configFileName, QSettings::IniFormat);
    templateSettings->beginGroup("templates");
    Static::templateLoader = new TemplateCache(templateSettings, this);

    // Configure session store
    QSettings* sessionSettings = new QSettings(Global::configFileName, QSettings::IniFormat);
    sessionSettings->beginGroup("sessions");
    Static::sessionStore = new HttpSessionStore(sessionSettings, this);

    // Configure static file controller
    QSettings* fileSettings = new QSettings(Global::configFileName, QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    Static::fileController = new FileController(fileSettings, QFileInfo(Global::pathApplication.absoluteFilePath() + "/http/www").absoluteFilePath().toUtf8(), 0, this);

    // Configure and start the TCP listener
    qDebug("ServiceHelper: Starting webserver");
    QSettings* listenerSettings = new QSettings(Global::configFileName,  QSettings::IniFormat);
    listenerSettings->beginGroup("listener");
    RequestMapper *requestMapper = new RequestMapper(this);
    listener = new HttpListener(listenerSettings, requestMapper);
    qDebug("ServiceHelper: Service has started");

    //Résout l'IP locale
    ipResolver = new QNetworkAccessManager(this);
    connect(ipResolver, SIGNAL(finished(QNetworkReply*)), SLOT(ipResolverFinished(QNetworkReply*)));
    ipResolverStart();

    //Démarre les services
    Global::udp = new Udp(this, getPort());
    connect(Global::udp, SIGNAL(outgoingMessage(QString,quint16,QString,QList<QVariant>)), requestMapper, SLOT(incomingMessage(QString,quint16,QString,QList<QVariant>)));
}


HttpHost Http::getLocalHost() const {
    HttpHost host;
    foreach(const QNetworkInterface &networkInterface, QNetworkInterface::allInterfaces()) {
        if((networkInterface.flags() & QNetworkInterface::IsLoopBack) == QNetworkInterface::IsLoopBack) {
            foreach(const QNetworkAddressEntry &address, networkInterface.addressEntries()) {
                if(address.ip().toIPv4Address() > 0) {
                    host.name = tr("This computer only");
                    host.type = "lo";
                    host.isOk = true;
                    host.isReachable = true;
                    if(address.broadcast().toIPv4Address() > 0) host.broadcast = address.broadcast().toString();
                    if(address.ip().toIPv4Address() > 0)        host.ip        = address.ip().toString();
                }
            }
        }
    }
    return host;
}
QList<HttpHost> Http::getRemoteHosts() const {
    QHash<QString, QString> interfacesName;
    /*
#ifdef Q_OS_MAC
    //Nom réseau
    QProcess interfacesNameProcess;
    interfacesNameProcess.start("networksetup", QStringList() << "-listallhardwareports");
    interfacesNameProcess.waitForFinished();
    QStringList interfacesNameInfos = QString(interfacesNameProcess.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
    QPair<QString, QString> lastInfoSeperated;
    foreach(const QString &info, interfacesNameInfos) {
        QPair<QString, QString> infoSeperated = Global::seperateMetadata(info);

        if(infoSeperated.first == "Device")
            interfacesName.insert(infoSeperated.second, lastInfoSeperated.second);

        lastInfoSeperated = infoSeperated;
    }
#endif
*/


    QList<HttpHost> hosts;
    foreach(const QNetworkInterface &networkInterface, QNetworkInterface::allInterfaces()) {
        if((networkInterface.flags() & QNetworkInterface::CanBroadcast) == QNetworkInterface::CanBroadcast) {
            foreach(const QNetworkAddressEntry &address, networkInterface.addressEntries()) {
                if(address.ip().toIPv4Address() > 0) {
                    HttpHost host;
                    host.name = networkInterface.humanReadableName();
                    if(interfacesName.contains(host.name))
                        host.name = interfacesName.value(host.name);
                    host.type = "lan";
                    host.isOk = true;
                    host.isReachable = true;
                    if(address.broadcast().toIPv4Address() > 0) host.broadcast = address.broadcast().toString();
                    if(address.ip().toIPv4Address() > 0)        host.ip        = address.ip().toString();
                    hosts << host;
                }
            }
        }
    }
    hosts << externalHost;
    return hosts;
}

void Http::ipResolverStart() {
    ipResolver->get(QNetworkRequest(QUrl(QString("http://www.rekall.fr/returns_ip?port=%1").arg(getPort()))));
}
void Http::ipResolverFinished(QNetworkReply *reply) {
    QStringList datas = QString(reply->readAll()).split("\n");
    externalHost.isOk        = false;
    externalHost.isReachable = false;
    externalHost.name        = "";

    //IP
    if((datas.length() > 0) && (!datas.at(0).trimmed().isEmpty())) {
        QHostAddress ip(datas.at(0));
        if(ip.toIPv4Address() > 0) {
            externalHost.ip   = ip.toString();
            externalHost.isOk = true;
        }
    }

    //Test retour
    if((datas.length() > 1) && (!datas.at(1).trimmed().isEmpty())) {
        qint32 curlErrorCode = datas.at(1).toInt();
        externalHost.isReachable = (curlErrorCode==0);
    }

    //Host
    if((datas.length() > 2) && (!datas.at(2).trimmed().isEmpty())) {
    }

    //City
    if((datas.length() > 3) && (!datas.at(3).trimmed().isEmpty())) {
        externalHost.name += datas.at(3);
        //Global::userInfos->setLocation(datas.at(3));
    }

    //Country
    if((datas.length() > 4) && (!datas.at(4).trimmed().isEmpty())) {
        if(!externalHost.name.isEmpty())    externalHost.name += ", ";
        //Global::userInfos->setLocation(datas.at(3));
        externalHost.name += datas.at(4);
    }

    //GPS
    if((datas.length() > 5) && (!datas.at(5).trimmed().isEmpty())) {
    }

    //FAI
    if((datas.length() > 6) && (!datas.at(6).trimmed().isEmpty())) {
        if(!externalHost.name.isEmpty())    externalHost.name += " — ";
        externalHost.name += datas.at(6);
    }

    //Prochain refresh d'IP
    QTimer::singleShot(5*60*1000, this, SLOT(ipResolverStart()));
}
