#include "previewer.h"
#include "ui_previewer.h"

Previewer::Previewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Previewer) {
    ui->setupUi(this);
}

Previewer::~Previewer() {
    delete ui;
}

void Previewer::resizeEvent(QResizeEvent *) {
    if((picture.second.size().width() > 0) && (picture.second.size().height() > 0)) {
        QSizeF targetSize = QSizeF(QSizeF(picture.second.size()) / (qreal)qMax(picture.second.width(), picture.second.height()) * (qreal)qMax(ui->type->width(), ui->type->height()));
        qreal scaleDepassement = qMax(targetSize.width() / (qreal)ui->type->width(), targetSize.height() / (qreal)ui->type->height());
        if(scaleDepassement > 1)
            targetSize = QSizeF(targetSize / scaleDepassement);
        ui->picture->resize(QSize(targetSize.width() - 20, targetSize.height() - 20));
        ui->picture->move(ui->type->width() / 2 - ui->picture->width() / 2, ui->type->height() / 2 - ui->picture->height() / 2);
    }
}
void Previewer::displayPixmap(const QPair<QString, QPixmap> &_picture) {
    if(!_picture.second.isNull()) {
        picture = _picture;
        ui->picture->setPixmap(picture.second);
        resizeEvent(0);
        ui->picture->setVisible(true);
    }
    else
        ui->picture->setVisible(false);
    resizeEvent(0);
}
