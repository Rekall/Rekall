#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer) {
    ui->setupUi(this);
}

Viewer::~Viewer() {
    delete ui;
}
