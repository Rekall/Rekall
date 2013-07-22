#include "splash.h"
#include "ui_splash.h"

Splash::Splash(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Splash) {
    ui->setupUi(this);

    ui->logo->setPixmap(QString(":/spash/splash-%1.jpg").arg(Global::alea(0, 19)));

    setWindowFlags(Qt::FramelessWindowHint);
    ui->version->setText(tr("version") + " " + QString(QCoreApplication::applicationVersion()) + " " + tr("beta"));
    QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
}

Splash::~Splash() {
    delete ui;
}
