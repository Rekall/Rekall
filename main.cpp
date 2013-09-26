#include <QtGui/QApplication>
#include <QTextCodec>
#include <QLocale>
#include <QTranslator>
#include <QProcessEnvironment>
#include "rekall.h"
#include "misc/global.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


#ifdef QT4
    QTextCodec::setCodecForTr      (QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale  (QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    QString locale = QLocale::system().name();
    //QTranslator translator;
    //translator.load("Translation_" + locale, "Tools");
    //rekallApp.installTranslator(&translator);

    QString appName    = "Rekall ";
    QString appVersion = "0.1";

#ifdef Q_OS_MAC
    appName += "Mac";
    //qDebug("Command line syntax : ./Rekall.app/Contents/MacOS/Rekall <file path>");
#endif
#ifdef Q_OS_WIN
    appName += "Windows";
    //qDebug("Command line syntax : Rekall.exe <file path>");
#endif
#ifdef Q_OS_LINUX
    appName += "Linux";
    //qDebug("Command line syntax : ./Rekall <file path>");
#endif

    QCoreApplication::setApplicationName   (appName);
    QCoreApplication::setApplicationVersion(appVersion);
    QCoreApplication::setOrganizationName  ("Rekall");
    QCoreApplication::setOrganizationDomain("org.rekall");


    QDir pathApplicationDir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
#ifdef Q_OS_MAC
    pathApplicationDir.cdUp();
    pathApplicationDir.cdUp();
    pathApplicationDir.cdUp();
#endif
#ifdef QT4
    Global::pathDocuments   = QFileInfo(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + "/Rekall");
#else
    Global::pathDocuments   = QFileInfo(QStandardPaths::DocumentsLocation + "/Rekall");
#endif
    Global::pathApplication = QFileInfo(pathApplicationDir.absolutePath());
    Global::pathCurrent     = QFileInfo(QDir::currentPath() + "/Project");
    if(Global::pathApplication.absoluteFilePath().endsWith("/Rekall-build"))
        Global::pathApplication = QFileInfo(Global::pathApplication.absoluteFilePath().remove("-build"));
    if(Global::pathApplication.absoluteFilePath().endsWith("/Rekall-debug"))
        Global::pathApplication = QFileInfo(Global::pathApplication.absoluteFilePath().remove("-debug"));

    qDebug("Pathes");
    qDebug("\tDocuments  : %s", qPrintable(Global::pathDocuments  .absoluteFilePath()));
    qDebug("\tApplication: %s", qPrintable(Global::pathApplication.absoluteFilePath()));
    qDebug("\tCurrent    : %s", qPrintable(Global::pathCurrent    .absoluteFilePath()));
    qDebug("Arguments");
    for(quint16 i = 0 ; i < argc ; i++) {
        qDebug("\t%2d=\t%s", i, argv[i]);
    }

    QFileInfo project;
    quint16 indexArgument = 1;
#ifdef Q_OS_WIN
 //TOTO
    indexArgument = 0;
#endif
    if(argc > indexArgument)
        project = QFileInfo(Global::pathCurrent.absoluteFilePath() + "/" + argv[indexArgument]);

    if(QFontDatabase::addApplicationFont(Global::pathApplication.absoluteFilePath() + "/Tools/Rekall.ttf"))
        qDebug("Loading Rekall font failed");
    qsrand(QDateTime::currentDateTime().toTime_t());

    Rekall w;
    return a.exec();
}
