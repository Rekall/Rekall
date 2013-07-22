#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMouseEvent>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QGLWidget>
#include <QTreeWidget>
#include <QTimer>
#include <Phonon>
#include <QImage>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include "core/sorting.h"
#include "core/phases.h"
#include "misc/options.h"
#include "gui/previewer.h"
#include "interfaces/udp.h"
#include "interfaces/location.h"
#include "qmath.h"

using namespace Phonon;

class GlDrawable {
public:
    virtual void fireEvents()         {}
    virtual void askDisplayMetaData() {}
    virtual qreal totalTime() const   { return 0.; }
public:
    virtual QPointF getTimelineCursorPos (qreal)           { return QPointF(); }
    virtual QPointF getViewerCursorPos   (qreal)           { return QPointF(); }
    virtual qreal   getTimelineCursorTime(const QPointF &) { return 0;         }
    virtual qreal   getViewerCursorTime  (const QPointF &) { return 0;         }
public:
    virtual const QRectF paintTimeline(bool before = false) = 0;
    virtual const QRectF paintViewer()   = 0;
    virtual bool  mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool) = 0;
    virtual bool  mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool) = 0;
};

class GlVideo {
public:
    virtual void load(void* tag) = 0;
    virtual void play(bool state) = 0;
    virtual void unload(void* tag) = 0;
    virtual void seek(qreal time) = 0;
    virtual qreal getCurrentTime() const = 0;
};

class GlWidget : public QGLWidget {
public:
    explicit GlWidget(const QGLFormat &format, QWidget *parent) : QGLWidget(format, parent) {
        showLinkedTags = showLinkedRenders = 0;
        showLinkedTagsDest = showLinkedRendersDest = showLegendDest = false;
        mouseTimer.setSingleShot(true);
        mouseTimerOk = false;
    }
public:
    QTimer mouseTimer;
    QPointF mouseTimerPos;
    bool mouseTimerOk;
    QRectF drawingBoundingRect, visibleRect;
    QPointF scroll, scrollDest;
    UiBool showLegendDest, showLinkedRendersDest, showLinkedTagsDest;
    qreal showLinkedRenders, showLinkedTags;
public:
    void ensureVisible(const QPointF &point, qreal ratio = 0.5);
    void scrollTo(const QPointF &point);
    inline void scrollTo() { scrollTo(scrollDest); }
};

class WatcherBase {
public:
    QFileSystemWatcher *watcher;
public:
    virtual void sync  (const QString &file, bool inTracker = false) = 0;
    virtual void unsync(const QString &file, bool inTracker = false) = 0;
public slots:
    virtual void fileWatcherDirChanged(QString) = 0;
    virtual void fileWatcherFileChanged(QString) = 0;
};

enum TaskProcessType { TaskProcessTypeProcess, TaskProcessTypeMetadata };
class Metadata;
class ProjectBase {};
class TaskListBase {
public:
    virtual void addTask(Metadata *metadata, TaskProcessType type, qint16 version) = 0;
};



class GlText {
public:
    GLuint  texture;
    QFont   font;
    QImage  image;
    bool    init;
    int     alignement;
    QSize   size;
    QString text;
public:
    explicit GlText() { init = false; texture = 0; }
public:
    void setStyle(const QSize &_size, int _alignement, const QFont &_font);
    void setText(const QString &text);
    void drawText(const QString &text,const QPoint &pos = QPoint(0, 0));
    void drawText(const QPoint &pos);
};

class GlRect {
public:
    QSize   size;
    QImage  image;
private:
    GLuint  texture;
    QString filenameOld;
    bool    init;
public:
    explicit GlRect() { init = false; texture = 0; }
    explicit GlRect(const QString &filename, const QSize &desiredSize = QSize(-1,-1)) { texture = 0; setTexture(filename, desiredSize); }
public:
    void setTexture(const QString &filename, const QSize &desiredSize = QSize(-1,-1));
    void drawTexture(const QRectF &rect, qreal croppingMode = -2);
    void drawTexture(const QString &filename, const QRectF &rect, qreal croppingMode = -2);
    static void drawRoundedRect(const QRectF &rect, bool border, qreal precision = M_PI/4);
    static void drawRect(const QRectF &rect, qreal borderRadius = 0, const QRectF &texCoord = QRectF(0,0,1,1));
};


enum TagSelection { TagSelectionStart, TagSelectionEnd, TagSelectionMove };
enum TagType      { TagTypeContextualMilestone, TagTypeContextualTime };


class Global {
public:
    static QString userName;
    static Location *userLocation;
    static QImage temporaryScreenshot;
    static qreal thumbsEach, waveEach;
    static QFileInfo pathApplication, pathDocuments, pathCurrent;
    static GlWidget *timelineGL, *viewerGL;
    static GlDrawable *currentProject, *timeline;
    static QTreeWidget *chutier;
    static qreal timeUnit, timeUnitTick;
    static UiReal timeUnitDest, timelineTagHeightDest;
    static qreal timelineTagHeight, timelineTagVSpacing, timelineTagVSpacingSeparator, timelineTagThumbHeight;
    static qreal viewerTagHeight;
    static QSizeF timelineHeaderSize;
    static bool  timerPlay;
    static qreal time, thumbnailSlider, thumbnailSliderStep;
    static qreal tagBlinkTime;
    static qreal breathing, breathingDest;
    static QTime timer;
    static Sorting *tagSortCriteria, *tagColorCriteria, *tagClusterCriteria, *tagFilterCriteria;
    static Phases *phases;
    static void *selectedTag, *selectedTagHover, *selectedTagHoverSnap;
    static qreal selectedTagHoverSnapped;
    static QMap<QString,void*> renders;
    static qreal selectedTagStartDrag;
    static TagSelection selectedTagMode;
    static qreal inertie;
    static Udp *udp;
    static QFont font, fontSmall, fontLarge;
    static GlVideo *video;
    static QColor colorAlternateStrong, colorAlternate, colorAlternateBold, colorAlternateInv, colorSelection, colorProgression, colorText, colorTextDark, colorBackground;
    static QMap<QString, QPair<QColor, qreal> > colorForMeta;
    static bool timelineSortChanged, viewerSortChanged, eventsSortChanged, metaChanged;
    static WatcherBase *watcher;
    static QMainWindow* mainWindow;
    static TaskListBase *taskList;
    static Previewer *previewer;

public:
    static const QString timeToString(qreal time);
    static const QString dateToString(const QDateTime &date);
    static const QString plurial(qint16 value, const QString &text);
    static const QString cacheFile(const QString &type, const QFileInfo &file) {
        return Global::pathCurrent.absoluteFilePath() + "/rekall_cache/" + type + "_" + file.absoluteDir().absolutePath().remove(Global::pathCurrent.absoluteFilePath()).replace("/", "_") + "_" + file.fileName();
    }

public:
    static void seek(qreal);
    static void play(bool);




public:
    static inline quint16 alea(quint16 min, quint16 max) {
        if(max == min)  return min;
        else            return ((qreal)qrand()/(qreal)RAND_MAX) * (max-min) + min;
    }
    static inline qreal aleaF(qreal min, qreal max) {
        if(max == min)  return min;
        else            return ((qreal)qrand()/(qreal)RAND_MAX) * (max-min) + min;
    }
    static inline qreal exp(qreal val, qreal facteur) {
        if(val <= 0)        return 0;
        else if(val >= 1)   return 1;
        else                return (qExp(facteur * val - facteur) - qExp(-facteur)) / (1 - qExp(-facteur));
    }
};


#endif // GLOBAL_H
