#include "previewer.h"
#include "ui_previewer.h"

Previewer::Previewer(const QString &_pathApp, QWidget *parent) :
    QWidget(parent, Qt::Tool),
    ui(new Ui::Previewer) {
    ui->setupUi(this);

    pathApp = _pathApp;
    ui->gps->load(QUrl::fromLocalFile(pathApp + "/tools/maps.html"));
    ui->tabWidget->setTabEnabled(1, false);
}

Previewer::~Previewer() {
    delete ui;
}

void Previewer::resizeEvent(QResizeEvent *) {
    QSizeF targetSize = QSizeF(QSizeF(picture.second.size()) / (qreal)qMax(picture.second.width(), picture.second.height()) * (qreal)qMax(ui->type->width(), ui->type->height()));
    qreal scaleDepassement = qMax(targetSize.width() / (qreal)ui->type->width(), targetSize.height() / (qreal)ui->type->height());
    if(scaleDepassement > 1)
        targetSize = QSizeF(targetSize / scaleDepassement);
    ui->picture->resize(QSize(targetSize.width() - 20, targetSize.height() - 20));
    ui->picture->move(ui->type->width() / 2 - ui->picture->width() / 2, ui->type->height() / 2 - ui->picture->height() / 2);
}

void Previewer::displayPixmap(const QPair<QString, QPixmap> &_picture) {
    if(!_picture.second.isNull()) {
        picture = _picture;
        ui->type->setCurrentIndex(0);
        ui->picture->setPixmap(picture.second);
        resizeEvent(0);
        show();
        ui->picture->setVisible(true);
    }
    else
        ui->picture->setVisible(false);
}
void Previewer::displayGps(const QPair<QString,QString> &gps) {
    displayGps(gps.first, gps.second);
}
void Previewer::displayGps(const QString &gps, const QString &place) {
    QStringList gpsData = gps.split(",");
    if(gpsData.count() > 1) {
        ui->gps->page()->mainFrame()->evaluateJavaScript("initializeMaps([" + QString("['%1',%2,%3],").arg(place).arg(gpsData.at(0).trimmed()).arg(gpsData.at(1).trimmed()) + "])");
        ui->tabWidget->setTabEnabled(1, true);
    }
    else
        ui->tabWidget->setTabEnabled(1, false);
}

void Previewer::action() {
    QFileInfo fileInfo(picture.first);
    if(sender() == ui->open) {
        if(fileInfo.exists())
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else if(sender() == ui->reveal) {
        if(QFileInfo(picture.first).exists()) {
#ifdef Q_OS_MAC
            QStringList scriptArgs;
            scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(fileInfo.absoluteFilePath());
            QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
            scriptArgs.clear();
            scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
            QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
        }
    }
}

