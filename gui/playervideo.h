#ifndef PLAYERVIDEO_H
#define PLAYERVIDEO_H

#include <QWidget>
#include <Phonon>

using namespace Phonon;

namespace Ui {
class PlayerVideo;
}

class PlayerVideo : public QWidget {
    Q_OBJECT
    
public:
    explicit PlayerVideo(QWidget *parent = 0);
    ~PlayerVideo();

public:
    void load(const QString &filename);
    void play();
    void pause();
    void seek(qint64 time);
    qint64 currentTime();
    qint64 totalTime();
    void setVolume(qreal volume);
protected:
    void resizeEvent(QResizeEvent *);
    
private:
    Ui::PlayerVideo *ui;
};

#endif // PLAYERVIDEO_H
