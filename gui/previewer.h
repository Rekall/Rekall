#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QWidget>
#include <QFileInfo>
#include <QProcess>
#include <QWebFrame>
#include <QWebPage>
#include <QDesktopServices>

namespace Ui {
class Previewer;
}

class Previewer : public QWidget {
    Q_OBJECT
    
public:
    explicit Previewer(const QString &_pathApp, QWidget *parent = 0);
    ~Previewer();

protected:
    void resizeEvent(QResizeEvent *);

private:
    QString pathApp;
    QPair<QString, QPixmap> picture;

public:
    void displayPixmap(const QPair<QString, QPixmap> &_picture);
    void displayGps(const QPair<QString,QString> &gps);
    void displayGps(const QString &gps, const QString &place);

public slots:
    void action();
    
private:
    Ui::Previewer *ui;
};

#endif // PREVIEWER_H
