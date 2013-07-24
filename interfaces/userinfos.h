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
private slots:
    void updateWeatherFinished(QNetworkReply *reply);
private:
    void getGPS();
    void getWifiSSID();
    void getUserName();
    void getDetailedInfo();
};

#endif // USERINFOS_H
