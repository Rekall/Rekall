#ifndef WEBWRAPPER_H
#define WEBWRAPPER_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>
#include "global.h"

namespace Ui {
class WebWrapper;
}

class WebWrapper : public QMainWindow, public WebWrapperInterface {
    Q_OBJECT

public:
    explicit WebWrapper(QWidget *parent = 0);
    ~WebWrapper();

public slots:
    void openWebPage(const QUrl &url, const QString &title = "", bool inBrowser = false);

public:
    void closeEvent(QCloseEvent *);

private:
    Ui::WebWrapper *ui;
};

#endif // WEBWRAPPER_H
