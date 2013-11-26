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


    Global::timelineGL->showLinkedRendersDest.setAction(ui->linkedRenders);
    Global::timelineGL->showLinkedTagsDest   .setAction(ui->linkedTags);
    Global::showHelp.setAction(ui->help);
    Global::timelineGL->showHistory.setAction(ui->history);
    Global::timelineGL->showHash.setAction(ui->hash);
    connect(&Global::timelineGL->showHistory, SIGNAL(triggered(bool)), SLOT(action()));
    connect(&Global::timelineGL->showHash,    SIGNAL(triggered(bool)), SLOT(action()));
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


void TimelineControl::action() {
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::metaChanged = Global::phases->needCalulation = true;
}

TimelineControl::~TimelineControl() {
    delete ui;
}
