#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QWidget>
#include <QUrl>
#include <QDesktopServices>

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

private slots:
    void action();

private:
    QPair<QString, QPixmap> picture;
public:
    void displayPixmap(const QPair<QString, QPixmap> &_picture);
    
private:
    Ui::Previewer *ui;
};

#endif // PREVIEWER_H
