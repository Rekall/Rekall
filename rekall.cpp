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

#include "rekall.h"

Rekall::Rekall(const QStringList &arguments, QWidget *parent) :
    QDialog(parent) {
    trayIconWorking = false;
    trayIconIndex   = 0;
    trayIconIndexOld = 9999;
    Global::rekall = this;

#ifdef QT5
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    //Update
    updateManager = 0;
    forceUpdate = false;
    firstTimeOpened = false;
    if(arguments.contains("-forceupdate"))
        forceUpdate = true;

    //Tray icon
    trayTimer.setInterval(500);
    connect(&trayTimer,    SIGNAL(timeout()), SLOT(trayIconToOnPrivate()));
    connect(&trayTimerOff, SIGNAL(timeout()), SLOT(trayIconToOffPrivate()));
    for(quint16 i = 0 ; i <= 17 ; i++)
        trayIcons << QIcon(QString(":/icons/rekall-menubar-%1.png").arg(i, 2, 10, QChar('0')));
    //rekall-menubar-mini-00
    trayIcon = new QSystemTrayIcon(this);
    trayTimer.start();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    //
    Global::userInfos = new UserInfos(this);
    Global::http      = new Http(this);
    Analyse *analyse = new Analyse(this);
    Global::analyse   = analyse;
    connect(analyse, SIGNAL(trayChanged(QString,bool)), SLOT(analyseTrayChanged(QString,bool)));
    connect(analyse, SIGNAL(trayIconToOff()), SLOT(trayIconToOff()));
    connect(analyse, SIGNAL(trayIconToOn(qint16)), SLOT(trayIconToOn(qint16)));

    trayMenu = new QMenu(this);
    trayMenu->setSeparatorsCollapsible(true);
    trayAnalyse = trayMenu->addAction(tr("File analysis…"));
    trayMenu->addAction(tr("Open welcome page"), this, SLOT(openWebPage()));
    //trayMenu->addAction(tr("Create a new project"), this, SLOT(addProject()));
    trayMenu->addSeparator();
    trayMenuProjects = trayMenu->addAction(tr("Quit Rekall"), this, SLOT(closeRekall()));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    trayIconToOnPrivate();

    QSettings settings;
    quint16 projectCount = settings.beginReadArray("projects");
    /*
    if(projectCount == 0) {
        addProject(new Project("walden",     "Walden", true, QFileInfo("/Users/guillaume/Documents/Rekall/Walden"), this));
        addProject(new Project("joris-test", "Joris", false, QFileInfo("/Users/guillaume/Documents/Rekall/joris-test"), this));
    }
    */
    connect(Global::udp, SIGNAL(outgoingMessage(QString,quint16,QString,QList<QVariant>)), SLOT(incomingMessage(QString,quint16,QString,QList<QVariant>)));

    for(quint16 projectIndex = 0 ; projectIndex < projectCount ; projectIndex++) {
        settings.setArrayIndex(projectIndex);
        Project *project = new Project(settings.value("name").toString(), settings.value("friendlyName").toString(), true, QFileInfo(settings.value("path").toString()), this);
        addProject(project);
    }
    settings.endArray();

    //foreach(ProjectInterface *project, Global::projects)
    //    project->load();


    //Global settings creation if needed
    globalSettings = new QSettings();
    if((globalSettings) && ((!globalSettings->childKeys().contains("id")) || (arguments.contains("-newuser")))) {
        firstTimeOpened = true;
        qsrand(QDateTime::currentDateTime().toTime_t());
        updateAnonymousId = QString::number(qrand());
        globalSettings->setValue("id", updateAnonymousId);
        globalSettings->setValue("version", "");
        globalSettings->setValue("updatePeriod", 1);
        globalSettings->setValue("lastUpdate",   QDateTime(QDate(2000, 01, 01)));
    }

    //Update management
    if((globalSettings) && (globalSettings->childKeys().contains("id"))) {
        QDateTime updateLastDate  = globalSettings->value("lastUpdate")  .toDateTime();
        quint16   updatePeriod    = globalSettings->value("updatePeriod").toUInt();
        updateAnonymousId         = globalSettings->value("id")          .toString();
        QString applicationVersionSettings = globalSettings->value("version").toString();
        if(applicationVersionSettings != QCoreApplication::applicationVersion()) {
            globalSettings->setValue("version", QCoreApplication::applicationVersion());
            firstTimeOpened = true;
        }

        qDebug("Last update : %s (should update each %d day(s))", qPrintable(updateLastDate.toString("dd/MM/yyyy hh:mm:ss")), updatePeriod);
        if((updateLastDate.daysTo(QDateTime::currentDateTime()) >= updatePeriod) || (forceUpdate))
            checkForUpdates();
    }

    askScreenshot = askAddProject = 0;
    startTimer(50);
    if(firstTimeOpened)
        showMessage(tr("Welcome!\nRekall is now running!"));
    else
        showMessage(tr("Rekall is now running!"));

    if(!arguments.contains("-silent"))
        openWebPage();

    /*
    QWebView *webView = new QWebView();
    webView->load(QUrl("http://127.0.0.1:23411"));
    webView->show();
    */
}
Rekall::~Rekall() {
}
void Rekall::checkForUpdates() {
    updateManager = new QNetworkAccessManager(this);
    connect(updateManager, SIGNAL(finished(QNetworkReply*)), SLOT(checkForUpdatesFinished(QNetworkReply*)));
    QString url = "http://www.rekall.fr/download/updates.php?id=" + updateAnonymousId + "&package=" + (QCoreApplication::applicationName() + "__" + QCoreApplication::applicationVersion()).toLower().replace(" ", "_").replace(".", "_");
    qDebug("Checking for updates %s", qPrintable(url));
    updateManager->get(QNetworkRequest(QUrl(url, QUrl::TolerantMode)));
}
void Rekall::checkForUpdatesFinished(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError)
        qDebug("Network error. %s", qPrintable(reply->errorString()));
    else {
        if(globalSettings)
            globalSettings->setValue("lastUpdate", QDateTime::currentDateTime());
        QString info = reply->readAll().trimmed();
        if((info.length() > 0) || (forceUpdate)) {
            int rep = QMessageBox::information(0, tr("Rekall Update Center"), tr("A new version of Rekall is available. Would you like to update Rekall with the new version ?"), QMessageBox::Yes, QMessageBox::No);
            if(rep == QMessageBox::Yes)
                QDesktopServices::openUrl(QUrl("http://www.rekall.fr/", QUrl::TolerantMode));
        }
    }
}



void Rekall::addProject() {
    QFileInfo dirToOpen(QFileDialog::getExistingDirectory(0, tr("Select a folder where you store the files of your creative project"), Global::pathDocuments.absoluteFilePath()));
    if(dirToOpen.exists()) {
        addProject(new Project(dirToOpen.baseName(), dirToOpen.baseName(), true, dirToOpen, this));
        showMessage(tr("Rekall is analysing your project %1…").arg(dirToOpen.baseName()));
    }
}
void Rekall::addProject(ProjectInterface *project) {
    trayMenu->insertAction(trayMenuProjects, project->trayMenuTitle);
    trayMenu->insertMenu  (trayMenuProjects, project->trayMenuEvents);
    trayMenu->insertAction(trayMenuProjects, project->trayMenuFolder);
    trayMenu->insertAction(trayMenuProjects, project->trayMenuWeb);
    project->trayMenuSeparator = trayMenu->insertSeparator(trayMenuProjects);
    Global::projects.append(project);
    updateGUI();
    project->load();
}
void Rekall::removeProject(ProjectInterface *project) {
    trayMenu->removeAction(project->trayMenuEvents->menuAction());
    trayMenu->removeAction(project->trayMenuTitle);
    trayMenu->removeAction(project->trayMenuFolder);
    trayMenu->removeAction(project->trayMenuWeb);
    trayMenu->removeAction(project->trayMenuSeparator);
    Global::projects.removeOne(project);
    project->isRemoved = true;
    updateGUI();
}
void Rekall::updateGUI() {
    foreach(ProjectInterface *project, Global::projects)
        project->updateGUI();
}


void Rekall::trayIconToOn(qint16 duration) {
    if(duration > 0) {
        trayIconWorking = true;
        trayTimerOff.stop();
        trayTimerOff.start(duration);
    }
}
void Rekall::trayIconToOff() {
    trayTimerOff.stop();
    trayTimerOff.start(10);
}
void Rekall::analyseTrayChanged(QString text, bool enable) {
    trayAnalyse->setText(text);
    trayAnalyse->setEnabled(enable);
}


void Rekall::trayIconToOnPrivate() {
    if(trayIconWorking) {
        if(trayIconIndexOld != trayIconIndex)
            trayIcon->setIcon(trayIcons.at(trayIconIndex));
        trayIconIndexOld = trayIconIndex;
        trayIconIndex = (trayIconIndex+1) % (trayIcons.length());
    }
}
void Rekall::trayIconToOffPrivate() {
    trayIconWorking = false;
    trayIconIndex = 4;
    if(trayIconIndexOld != trayIconIndex)
        trayIcon->setIcon(trayIcons.at(trayIconIndex));
    trayIconIndexOld = trayIconIndex;
}


void Rekall::openWebPage() {
    QString param;
    if(firstTimeOpened)
        param += "/intro.html";
    QDesktopServices::openUrl(QUrl(QString("http://%1:%2%3").arg(Global::http->getLocalHost().ip).arg(Global::http->getPort()).arg(param)));
}
void Rekall::closeRekall() {
    Global::analyse->stop();
    QSettings settings;
    settings.beginWriteArray("projects");
    quint16 projectIndex = 0;
    foreach(ProjectInterface *project, Global::projects) {
        settings.setArrayIndex(projectIndex++);
        settings.setValue("name", project->name);
        settings.setValue("friendlyName", project->friendlyName);
        settings.setValue("path", project->path.absoluteFilePath());
    }
    settings.endArray();
    settings.sync();
    QApplication::exit();
}

void Rekall::incomingMessage(const QString &, quint16, const QString &destination, const QList<QVariant> &valeurs) {
    if((destination.startsWith("/rekall/message")) && (valeurs.count()))
        showMessage(valeurs.first().toString());
    else if(destination.startsWith("/rekall/webpage"))
        openWebPage();
}

void Rekall::showMessage(const QString &message) {
    trayIcon->showMessage(tr("Rekall"), message);
    qDebug("=> %s", qPrintable(message));
}


void Rekall::timerEvent(QTimerEvent *) {
    if(askScreenshot == 1) {
        askScreenshot = 2;
        takeScreenshot();
        askScreenshot = 0;
    }
    if(askAddProject == 1) {
        askAddProject = 2;
        addProject();
        askAddProject = 0;
    }
}

void Rekall::takeScreenshot() {
    showMessage(tr("Snapshot taken!"));

    QRectF screenSize = QDesktopWidget().screenGeometry(0);
    for(quint16 i = 1 ; i < QDesktopWidget().screenCount() ; i++)
        screenSize = screenSize.united(QDesktopWidget().screenGeometry(i));

#ifdef Q_OS_MAC
    CGRect screenRect;
    screenRect.origin.x    = screenSize.x();
    screenRect.origin.y    = screenSize.y();
    screenRect.size.width  = screenSize.width();
    screenRect.size.height = screenSize.height();
    CGImageRef screenshotMac = CGWindowListCreateImage(screenRect, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
#ifdef QT4
    screenshot = QPixmap::fromMacCGImageRef(screenshotMac).toImage();
#endif
    CGImageRelease(screenshotMac);
#else
    screenshot = QPixmap::grabWindow(QApplication::desktop()->winId(), screenSize.x(), screenSize.y(), screenSize.width(), screenSize.height()).toImage();
#endif
}

void Rekall::trayActivated(QSystemTrayIcon::ActivationReason) {
    updateGUI();
}
