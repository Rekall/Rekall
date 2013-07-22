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

void WatcherFeeling::display(Document *_document) {
    if(!isVisible())
        documents.clear();
    if(!documents.contains(_document)) {
        documents.append(_document);
        ui->baseText->clear();
        ui->moreText->clear();
        ui->preview->setChecked(true);
        ui->preview->setIcon(documents.first()->temporaryScreenshot);
        ui->ok->resetCounter("Ok", 5);
        move(QApplication::desktop()->screenGeometry().topRight() + QPoint(-width()-5, 5));
    }
    if(documents.count() == 1)  ui->label->setText(tr("Something to say about your changes on %1 since last time (%2)?").arg(documents.first()->file.baseName()).arg(Global::dateToString(documents.first()->getMetadata("Document Date/Time").toDateTime())));
    else                        ui->label->setText(tr("Something to say about your changes on these %1 since last time?").arg(Global::plurial(documents.count(), "document")));
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
        foreach(Document *document, documents) {
            if(document->updateFile(document->file))
                document->createTagBasedOnPrevious();
            qint16 version = document->getMetadataIndexVersion(-1);
            QString baseText = ui->baseText->text(), moreTexte = ui->moreText->toPlainText();
            if(!baseText.isEmpty())
                document->setMetadata("Rekall", "Comments", baseText, version);
            if(!moreTexte.isEmpty())
                document->setMetadata("Rekall", "Comments (details)", moreTexte, version);
            if(ui->preview->isChecked()) {
                document->temporaryScreenshot.save(QString("%1_%2.jpg").arg(Global::cacheFile("comment", document->file)).arg(version), "jpeg", 70);
                document->setMetadata("Rekall", "Snapshot", "Comment", version);
            }
        }
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::metaChanged = true;
        close();
    }
    else if(sender() == ui->cancel) {
        close();
    }
}
