#include "inspector.h"
#include "ui_inspector.h"

Inspector::Inspector(QWidget *parent) :
    QWidget(parent, Qt::Tool),
    ui(new Ui::Inspector) {
    ui->setupUi(this);
    toolbarButton = 0;

    Global::tagSortCriteria    = new Sorting(false, tr("Categories sorted by"), 2, 0);
    Global::tagColorCriteria   = new Sorting(false, tr("Colors represent"),     5, 0);
    Global::tagClusterCriteria = new Sorting(false, tr("Hightlight on"),   0, 0);
    Global::tagFilterCriteria  = new Sorting(true,  tr("Filter by"), 0, 0);
    Global::tagFilterCriteria->displayLinked.setAction(ui->history);
    connect(&Global::tagFilterCriteria->displayLinked, SIGNAL(triggered(bool)), Global::tagFilterCriteria, SLOT(action()));

    Global::phases = new Phases(0);
    ui->filters->addWidget(Global::tagSortCriteria);
    ui->filters->addWidget(Global::tagColorCriteria);
    ui->filters->addWidget(Global::tagClusterCriteria);
    ui->filters->addWidget(Global::phases);
    //ui->filters->addWidget(Global::tagFilterCriteria);

    connect(Global::phases,             SIGNAL(actionned()), SLOT(action()));
    connect(Global::tagFilterCriteria,  SIGNAL(actionned()), SLOT(action()));
    connect(Global::tagSortCriteria,    SIGNAL(actionned()), SLOT(action()));
    connect(Global::tagColorCriteria,   SIGNAL(actionned()), SLOT(action()));
    connect(Global::tagClusterCriteria, SIGNAL(actionned()), SLOT(action()));

    Global::timelineGL->showLegendDest.setAction(ui->legend);
    Global::timelineGL->showLinkedRendersDest.setAction(ui->linkedRenders);
    Global::timelineGL->showLinkedTagsDest   .setAction(ui->linkedTags);
    Global::showHelp.setAction(ui->help);
}

Inspector::~Inspector() {
    delete ui;
}

void Inspector::action() {
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    Global::timelineGL->scrollTo();
    Global::viewerGL  ->scrollTo();
}

void Inspector::showEvent(QShowEvent *e) {
    if(toolbarButton)
        toolbarButton->setChecked(true);
    QWidget::showEvent(e);
}
void Inspector::closeEvent(QCloseEvent *e) {
    if(toolbarButton)
        toolbarButton->setChecked(false);
    QWidget::closeEvent(e);
}
