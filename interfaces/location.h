#ifndef LOCATION_H
#define LOCATION_H

#include <QObject>
#include <QProcess>
#include <QPixmap>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomDocument>

class Location : public QObject {
    Q_OBJECT

#ifdef LOCATION_INSTALLED
private:
    void *locationManager;
#endif

public:
    explicit Location(QObject *parent = 0) : QObject(parent) {
        locationManager = 0;
        weatherManager  = 0;
        currentWeatherTemp = -100;
    }

private:
    QNetworkAccessManager *weatherManager;
public:
    QString currentLocation, currentWeather;
    QString currentLocationGps, currentLocationPlace;
    qint16  currentWeatherTemp;
    QString currentWeatherSky, currentWeatherIcon;
public:
    void start();


public:
    const QString& getLocation() const { return currentLocation; }
    void update() {
        currentLocation.clear();
        currentWeather .clear();
        currentLocationPlace = getWifiSSID();
        currentLocationGps   = getGPS();
        updateWeather(currentLocationGps);

        if((!currentLocationPlace.isEmpty()) && (!currentLocationGps.isEmpty()))
            currentLocation = currentLocationPlace + " @ " + currentLocationGps;
        else if(!currentLocationPlace.isEmpty())
            currentLocation = currentLocationPlace;
        else if(!currentLocationGps.isEmpty())
            currentLocation = currentLocationGps;
    }
    void updateWeather(const QString &gpsCoords) {
        QStringList gpsCoord = gpsCoords.split(",");
        if(gpsCoord.count() > 1) {
            weatherManager = new QNetworkAccessManager(this);
            connect(weatherManager, SIGNAL(finished(QNetworkReply*)), SLOT(updateWeatherFinished(QNetworkReply*)));
            weatherManager->get(QNetworkRequest(QUrl(QString("http://api.openweathermap.org/data/2.5/weather?mode=xml&units=metric&lat=%1&lon=%2").arg(gpsCoord.at(0).trimmed()).arg(gpsCoord.at(1).trimmed()), QUrl::TolerantMode)));
        }
    }

private slots:
    void updateWeatherFinished(QNetworkReply *reply) {
        if(reply->error() != QNetworkReply::NoError)
            qDebug("Network error. %s", qPrintable(reply->errorString()));
        else {
            QDomDocument doc;
            doc.setContent(reply->readAll());
            QDomElement racine = doc.documentElement();
            QDomNode noeud = racine.firstChild();
            currentWeatherTemp = -100;
            while(!noeud.isNull()) {
                QDomElement noeudElement = noeud.toElement();
                if(noeudElement.tagName() == "temperature")
                    currentWeatherTemp = noeudElement.attribute("value").toDouble();
                else if(noeudElement.tagName() == "weather") {
                    currentWeatherSky  = noeudElement.attribute("value");
                    currentWeatherIcon = noeudElement.attribute("icon");
                    currentWeatherSky  = currentWeatherSky.left(1).toUpper() + currentWeatherSky.mid(1).toLower();
                }
                noeud = noeud.nextSibling();
            }

            if(currentWeatherTemp > -100)
                currentWeather = QString("%1°C, %2 (%3)").arg(currentWeatherTemp).arg(currentWeatherSky).arg(currentWeatherIcon);
        }
    }
private:
    const QString getGPS();
    const QString getWifiSSID() {
        QString ssid = "";
#ifdef Q_OS_MAC
        QProcess process;
        process.start("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I");
        process.waitForFinished();
        QStringList infos = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
        foreach(QString info, infos) {
            info = info.trimmed();
            if(info.startsWith("SSID: "))
                ssid = info.remove("SSID: ");
        }
#endif
        return ssid;
    }
};

#endif // LOCATION_H
