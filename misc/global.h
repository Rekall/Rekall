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
#include <QToolBox>
#include <QImage>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QStyledItemDelegate>
#include <QApplication>
#include "core/sorting.h"
#include "core/phases.h"
#include "misc/options.h"
#include "gui/playervideo.h"
#include "interfaces/udp.h"
#include "qmath.h"

typedef QMap<QString, MetadataElement> QMetaMap;
typedef QMap<QString, QMetaMap > QMetaDictionnay;


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
typedef QList<GlRect> Thumbnails;




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
class ProjectBase : public QObject, public GlDrawable {
public:
    quint32 noteId;
public:
    explicit ProjectBase(QObject *parent) : QObject(parent) {
        noteId = 0;
    }
public:
    virtual void addDocument(void *document) = 0;
    virtual void addPerson(void* person) = 0;
};
class TaskListBase {
public:
    virtual void setToolbox(QToolBox*) = 0;
    virtual void addTask(Metadata *metadata, TaskProcessType type, qint16 version) = 0;
};

enum FeedItemBaseType { FeedItemBaseTypeCreation, FeedItemBaseTypeUpdate, FeedItemBaseTypeDelete, FeedItemBaseTypeProcessingStart, FeedItemBaseTypeProcessingEnd };
class FeedItemBase : public QTreeWidgetItem {
public:
    explicit FeedItemBase(FeedItemBaseType _action, const QString &_author, const QString &_object, const QDateTime &_date = QDateTime::currentDateTime());

public:
    QIcon icon;
    FeedItemBaseType action;
    QString object, author, actionStr;
    QDateTime date;
public:
    void update();
};
class FeedListBase {
public:
    virtual void setToolbox(QToolBox*) = 0;
    virtual void addFeed(FeedItemBase *feedItem) = 0;
};

class UserInfosBase {
public:
    virtual const QString getInfo(const QString &key) = 0;
    virtual void update() = 0;
    virtual const QMetaDictionnay getInfos() = 0;
};

class RekallBase : public QMainWindow {
public:
    QTreeWidget *personsTreeWidget;
public:
    explicit RekallBase(QWidget *parent = 0) : QMainWindow(parent) {}
public:
    virtual void setVisbility(bool) = 0;
    virtual void displayDocumentName(const QString &documentName = "") = 0;
    virtual void displayPixmap(const QPair<QString, QPixmap> &_picture) = 0;
    virtual void displayGps(const QList< QPair<QString,QString> > &gps) = 0;
public:
    virtual bool parseMimeData(const QMimeData *mime, const QString &source, bool test = false) = 0;
};







enum TagSelection { TagSelectionStart, TagSelectionEnd, TagSelectionMove };
enum TagType      { TagTypeContextualMilestone, TagTypeContextualTime, TagTypeGlobal };


class Global {
public:
    static UserInfosBase *userInfos;
    static QImage temporaryScreenshot;
    static qreal thumbsEach, waveEach;
    static QFileInfo pathApplication, pathDocuments, pathCurrent;
    static GlWidget *timelineGL, *viewerGL;
    static ProjectBase *currentProject;
    static GlDrawable *timeline;
    static QTreeWidget *chutier;
    static qreal timeUnit, timeUnitTick;
    static UiReal timeUnitDest, timelineTagHeightDest;
    static qreal timelineTagHeight, timelineTagVSpacing, timelineTagVSpacingSeparator, timelineTagThumbHeight;
    static qreal viewerTagHeight, timelineNegativeHeaderWidth;
    static QSizeF timelineHeaderSize;
    static bool  timerPlay;
    static qreal time, thumbnailSlider, thumbnailSliderStep;
    static qreal tagBlinkTime;
    static qreal breathing, breathingDest, breathingFast, breathingFastDest, breathingPics, breathingPicsDest;
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
    static RekallBase* mainWindow;
    static TaskListBase *taskList;
    static FeedListBase *feedList;

public:
    static const QString timeToString(qreal time);
    static const QString dateToString(const QDateTime &date, bool addExactTime = true);
    static const QString plurial(qint16 value, const QString &text);
    static const QString cacheFile(const QString &type, const QFileInfo &file) {
        return cacheFile(type, file.absoluteDir().absolutePath().remove(Global::pathCurrent.absoluteFilePath()).replace("/", "_") + "_" + file.fileName());
    }
    static const QString cacheFile(const QString &type, const QString &info) {
        return Global::pathCurrent.absoluteFilePath() + "/rekall_cache/" + type + "_" + info;
    }
    static QString getBetween(const QString &data, const QString &start, const QString &end, bool trim = true);
    static qreal getDurationFromString(QString timeStr);
    static QPair<QString,QString> seperateMetadata(const QString &metaline, const QString &separator = QString(":"));
    static QPair<QString, QPair<QString,QString> > seperateMetadataAndGroup(const QString &metaline, const QString &separator = QString(":"));

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



class HtmlDelegate : public QStyledItemDelegate {
protected:
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};


#endif // GLOBAL_H
