#include "vlc.h"

struct StringToCStr {
    const char* operator()(const std::string& str) {
        return str.c_str();
    }
};

VlcPlayer::VlcPlayer(const QSizeF &_textureSize, const std::vector<std::string> &arguments, QObject *parent)
    : QObject(parent) {
    textureSize = _textureSize;
#ifdef VLC_MODE_PLAYLIST
    playlistPlayer = 0;
#endif
    connect(this, SIGNAL(frameReady(vlc_callback *)), SLOT(processFrame(vlc_callback *)));

    //Instance
    std::vector<const char*> argv;
    std::transform(arguments.begin(), arguments.end(), std::back_inserter(argv), StringToCStr());
    vlcInstance = libvlc_new(arguments.size(), &argv[0]);

    //Callback
    vlcCallback = new vlc_callback;
    vlcCallback->mutex = new QMutex;
    vlcCallback->target = this;
    vlcCallback->pixels = new unsigned char[int(sizeof(*(vlcCallback->pixels)) * textureSize.width() * textureSize.height() * 4)];

    //Mediaplayer
    vlcMediaPlayer  = libvlc_media_player_new(vlcInstance);
    libvlc_video_set_format(vlcMediaPlayer, "RV32", textureSize.width(), textureSize.height(), textureSize.width() * 4);

    //Événements
    vlcEventManager = libvlc_media_player_event_manager(vlcMediaPlayer);
    libvlc_event_attach(vlcEventManager, libvlc_MediaPlayerPlaying, vlcStartEvent, this);
    libvlc_event_attach(vlcEventManager, libvlc_MediaPlayerEndReached, vlcEndEvent, this);

    //Mise en service du callback
    libvlc_video_set_callbacks(vlcMediaPlayer, lock, unlock, NULL, vlcCallback);
}

VlcPlayer::~VlcPlayer() {
    stop();
    libvlc_media_player_release(vlcMediaPlayer);
    if (vlcCallback) {
        if (vlcCallback->pixels)
            delete[] vlcCallback->pixels;
        delete vlcCallback->mutex;
        delete vlcCallback;
    }
    libvlc_release(vlcInstance);
}


void VlcPlayer::processFrame(vlc_callback* callback) {
    image = QImage(callback->pixels, textureSize.width(), textureSize.height(), QImage::Format_RGB32);
    emit(textureUpdate());
}

void VlcPlayer::play() {
#ifdef VLC_MODE_PLAYLIST
    libvlc_media_list_player_play(playlistPlayer);
#else
    libvlc_media_player_play(vlcMediaPlayer);
#endif
}

void VlcPlayer::pause() {
#ifdef VLC_MODE_PLAYLIST
    libvlc_media_list_player_pause(playlistPlayer);
#else
    libvlc_media_player_pause(vlcMediaPlayer);
#endif
}

void VlcPlayer::stop() {
#ifdef VLC_MODE_PLAYLIST
    libvlc_media_list_player_stop(playlistPlayer);
#else
    libvlc_media_player_stop(vlcMediaPlayer);
#endif
    //image = QImage();
    //emit(textureUpdate());
}

void VlcPlayer::setTime(qreal pos) {
    libvlc_media_player_set_time(vlcMediaPlayer, pos);
}
void VlcPlayer::setPosition(qreal pos) {
    libvlc_media_player_set_position(vlcMediaPlayer, pos);
}
qreal VlcPlayer::getTime() {
    return libvlc_media_player_get_time(vlcMediaPlayer);
}
qreal VlcPlayer::getPosition() {
    return libvlc_media_player_get_position(vlcMediaPlayer);
}


void VlcPlayer::setVolume(qreal vol) {
    libvlc_audio_set_volume(vlcMediaPlayer, 100 * vol);
}
qreal VlcPlayer::getVolume() {
    return libvlc_audio_get_volume(vlcMediaPlayer) / 100.;
}

void VlcPlayer::replay() {
    stop();
    setPosition(0);
    play();
}
void VlcPlayer::setUrl(const QUrl &_url, bool _inLoop) {
    url    = _url;
    inLoop = _inLoop;
    libvlc_media_t *media = libvlc_media_new_location(vlcInstance, url.toString().toLocal8Bit().constData());

#ifdef VLC_MODE_PLAYLIST
    libvlc_media_list_t *playlist = libvlc_media_list_new(vlcInstance);

    libvlc_media_list_add_media(playlist, media);

    playlistPlayer = libvlc_media_list_player_new(vlcInstance);
    if(inLoop)
        libvlc_media_list_player_set_playback_mode(playlistPlayer, libvlc_playback_mode_loop);
    else
        libvlc_media_list_player_set_playback_mode(playlistPlayer, libvlc_playback_mode_default);

    libvlc_media_list_player_set_media_list(playlistPlayer, playlist);
    libvlc_media_list_player_set_media_player(playlistPlayer, vlcMediaPlayer);
#else
    libvlc_media_player_set_media(vlcMediaPlayer, media);
#endif
    libvlc_media_release(media);
}

void* VlcPlayer::lock(void *data, void **pixels) {
    vlc_callback *callback = static_cast<vlc_callback *>(data);
    callback->mutex->lock();
    *pixels = callback->pixels;

    return NULL;
}
void VlcPlayer::unlock(void *data, void *, void *const *) {
    vlc_callback *callback = static_cast<vlc_callback *>(data);
    callback->mutex->unlock();
    callback->target->emit frameReady(callback);
}
void VlcPlayer::vlcStartEvent(const libvlc_event_t *event, void *vlcvideo_object) {
    if((event) && (vlcvideo_object)) {
        VlcPlayer* thiss = (VlcPlayer*)vlcvideo_object;
        quint32 width, height=0;
        libvlc_video_get_size(thiss->vlcMediaPlayer, 0, &width, &height);
        thiss->videoSize = QSizeF(width, height);
        thiss->emit mediaStartEvent(event);
    }
}
void VlcPlayer::vlcEndEvent(const libvlc_event_t *event, void *vlcvideo_object) {
    if((event) && (vlcvideo_object)) {
        VlcPlayer* thiss = (VlcPlayer*)vlcvideo_object;
        thiss->emit mediaEndEvent(event);
    }
}
