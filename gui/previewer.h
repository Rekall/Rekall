#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QWidget>

namespace Ui {
class Previewer;
}

class Previewer : public QWidget {
    Q_OBJECT
    
public:
    explicit Previewer(QWidget *parent = 0);
    ~Previewer();

protected:
    void resizeEvent(QResizeEvent *);

private:
    QPair<QString, QPixmap> picture;
public:
    void displayPixmap(const QPair<QString, QPixmap> &_picture);
    
private:
    Ui::Previewer *ui;
};

#endif // PREVIEWER_H
