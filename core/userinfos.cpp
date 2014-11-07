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

#include "userinfos.h"

UserInfos::UserInfos(QObject *parent)
    : QObject(parent) {

    weatherManager  = 0;

#ifdef Q_OS_MAC
    locationManager = 0;
    //start_mac();
#endif

    update();
}

void UserInfos::update() {
    //Infos
    getPlateformInfos();
    //Weather
    getWeather();

    //Prochain refresh des infos
    QTimer::singleShot(5*60*1000, this, SLOT(update()));
}

const QString UserInfos::getInfo(const QString &key) const {
    return value(key);
}
void UserInfos::setInfo(const QString &key, const QString &value) {
    //qDebug("UINFO %s = %s", qPrintable(key), qPrintable(value));
    (*this)[key] = value;
}

const QString UserInfos::getAuthor() const {
    return getInfo("User Name");
}
const QString UserInfos::getLocationGPS() const {
    return getInfo("Location GPS");
}
const QString UserInfos::getLocationName() const {
    return getInfo("Location Name");
}


void UserInfos::getWeather() {
    QStringList gpsCoord = getInfo("Location GPS").split(",");
    if(gpsCoord.count() > 1) {
        weatherManager = new QNetworkAccessManager(this);
        connect(weatherManager, SIGNAL(finished(QNetworkReply*)), SLOT(getWeatherFinished(QNetworkReply*)));
        weatherManager->get(QNetworkRequest(QUrl(QString("http://api.openweathermap.org/data/2.5/weather?mode=xml&units=metric&lat=%1&lon=%2").arg(gpsCoord.at(0).trimmed()).arg(gpsCoord.at(1).trimmed()), QUrl::TolerantMode)));
    }
}
void UserInfos::getWeatherFinished(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError)
        qDebug("Network error. %s", qPrintable(reply->errorString()));
    else {
        QDomDocument doc;
        doc.setContent(reply->readAll());
        QDomElement racine = doc.documentElement();
        QDomNode noeud = racine.firstChild();
        while(!noeud.isNull()) {
            QDomElement noeudElement = noeud.toElement();
            if(noeudElement.tagName() == "temperature")
                setInfo("Weather Temperature", QString::number(qFloor(noeudElement.attribute("value").toDouble())));
            else if(noeudElement.tagName() == "weather") {
                QString weatherSky  = noeudElement.attribute("value");
                weatherSky  = weatherSky.left(1).toUpper() + weatherSky.mid(1).toLower();
                setInfo("Weather Sky", weatherSky);
                setInfo("Weather Sky Icon", noeudElement.attribute("icon"));
            }
            noeud = noeud.nextSibling();
        }
    }
}

void UserInfos::getPlateformInfos() {
#ifdef Q_OS_MAC
    //Géolocalisation
    //setInfo("Location GPS", getGPS_mac());

    //Wifi name
    QProcess wifiProcess;
    wifiProcess.start("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport", QStringList() << "-I");
    wifiProcess.waitForFinished();
    QStringList wifiPnfos = QString(wifiProcess.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
    foreach(const QString &info, wifiPnfos) {
        QPair<QString, QString> infoSeperated = Global::seperateMetadata(info);
        if(infoSeperated.first == "SSID")
            setInfo("Location Name", infoSeperated.second);
    }

    //User name (basic)
    /*
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    foreach(const QString &key, systemEnvironment.keys()) {
        if(key.toLower().contains("user")) {
            QString name = systemEnvironment.value(key);
            name = name.left(1).toUpper() + name.mid(1);
            setInfo("User Name", name);
        }
    }
    */

    //Opened applications
    QProcess finderProcess;
    finderProcess.start("osascript", QStringList() << "-e" << QString("tell application \"System Events\" to get the name of every process whose background only is false"));
    finderProcess.waitForFinished();
    QString apps = finderProcess.readAllStandardOutput().trimmed();
    setInfo("Applications running", apps);

    //Hardware infos
    QProcess hardwareProcess;
    hardwareProcess.start("system_profiler", QStringList() << "SPSoftwareDataType" << "SPHardwareDataType" << "SPAudioDataType" << "SPDisplaysDataType");
    hardwareProcess.waitForFinished();
    QStringList hardwareInfos = QString(hardwareProcess.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
    QString lastSection = "";
    quint16 graphicalCardIndex = 0, screenIndex = 0;
    foreach(const QString &info, hardwareInfos) {
        QPair<QString, QString> infoSeperated = Global::seperateMetadata(info);

        if(infoSeperated.second.isEmpty())
            lastSection = infoSeperated.first;

        if(lastSection == "System Software Overview") {
            if     (infoSeperated.first == "System Version")    setInfo("Computer OS",   infoSeperated.second);
            else if(infoSeperated.first == "Computer Name")     setInfo("Computer Name", infoSeperated.second);
            else if(infoSeperated.first == "User Name")         setInfo("User Name",     Global::seperateMetadata(infoSeperated.second, "(").first);
        }
        else if(lastSection == "Hardware Overview") {
            if     (infoSeperated.first == "Model Name")        setInfo("Computer Type",            infoSeperated.second);
            else if(infoSeperated.first == "Model Identifier")  setInfo("Computer Model",           infoSeperated.second);
            else if(infoSeperated.first == "Processor Name")    setInfo("Computer Processor Name",  infoSeperated.second);
            else if(infoSeperated.first == "Processor Speed")   setInfo("Computer Processor Speed", infoSeperated.second);
            else if(infoSeperated.first == "Memory")            setInfo("Computer Memory",          infoSeperated.second);
            else if(infoSeperated.first == "Serial Number")     setInfo("Computer Serial Number",   infoSeperated.second);
        }
        else {
            if     (infoSeperated.first == "Chipset Model")     setInfo(QString("Computer Graphical Card #%1").arg(++graphicalCardIndex),    infoSeperated.second);
            else if(infoSeperated.first == "VRAM (Total)")      setInfo(QString("Computer Graphical Card #%1 VRAM").arg(graphicalCardIndex), infoSeperated.second);
            else if(infoSeperated.first == "Display Type")      setInfo(QString("Computer Screen #%1").arg(++screenIndex),                   infoSeperated.second);
            else if(infoSeperated.first == "Resolution")        setInfo(QString("Computer Screen #%1 Resolution").arg(screenIndex),          infoSeperated.second);
            else if(infoSeperated.first == "Main Display")      setInfo(QString("Computer Screen #%1 Is Main").arg(screenIndex),             infoSeperated.second);
            else if(infoSeperated.first == "Mirror")            setInfo(QString("Computer Screen #%1 Is Mirrored").arg(screenIndex),         infoSeperated.second);
        }
    }
#endif
}
