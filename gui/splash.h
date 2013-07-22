#ifndef SPLASH_H
#define SPLASH_H

#include <QWidget>
#include <QDesktopWidget>
#include "misc/global.h"

namespace Ui {
class Splash;
}

class Splash : public QWidget {
    Q_OBJECT
    
public:
    explicit Splash(QWidget *parent = 0);
    ~Splash();
    
private:
    Ui::Splash *ui;
};

#endif // SPLASH_H
