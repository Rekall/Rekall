#include "webwrapper.h"
#include "ui_webwrapper.h"

WebWrapper::WebWrapper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebWrapper) {
    ui->setupUi(this);

    QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - QPoint(width(), height())/2);

    connect(ui->webView, SIGNAL(titleChanged(QString)), SLOT(setWindowTitle(QString)));
    restoreGeometry(QSettings().value("WebWrapperGeometry").toByteArray());
}

WebWrapper::~WebWrapper() {
    delete ui;
}

void WebWrapper::openWebPage(const QUrl &url, const QString &title) {
    //QDesktopServices::openUrl(url);

    Global::userInfos->setDockIcon(this, true);
    ui->webView->setUrl(url);
    if(title.isEmpty())
        setWindowTitle(tr("Rekall"));
    else
        setWindowTitle(title);
    show();
    activateWindow();
    raise();
}

void WebWrapper::closeEvent(QCloseEvent *) {
    foreach(ProjectInterface *project, Global::projects)
        foreach(VideoPlayerInterface *videoPlayers, project->videoPlayers)
            videoPlayers->forceClose();

    Global::userInfos->setDockIcon(this, false);
    QSettings().setValue("WebWrapperGeometry", saveGeometry());
}
