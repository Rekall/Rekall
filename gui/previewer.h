#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QWidget>
#include <QUrl>
#include <QFileInfo>
#include <QDesktopServices>
#include <Phonon>
#include "misc/global.h"


using namespace Phonon;

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
    void timerEvent(QTimerEvent *);

private slots:
    void action();
    void actionOpen();
    void actionFinished();

private:
    bool isUpdating;
    qreal volumeOld;
    QPixmap picture;
    QString filename;
public:
    void setVolume(qreal volume);
    void preview(int documentType, const QString &_filename, const QPixmap &_picture);

private:
    Ui::Previewer *ui;
};

#endif // PREVIEWER_H
