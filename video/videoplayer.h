#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include "../global.h"

namespace Ui {
class VideoPlayer;
}

class VideoPlayer : public QWidget, public VideoPlayerInterface {
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

private:
    QMediaPlayer *player;
protected:
    void closeEvent(QCloseEvent *);

public slots:
    void setUrl(const QUrl &url, bool askClose = false, const QString &title = "");
    void seek(qint64 timecode);
    void play(qint64 timecode = -1);
    void pause();
    void rewind(qint64 timecode = 0);
    void forceClose();

private:
    Ui::VideoPlayer *ui;
};

#endif // VIDEOPLAYER_H
