#ifndef PREVIEWERLABEL_H
#define PREVIEWERLABEL_H

#include <QLabel>
#include <Phonon>

using namespace Phonon;

class PreviewerLabel : public QLabel {
    Q_OBJECT
public:
    explicit PreviewerLabel(QWidget *parent = 0);

signals:
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};


class PreviewerVideoPlayer : public Phonon::VideoPlayer {
    Q_OBJECT
public:
    explicit PreviewerVideoPlayer(QWidget *parent = 0);

signals:
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};

#endif // PREVIEWERLABEL_H
