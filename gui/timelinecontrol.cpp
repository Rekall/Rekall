#include "timelinecontrol.h"
#include "ui_timelinecontrol.h"

TimelineControl::TimelineControl(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::TimelineControl) {
    ui->setupUi(this);

    Global::timeUnitDest              .setAction(ui->hZoom);
    Global::timelineTagHeightDest     .setAction(ui->vZoom);
    ui->hZoom->setValue(13);
    ui->vZoom->setValue(8);


    //Global::timelineGL->showLinkedRendersDest.setAction(ui->linkedRenders);
    //Global::timelineGL->showLinkedTagsDest   .setAction(ui->linkedTags);
    Global::timelineGL->showHashedTagsDest   .setAction(ui->hash);
    Global::showHistory.setAction(ui->history);
    connect(&Global::showHistory, SIGNAL(triggered(bool)), SLOT(action()));
}

void TimelineControl::showEvent(QShowEvent *) {
    emit(displayed(true));
}
void TimelineControl::closeEvent(QCloseEvent *) {
    emit(displayed(false));
}
void TimelineControl::hideEvent(QHideEvent *) {
    emit(displayed(false));
}
const QString TimelineControl::styleSheet2() const {
    return styleSheet() + ui->globalFrameRed->styleSheet();
}
void TimelineControl::setStyleSheet2(const QString &str) {
    ui->globalFrameRed->setStyleSheet(str);
}

void TimelineControl::setDuplicates(quint16 nbDuplicates) {
    if(     nbDuplicates == 0)  ui->hash->setText(tr("Reveal duplicate documents") + tr(" (no document concerned)"));
    else if(nbDuplicates == 1)  ui->hash->setText(tr("Reveal duplicate documents") + tr(" (1 document concerned)"));
    else                        ui->hash->setText(tr("Reveal duplicate documents") + tr(" (%1 documents concerned)").arg(nbDuplicates));
}
void TimelineControl::setHistories(quint16 nbHistories) {
    if(     nbHistories == 0)   ui->history->setText(tr("Show document modifications history") + tr(" (no document concerned)"));
    else if(nbHistories == 1)   ui->history->setText(tr("Show document modifications history") + tr(" (1 document concerned)"));
    else                        ui->history->setText(tr("Show document modifications history") + tr(" (%1 documents concerned)").arg(nbHistories));
}


void TimelineControl::action() {
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::phases->needCalulation = true;
}

TimelineControl::~TimelineControl() {
    delete ui;
}
