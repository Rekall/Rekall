#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class Viewer;
}

class Viewer : public QWidget {
    Q_OBJECT
    
public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();
    
private:
    Ui::Viewer *ui;
};

#endif // VIEWER_H
