/*
    This file is part of Rekall.
    Copyright (C) 2013-2014

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMouseEvent>
#include <QProcessEnvironment>
#include <QCryptographicHash>
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

enum DocumentType     { DocumentTypeFile, DocumentTypeVideo, DocumentTypeAudio, DocumentTypeImage, DocumentTypeDoc, DocumentTypeMarker, DocumentTypePeople, DocumentTypeWeb };
enum DocumentFunction { DocumentFunctionContextual, DocumentFunctionRender };
enum DocumentStatus   { DocumentStatusWaiting, DocumentStatusProcessing, DocumentStatusReady };
typedef QMap<QString, MetadataElement> QMetaMap;
class QMetaDictionnay : public QMap<QString, QMetaMap> {
public:
    QString getNameCache, getAuthorCache, getTypeStrCache, getSnapshotCache, getUserNameCache;
    DocumentFunction getFunctionCache;
    DocumentType getTypeCache;
    qreal   getMediaDurationCache;
};


class GlDrawable {
public:
    virtual void actionPlay()       {}
    virtual void fireEvents()       {}
    virtual void seek(qreal, bool = false, bool = false) {}
    virtual bool jumpTo()           { return false; }
    virtual qreal totalTime() const { return 0.;    }
    virtual void closePopups()      {}
public:
    virtual void setDuplicates(quint16) {}
    virtual void setHistories (quint16) {}
public:
    virtual void actionMarkerAddStart() {}
    virtual void actionMarkerAddEnd()   {}
public:
    virtual const QPointF getTimelineCursorPos (qreal) const           { return QPointF(); }
    virtual const QPointF getViewerCursorPos   (qreal) const           { return QPointF(); }
    virtual qreal         getTimelineCursorTime(const QPointF &) const { return 0;         }
    virtual qreal         getViewerCursorTime  (const QPointF &) const { return 0;         }
public:
    virtual const QRectF paintTimeline(bool before = false)                                    = 0;
    virtual const QRectF paintViewer  ()                                                       = 0;
    virtual       bool   mouseTimeline(const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool) = 0;
    virtual       bool   mouseViewer  (const QPointF &, QMouseEvent *, bool, bool, bool, bool, bool) = 0;
};

class GlVideo {
public:
    virtual void load  (void* tag)  = 0;
    virtual void play  (bool state) = 0;
    virtual void unload(void* tag)  = 0;
    virtual void seek  (qreal time) = 0;
    virtual qreal getCurrentTime() const = 0;
};


class GlWidget : public QGLWidget {
public:
    explicit GlWidget(const QGLFormat &format, QWidget *parent) : QGLWidget(format, parent) {
        showLinkedTags = showLinkedRenders = showHashedTags = tagSnap = tagSnapSlow = 0;
        showLinkedTagsDest = showLinkedRendersDest = showLegendDest = showHashedTagsDest = tagSnapDest = tagSnapSlowDest = false;
        mouseTimer.setSingleShot(true);
        mouseTimerOk = false;
        glReady      = false;
    }
public:
    QRectF  drawingBoundingRect, visibleRect;
    QPointF scroll, scrollDest;
    UiBool  showLegendDest, showLinkedRendersDest, showLinkedTagsDest, showHashedTagsDest, tagSnapDest, tagSnapSlowDest;
    qreal   showLinkedRenders, showLinkedTags, showHashedTags, tagSnap, tagSnapSlow;
    bool    glReady;
protected:
    QTimer  mouseTimer;
    QPointF mouseTimerPos;
    bool    mouseTimerOk;
public:
    void        ensureVisible(const QPointF &point, qreal ratio = 0.5);
    void        scrollTo     (const QPointF &point);
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
    void drawText(const QPoint &pos = QPoint(0, 0));
};

class GlRect {
public:
    QSize   size;
    QImage  image;
    QString currentFilename;
private:
    GLuint  texture;
    bool    init;
public:
    explicit GlRect() { init = false; texture = 0; }
    explicit GlRect(const QString &filename, const QSize &desiredSize = QSize(-1,-1)) { texture = 0; setTexture(filename, desiredSize); }
public:
    void        setTexture (const QString &filename, const QSize &desiredSize = QSize(-1,-1));
    void        drawTexture(const QRectF &rect, qreal croppingMode = -2);
    void        drawTexture(const QString &filename, const QRectF &rect, qreal croppingMode = -2);
    static void drawRoundedRect(const QRectF &rect, bool border, qreal precision = M_PI/4);
    static void drawRect(const QRectF &rect, qreal borderRadius = 0, const QRectF &texCoord = QRectF(0,0,1,1));
};
typedef QList<GlRect> Thumbnails;









class WatcherBase : public QObject {
public:
    QFileSystemWatcher *watcher;
public:
    explicit WatcherBase(QObject *parent = 0) : QObject(parent) {}
    virtual void sync  (const QString &file, bool inTracker = false) = 0;
    virtual void unsync(const QString &file, bool inTracker = false) = 0;
public slots:
    virtual void fileWatcherDirChanged (QString) = 0;
    virtual void fileWatcherFileChanged(QString) = 0;
    virtual void writeNote() = 0;
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
    virtual void addPerson  (void* person) = 0;
};
class TaskListBase {
public:
    virtual void setToolbox(QToolBox*) = 0;
    virtual void addTask   (Metadata *metadata, TaskProcessType type, qint16 version, bool needCompleteScan = true) = 0;
    virtual void clearTasks() = 0;
};

enum FeedItemBaseType { FeedItemBaseTypeCreation, FeedItemBaseTypeUpdate, FeedItemBaseTypeDelete, FeedItemBaseTypeProcessingStart, FeedItemBaseTypeProcessingEnd };
class FeedItemBase : public QTreeWidgetItem {
public:
    explicit FeedItemBase(FeedItemBaseType _action, const QString &_author, const QString &_object, const QDateTime &_date = QDateTime::currentDateTime());

private:
    FeedItemBaseType action;
    QString          actionStr, object;
    QIcon            icon;
public:
    QString   author;
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
    qint32 updateDecounter;
public:
    virtual const QString         getInfo(const QString &key) = 0;
    virtual const QMetaDictionnay getInfos()                  = 0;
    virtual void                  update()                    = 0;
};

class RekallBase : public QMainWindow {
public:
    QTreeWidget *personsTreeWidget;
public:
    explicit RekallBase(QWidget *parent = 0) : QMainWindow(parent) {}
public:
    virtual void setVisbility(bool) = 0;
    virtual void displayMetadataAndSelect(void *tag = 0)  = 0;
    virtual void displayMetadata(QTreeWidgetItem * = 0, QTreeWidgetItem * = 0)  = 0;
    virtual void showPreviewTab()   = 0;
public:
    virtual bool parseMimeData(const QMimeData *, const QString &, bool = false) = 0;
};




enum TagSelection { TagSelectionStart, TagSelectionEnd, TagSelectionMove, TagSelectionMediaOffset, TagSelectionDuplicate };
enum TagType      { TagTypeContextualMilestone, TagTypeContextualTime, TagTypeGlobal };




class Global {
public:
    static bool falseProject;
    static UserInfosBase *userInfos;
    static QImage temporaryScreenshot;
    static qreal thumbsEach, waveEach;
    static QFileInfo pathApplication, pathDocuments, pathCurrent, pathCurrentDefault;
    static GlWidget *timelineGL, *viewerGL;
    static ProjectBase *currentProject;
    static GlDrawable *timeline;
    static QTreeWidget *chutier;
    static qreal timeUnit, timeUnitTick;
    static UiReal timeUnitDest, timelineTagHeightDest;
    static UiBool showHelp, showHistory;
    static qreal timelineTagHeight, timelineTagVSpacing, timelineTagVSpacingSeparator, timelineTagThumbHeight;
    static qreal viewerTagHeight, timelineGlobalDocsWidth;
    static QSizeF timelineHeaderSize;
    static bool  timerPlay;
    static qreal time, thumbnailSlider, thumbnailSliderStep;
    static qreal tagBlinkTime;
    static qreal breathing, breathingDest, breathingFast, breathingFastDest, breathingPics, breathingPicsDest;
    static QTime timer;
    static Sorting *tagSortCriteria, *tagColorCriteria, *tagTextCriteria, *tagClusterCriteria, *tagFilterCriteria, *tagHorizontalCriteria;
    static Phases *phases;
    static void *selectedTagHover, *timeMarkerAdded;
    static QList<void*> selectedTags, selectedTagsInAction;
    static QPair<qreal, qreal> selectedTagHoverSnapped;
    static QMap<QString,void*> renders;
    static qreal selectedTagStartDrag;
    static TagSelection selectedTagMode;
    static qreal inertie;
    static Udp *udp;
    static QFont font, fontSmall;
    static GlVideo *video;
    static QColor colorAlternateStrong, colorAlternate, colorAlternateLight, colorAlternateMore, colorCluster, colorText, colorBackground, colorTicks, colorTextBlack, colorTagCaptation, colorTagDisabled, colorTimeline;
    static QMap<QString, QPair<QColor, qreal> > colorForMeta;
    static bool timelineSortChanged, viewerSortChanged, eventsSortChanged, metaChanged, ticksChanged;
    static WatcherBase *watcher;
    static RekallBase* mainWindow;
    static TaskListBase *taskList;
    static FeedListBase *feedList;

public:
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
    static QColor getColorScale(qreal val);
    static QString getFileHash(const QFileInfo &file);

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
private:
    bool editable;
public:
    explicit HtmlDelegate(bool _editable = false, QObject *parent = 0);
public:
    static QString removeHtml(QString html);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


#endif // GLOBAL_H
