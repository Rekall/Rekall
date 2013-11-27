#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <Phonon>
#include "core/project.h"

namespace Ui {
class Player;
}

using namespace Phonon;

class Player : public QWidget, public GlVideo, public Resizable {
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    ~Player();

private:
    QList<Tag*> tags;
public:
    void load(void* tag);
    void play(bool state);
    void unload(void* _tag);
    void seek(qreal time);
    qreal getCurrentTime() const;

protected:
    void globalPlayPause();
    void forceResizeEvent();
    void resizeEvent(QResizeEvent *);

private:
    Ui::Player *ui;
};

#endif // PLAYER_H
