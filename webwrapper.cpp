#include "webwrapper.h"
#include "ui_webwrapper.h"

WebWrapper::WebWrapper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebWrapper) {
    ui->setupUi(this);

    QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - QPoint(width(), height())/2);

    connect(ui->webView, SIGNAL(titleChanged(QString)), SLOT(setWindowTitle(QString)));
}

WebWrapper::~WebWrapper() {
    delete ui;
}

void WebWrapper::openWebPage(const QUrl &url, const QString &title) {
    //QDesktopServices::openUrl(url);
    ui->webView->setUrl(url);
    if(title.isEmpty())
        setWindowTitle(tr("Rekall"));
    else if(title.isEmpty())
        setWindowTitle(tr("Rekall — ") + title);
    show();
    activateWindow();
    raise();
    Global::userInfos->setDockIcon(true);
}

void WebWrapper::closeEvent(QCloseEvent *) {
    Global::userInfos->setDockIcon(false);
}
