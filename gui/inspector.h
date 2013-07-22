#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class Inspector;
}

class Inspector : public QWidget {
    Q_OBJECT
    
public:
    explicit Inspector(QWidget *parent = 0);
    ~Inspector();

private slots:
    void action();

public:
    QAction *toolbarButton;
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::Inspector *ui;
};

#endif // INSPECTOR_H
