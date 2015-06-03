#ifndef VIDEOPLAYERS_H
#define VIDEOPLAYERS_H

#include <QWidget>
#include "videoplayer.h"
#include "global.h"

namespace Ui {
class VideoPlayers;
}

class VideoPlayers : public QWidget, public VideoPlayersInterface {
    Q_OBJECT

public:
    explicit VideoPlayers(QWidget *parent = 0);
    ~VideoPlayers();

public:
    void update(const QUrl &url, bool askClose = false, const QString &title = "", qint64 timecode = 0);
    void rewind(qint64 timecode = 0);
    void play(qint64 timecode = -1);
    void pause();

signals:
    void videoSeek(qint64);
    void videoRewind(qint64);
    void videoPlay(qint64);
    void videoPause();

private:
    Ui::VideoPlayers *ui;
};

#endif // VIDEOPLAYERS_H
