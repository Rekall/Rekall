#ifndef VLCMEDIAPLAYER_H
#define VLCMEDIAPLAYER_H

#include <QString>
#include <QMutex>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QUrl>
#include <QMap>
#ifdef VLC_INSTALLED
#include <vlc/vlc.h>
#endif
#include <vector>
#include <string>
//#define VLC_MODE_PLAYLIST

class VlcPlayer;
struct vlc_callback {
    QMutex        *mutex;
    VlcPlayer     *target;
    unsigned char *pixels;
};

class VlcPlayer : public QObject {
    Q_OBJECT

public:
    explicit VlcPlayer(const QSizeF &_size, const std::vector<std::string> &arguments = std::vector<std::string>(), QObject *parent = 0);
    ~VlcPlayer();

public slots:
    void play();
    void replay();
    void pause();
    void stop();
    void setVolume(qreal vol = 1);
    qreal getVolume();
    void setTime(qreal pos = 0);
    void setPosition(qreal pos = 0);
    qreal getTime();
    qreal getPosition();
    void setUrl(const QUrl &_url, bool _inLoop = false);

signals:
    void frameReady(vlc_callback *);
    void textureUpdate();
    void mediaStartEvent(const libvlc_event_t*);
    void mediaEndEvent(const libvlc_event_t*);

protected slots:
    void processFrame(vlc_callback *callback);

public:
    QImage image;
    QSizeF textureSize, videoSize;
    bool inLoop;
private:
#ifdef VLC_MODE_PLAYLIST
    libvlc_media_list_player_t *playlistPlayer;
#endif
    libvlc_instance_t      *vlcInstance;
    libvlc_media_player_t  *vlcMediaPlayer;
    vlc_callback           *vlcCallback;
    libvlc_event_manager_t *vlcEventManager;
private:
    QUrl url;
    static void* lock(void*, void**);
    static void unlock(void*, void*, void * const*);
    static void vlcStartEvent(const libvlc_event_t *event, void *vlcvideo_object);
    static void vlcEndEvent(const libvlc_event_t *event, void *vlcvideo_object);
};


#endif // VLCMEDIAPLAYER_H
