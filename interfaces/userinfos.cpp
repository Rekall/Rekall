#include "userinfos.h"

UserInfos::UserInfos(QObject *parent) :
    Metadata(parent, true), UserInfosBase() {
    locationManager = 0;
    weatherManager  = 0;

#ifdef Q_OS_MAC
    start_mac();
#endif
}

void UserInfos::update() {
    setInfo("Location (verbose)", "");
    setInfo("Weather (verbose)", "");

    //Infos
    getPlateformInfos();
    QString location = "", locationGps = getInfo("Location GPS"), locationPlace = getInfo("Location Place");
    if((!locationPlace.isEmpty()) && (!locationGps.isEmpty()))
        location = locationPlace + " @ " + locationGps;
    else if(!locationPlace.isEmpty())
        location = locationPlace;
    else if(!locationGps.isEmpty())
        location = locationGps;
    setInfo("Location (verbose)", location);


    //Weather
    getWeather();
}

const QString UserInfos::getInfo(const QString &key) {
    return getMetadata("Rekall User Infos", key).toString();
}
void UserInfos::setInfo(const QString &key, const QString &value) {
    setMetadata("Rekall User Infos", key, value, -1);
}
const QMetaDictionnay UserInfos::getInfos() {
    return getMetadata();
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
        bool weatherOk = false;
        while(!noeud.isNull()) {
            QDomElement noeudElement = noeud.toElement();
            if(noeudElement.tagName() == "temperature")
                setInfo("Weather Temperature", QString::number(qFloor(noeudElement.attribute("value").toDouble())));
            else if(noeudElement.tagName() == "weather") {
                weatherOk = true;
                QString weatherSky  = noeudElement.attribute("value");
                weatherSky  = weatherSky.left(1).toUpper() + weatherSky.mid(1).toLower();
                setInfo("Weather Sky", weatherSky);
                setInfo("Weather Sky Icon", noeudElement.attribute("icon"));
            }
            noeud = noeud.nextSibling();
        }

        if(weatherOk)
            setInfo("Weather (verbose)", QString("%1°C, %2").arg(getInfo("Weather Temperature")).arg(getInfo("Weather Sky")));
    }
}

void UserInfos::getPlateformInfos() {
#ifdef Q_OS_MAC
    setInfo("Location GPS", getGPS_mac());

    //Wifi name
    QProcess wifiProcess;
    wifiProcess.start("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport", QStringList() << "-I");
    wifiProcess.waitForFinished();
    QStringList wifiPnfos = QString(wifiProcess.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
    foreach(const QString &info, wifiPnfos) {
        QPair<QString, QString> infoSeperated = Global::seperateMetadata(info);
        if(infoSeperated.first == "SSID")
            setInfo("Location Place", infoSeperated.second);
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
