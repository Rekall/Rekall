#include "watcherfeeling.h"
#include "ui_watcherfeeling.h"

WatcherFeelingWaiter::WatcherFeelingWaiter(QWidget *parent) :
    QPushButton(parent) {
    counter     = 0;
    showCounter = false;
    startTimer(1000);
}
void WatcherFeelingWaiter::resetCounter(const QString &_baseText, quint16 _counter) {
    baseText = _baseText;
    counter  = _counter;
    showCounter = true;
    timerEvent(0);
}
void WatcherFeelingWaiter::disableCounter() {
    counter  = 0;
    showCounter = false;
    timerEvent(0);
}

void WatcherFeelingWaiter::timerEvent(QTimerEvent *e) {
    if((showCounter) && (counter)) {
        if(e)
            counter--;
        if(counter == 0)
            emit(released());
        else
            setText(QString("%1 (%2)").arg(baseText).arg(counter));
    }
    else
        setText(baseText);
}


WatcherFeeling::WatcherFeeling(QWidget *parent) :
    QWidget(parent, Qt::Widget | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    ui(new Ui::WatcherFeeling) {
    ui->setupUi(this);
}

void WatcherFeeling::display(Document *document) {
    bool resetInfo = false;
    if(!isVisible())
        documents.clear();
    if(!document) {
        resetInfo = true;
        ui->ok->resetCounter("Save");
    }
    else if(!documents.contains(document)) {
        documents.append(document);
        resetInfo = true;
        ui->ok->resetCounter("Ok", 5);
    }
    if(resetInfo) {
        ui->baseText->clear();
        ui->moreText->clear();
        ui->location->setText(Global::userLocation->currentLocation);
        if(!Global::userLocation->currentWeather.isEmpty()) {
            ui->weatherTemperature->setVisible(true);
            ui->weatherIcon->setVisible(true);
            ui->weatherSky->setVisible(true);
            ui->weatherTemperature->setText(QString("%1°C").arg(Global::userLocation->currentWeatherTemp));
            ui->weatherSky        ->setText(Global::userLocation->currentWeatherSky);
            ui->weatherIcon->setPixmap(QPixmap(":/weather/weather/" + Global::userLocation->currentWeatherIcon + ".png"));
        }
        else {
            ui->weatherTemperature->setVisible(false);
            ui->weatherIcon->setVisible(false);
            ui->weatherSky->setVisible(false);
        }
        ui->preview->setChecked(true);
        ui->preview->setIcon(QPixmap::fromImage(Global::temporaryScreenshot));
        move(QApplication::desktop()->screenGeometry().topRight() + QPoint(-width()-5, 5));
    }
    if(documents.count() == 1)       ui->label->setText(tr("Something to say about your changes on %1 since last time (%2)?").arg(documents.first()->file.baseName()).arg(Global::dateToString(documents.first()->getMetadata("Document Date/Time").toDateTime())));
    else if(documents.count()  > 1)  ui->label->setText(tr("Something to say about your changes on these %1 since last time?").arg(Global::plurial(documents.count(), "document")));
    else if(documents.count() == 0)  ui->label->setText(tr("Something to say?"));
    show();
}

WatcherFeeling::~WatcherFeeling() {
    delete ui;
}

void WatcherFeeling::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::ActivationChange:
        ui->ok->disableCounter();
        break;
    default:
        break;
    }
}

void WatcherFeeling::action() {
    if((sender() == ui->ok) || (sender() == ui->baseText)) {
        if(documents.count() == 0) {

        }
        else {
            foreach(Document *document, documents) {
                if(document->updateFile(document->file))
                    document->createTagBasedOnPrevious();
                qint16 version = document->getMetadataIndexVersion(-1);
                QString baseText = ui->baseText->text(), moreTexte = ui->moreText->toPlainText(), location = ui->location->text();
                if(!baseText.isEmpty())
                    document->setMetadata("Rekall", "Comments", baseText, version);
                if(!moreTexte.isEmpty())
                    document->setMetadata("Rekall", "Comments (details)", moreTexte, version);
                if(!location.isEmpty()) {
                    QStringList gpsElements = ui->location->text().split("@", QString::SkipEmptyParts);
                    if(gpsElements.count() > 1) {
                        document->setMetadata("Rekall", "Import Location Place", gpsElements.at(0).trimmed(), version);
                        document->setMetadata("Rekall", "Import Location GPS",   gpsElements.at(1).trimmed(), version);
                    }
                    else if(gpsElements.count() > 0)
                        document->setMetadata("Rekall", "Import Location Place", gpsElements.first().trimmed(), version);
                }
                if(!Global::userLocation->currentWeather.isEmpty()) {
                    document->setMetadata("Rekall", "Import Weather Temperature", Global::userLocation->currentWeatherTemp, version);
                    document->setMetadata("Rekall", "Import Weather Sky",         Global::userLocation->currentWeatherSky,  version);
                    document->setMetadata("Rekall", "Import Weather Sky Icon",    Global::userLocation->currentWeatherIcon, version);
                }
                if(ui->preview->isChecked()) {
                    Global::temporaryScreenshot.save(QString("%1_%2.jpg").arg(Global::cacheFile("comment", document->file)).arg(version), "jpeg", 70);
                    document->setMetadata("Rekall", "Snapshot", "Comment", version);
                }
            }
        }
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::metaChanged = true;
        close();
    }
    else if(sender() == ui->cancel) {
        close();
    }
}
