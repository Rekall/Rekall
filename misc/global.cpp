#include "global.h"

QImage       Global::temporaryScreenshot;
QFileInfo    Global::pathApplication;
QFileInfo    Global::pathDocuments;
QFileInfo    Global::pathCurrent;
GlWidget*    Global::timelineGL                   = 0;
GlWidget*    Global::viewerGL                     = 0;
GlDrawable*  Global::timeline                     = 0;
qreal        Global::thumbsEach                   = 5;
qreal        Global::waveEach                     = 0.1;
qreal        Global::timeUnit                     = 0;
UiReal       Global::timeUnitDest                 = 13;
qreal        Global::timelineTagHeight            = 0;
UiReal       Global::timelineTagHeightDest        = 7;
qreal        Global::timeUnitTick                 = 5;
qreal        Global::timelineTagThumbHeight       = 40;
qreal        Global::timelineTagVSpacing          = 2;
qreal        Global::timelineTagVSpacingSeparator = 5;
QSizeF       Global::timelineHeaderSize           = QSizeF(240, 15);
qreal        Global::timelineNegativeHeaderWidth  = 35;
qreal        Global::viewerTagHeight              = 30;
qreal        Global::time                         = 0;
qreal        Global::thumbnailSlider              = 0;
qreal        Global::thumbnailSliderStep          = 0.001;
qreal        Global::tagBlinkTime                 = 2000;
qreal        Global::breathing                    = 0;
qreal        Global::breathingDest                = 1;
qreal        Global::breathingPics                = 0;
qreal        Global::breathingPicsDest            = 1;
qreal        Global::breathingFast                = 0;
qreal        Global::breathingFastDest            = 1;
QTime        Global::timer;
UiBool       Global::showHelp                     = true;
bool         Global::timerPlay                    = false;
void*        Global::selectedTag                  = 0;
void*        Global::selectedTagHover             = 0;
void*        Global::selectedTagHoverSnap         = 0;
qreal        Global::selectedTagHoverSnapped      = -1;
QMap<QString,void*> Global::renders;
TagSelection Global::selectedTagMode              = TagSelectionMove;
qreal        Global::selectedTagStartDrag         = 0;
qreal        Global::inertie                      = 5;
Udp*         Global::udp                          = 0;
QFont        Global::font;
QFont        Global::fontSmall;
QFont        Global::fontLarge;
GlVideo*     Global::video                        = 0;
QTreeWidget*  Global::chutier                      = 0;
ProjectBase*  Global::currentProject               = 0;
UserInfosBase* Global::userInfos                = 0;

QColor       Global::colorAlternate               = QColor(255, 255, 255, 12);
QColor       Global::colorAlternateStrong         = QColor(0, 0, 0, 128);
QColor       Global::colorCluster                 = QColor(255, 255, 255, 62);

QColor       Global::colorTicks                   = QColor(43, 46, 47);
QColor       Global::colorSelection               = QColor(50, 221, 255);
QColor       Global::colorProgression             = QColor(50, 221, 255);//QColor(45, 202, 225);
QColor       Global::colorText                    = QColor(245, 248, 250);
QColor       Global::colorTextDark                = Global::colorText.darker(120);
QColor       Global::colorTextBlack               = QColor(45, 50, 53);
QColor       Global::colorBackground              = QColor(71, 77, 79);
QColor       Global::colorBackgroundDark          = QColor(41, 44, 45);
QColor       Global::colorBackgroundMidDark       = QColor(51, 61, 63);


bool         Global::timelineSortChanged          = true;
bool         Global::metaChanged                  = true;
bool         Global::viewerSortChanged            = true;
bool         Global::eventsSortChanged            = true;
QMap<QString, QPair<QColor, qreal> > Global::colorForMeta;
Sorting*     Global::tagSortCriteria    = 0;
Sorting*     Global::tagColorCriteria   = 0;
Sorting*     Global::tagClusterCriteria = 0;
Sorting*     Global::tagFilterCriteria  = 0;
Phases*      Global::phases             = 0;
WatcherBase* Global::watcher            = 0;
RekallBase*  Global::mainWindow         = 0;
TaskListBase*  Global::taskList         = 0;
FeedListBase*  Global::feedList         = 0;

void Global::seek(qreal _time) {
    time = _time;
    if(Global::video)
        Global::video->seek(time);
}
void Global::play(bool state) {
    if(Global::video)
        Global::video->play(state);
    Global::timer.restart();
    Global::timerPlay = state;
}


const QString Global::timeToString(qreal time) {
    QString timeStr = "";

    quint16 min = time / 60;
    if(min < 10)        timeStr += "0";
    else if(min < 100)  timeStr += "";
    timeStr += QString::number(min) + ":";

    quint8 sec = qFloor(time) % 60;
    if(sec < 10) timeStr += "0";
    timeStr += QString::number(sec);

    return timeStr;
}
const QString Global::dateToString(const QDateTime &date, bool addExactTime) {
    quint16 daysTo   = date.daysTo(QDateTime::currentDateTime());
    quint16 secsTo   = date.secsTo(QDateTime::currentDateTime());
    quint16 minsTo   = secsTo / 60;
    quint16 hoursTo  = secsTo / 3600;
    quint16 weeksTo  = daysTo / 7;
    quint16 monthsTo = daysTo / 30;

    if(addExactTime) {
        if(monthsTo > 12)     return QString("on %1").arg(date.toString("dddd dd MM yyyy, hh:mm"));
        else if(monthsTo > 1) return QString("%1 ago, on %2").arg(plurial(monthsTo, "month" )).arg(date.toString("dddd dd MM hh:mm"));
        else if(weeksTo > 1)  return QString("%1 ago, on %2").arg(plurial(weeksTo,  "week"  )).arg(date.toString("dddd dd MM hh:mm"));
        else if(daysTo > 1)   return QString("%1 ago, on %2").arg(plurial(daysTo,   "day"   )).arg(date.toString("dddd, hh:mm"));
        else if(hoursTo > 1)  return QString("%1 ago, on %2").arg(plurial(hoursTo,  "hour"  )).arg(date.toString("hh:mm"));
        else if(minsTo  > 1)  return QString("%1 ago, on %2").arg(plurial(minsTo,   "minute")).arg(date.toString("hh:mm"));
        else if(secsTo  > 10) return QString("%1 ago, on %2").arg(plurial(secsTo,   "second")).arg(date.toString("hh:mm:ss"));
        else                  return QString("A few seconds ago, on %1").arg(date.toString("hh:mm:ss"));
    }
    else {
        if(monthsTo > 12)     return QString("on %1").arg(date.toString("dddd dd MM yyyy, hh:mm"));
        else if(monthsTo > 1) return QString("%1 ago").arg(plurial(monthsTo, "month" ));
        else if(weeksTo > 1)  return QString("%1 ago").arg(plurial(weeksTo,  "week"  ));
        else if(daysTo > 1)   return QString("%1 ago").arg(plurial(daysTo,   "day"   ));
        else if(hoursTo > 1)  return QString("%1 ago").arg(plurial(hoursTo,  "hour"  ));
        else if(minsTo  > 1)  return QString("%1 ago").arg(plurial(minsTo,   "minute"));
        else if(secsTo  > 10) return QString("%1 ago").arg(plurial(secsTo,   "second"));
        else                  return QString("A few seconds ago");
    }
}
const QString Global::plurial(qint16 value, const QString &text) {
    if(qAbs(value) > 1) return QString("%1 %2s") .arg(value).arg(text);
    else                return QString("%1 %2").arg(value).arg(text);
}

QString Global::getBetween(const QString &data, const QString &start, const QString &end, bool trim) {
    qint16 startIndex = data.indexOf(start)+start.length()+1;
    qint16 endIndex   = data.indexOf(end, startIndex);
    if(trim)    return data.mid(startIndex, endIndex-startIndex).trimmed();
    else        return data.mid(startIndex, endIndex-startIndex);
}
qreal Global::getDurationFromString(QString timeStr) {
    qreal duration = 0;
    if(timeStr.length() > 0) {
        timeStr.remove("seconds").remove("second").remove("secs").remove("sec").remove("s");
        QStringList parts = timeStr.split(" ").first().split(":");
        if(parts.count() == 3) {
            duration += parts.at(0).toDouble()*3600;
            duration += parts.at(1).toDouble()*60;
            duration += parts.at(2).split(".").first().toDouble();
        }
        else
            duration += parts.last().split(".").first().toDouble();
    }
    return duration;
}
QPair<QString,QString> Global::seperateMetadata(const QString &metaline, const QString &separator) {
    QPair<QString,QString> retour;
    qint16 index = metaline.indexOf(separator);
    if(index > 0) {
        retour.first = metaline.left(index).trimmed();
        retour.second = metaline.right(metaline.length() - index - 1).trimmed();
    }
    return retour;
}
QPair<QString, QPair<QString,QString> > Global::seperateMetadataAndGroup(const QString &metaline, const QString &separator) {
    QPair<QString, QPair<QString,QString> > retour;
    QPair<QString,QString> firstRetour = seperateMetadata(metaline, "]");
    retour.first = firstRetour.first.remove("[").trimmed();
    retour.second = seperateMetadata(firstRetour.second, separator);
    return retour;
}


QColor Global::getColorScale(qreal val) {
    QList<QColor> colors;
    colors << QColor(255,84,79) << QColor(181,134,118) << QColor(255,147,102) << QColor(255,234,136) << QColor(166,204,91) << QColor(74,201,159) << QColor(102,206,226) << QColor(123,144,206) << QColor(229,149,205) << QColor(226,226,226);
    qreal indexFloat = val * (colors.count()-1);
    quint16 indexLow = qFloor(indexFloat), indexSup = qMin(indexLow+1, colors.count()-1); indexFloat = indexFloat - indexLow;
    return QColor(colors.at(indexLow).red()   * (1.-indexFloat) + colors.at(indexSup).red()   * (indexFloat),
                  colors.at(indexLow).green() * (1.-indexFloat) + colors.at(indexSup).green() * (indexFloat),
                  colors.at(indexLow).blue()  * (1.-indexFloat) + colors.at(indexSup).blue()  * (indexFloat),
                  colors.at(indexLow).alpha() * (1.-indexFloat) + colors.at(indexSup).alpha() * (indexFloat));
}
QString Global::getFileHash(const QFileInfo &file) {
    QCryptographicHash fileHasher(QCryptographicHash::Sha1);
    QFile fileToHash(file.absoluteFilePath());
    fileToHash.open(QFile::ReadOnly);
    while(!fileToHash.atEnd())
        fileHasher.addData(fileToHash.read(8192));
    return QString(fileHasher.result().toHex()).toUpper();
}


FeedItemBase::FeedItemBase(FeedItemBaseType _action, const QString &_author, const QString &_object, const QDateTime &_date) {
    action = _action;
    author = _author;
    object = _object;
    date   = _date;
    if     (action == FeedItemBaseTypeCreation) {
        icon = QIcon(":/icons/events/download.gif");
        actionStr = "added";
    }
    else if(action == FeedItemBaseTypeUpdate) {
        icon = QIcon(":/icons/events/pencil.gif");
        actionStr = "updated";
    }
    else if(action == FeedItemBaseTypeDelete) {
        icon = QIcon(":/icons/events/x.gif");
        actionStr = "removed";
    }
    else if(action == FeedItemBaseTypeProcessingStart) {
        icon = QIcon(":/icons/events/bookmark.gif");
        actionStr = "started metadata extraction on";
    }
    else if(action == FeedItemBaseTypeProcessingEnd) {
        icon = QIcon(":/icons/events/bookmark.gif");
        actionStr = "finished metadata extraction on";
    }
}
void FeedItemBase::update() {
    setIcon(0, icon);
    setText(0, QString("<span style='font-family: Calibri, Arial; font-size: 10px; color: #F5F8FA'>%1 <span style='color: #A1A5A7'>%2</span> %3<span style='color: #A1A5A7'>, %4</span></span>").arg(author).arg(actionStr).arg(object).arg(Global::dateToString(date, false).toLower()));
    setToolTip(0, Global::dateToString(date));
}




void GlWidget::ensureVisible(const QPointF &point, qreal ratio) {
    QRectF rect(QPointF(0, 0), size());
    if(!rect.translated(scroll).contains(point)) {
        QPointF pseudoCenter = rect.topLeft() + QPointF(rect.width() * ratio, rect.height() * ratio);
        if(point.y() >= 0)  scrollTo(QPointF(scrollDest.x(), point.y() - pseudoCenter.y()));
        if(point.x() >= 0)  scrollTo(QPointF(point.x() - pseudoCenter.x(), scrollDest.y()));
    }
}
void GlWidget::scrollTo(const QPointF &point) {
    scrollDest.setX(qBound(0., point.x(), qMax(0., drawingBoundingRect.right()  + 200 - width())));
    scrollDest.setY(qBound(0., point.y(), qMax(0., drawingBoundingRect.bottom() + 200 - height())));
}


void GlText::setStyle(const QSize &_size, int _alignement, const QFont &_font) {
    size       = _size;
    font       = _font;
    init       = false;
    alignement = _alignement;
    image      = QImage(size, QImage::Format_ARGB32_Premultiplied);
    text       = "";
}

void GlText::drawText(const QString &newtext, const QPoint &pos) {
    setText(newtext);
    drawText(pos);
}
void GlText::setText(const QString &newText) {
    if(newText != text) {
        text = newText;
        init = false;
        image.fill(Qt::transparent);
        QPainter painter(&image);
        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.drawText(QRect(QPoint(0, 0), size), alignement, text);
        painter.end();
    }
}
void GlText::drawText(const QPoint &pos) {
    if(!init) {
        glEnable(GL_TEXTURE_2D);
        if(!texture) {
            glGenTextures(1, &texture);
            qDebug("%s", qPrintable(QString("[OPENGL] Création de la texture #%1 => %2 (%3 x %4)").arg(texture).arg(text).arg(size.width()).arg(size.height())));
        }
        glBindTexture  (GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, QGLWidget::convertToGLFormat(image).bits());
        glDisable(GL_TEXTURE_2D);
        init = true;
    }
    if(init) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        GlRect::drawRect(QRect(pos, size));
        glDisable(GL_TEXTURE_2D);
    }
}



void GlRect::setTexture(const QString &filename, const QSize &desiredSize) {
    if(filename != filenameOld) {
        init = false;
        image.load(filename);
        if((desiredSize.width() > 0) && (desiredSize.height() > 0))
            image = image.scaled(desiredSize, Qt::KeepAspectRatio);
        size = image.size();
        filenameOld = filename;
    }
}
void GlRect::drawTexture(const QString &filename, const QRectF &rect, qreal croppingMode) {
    setTexture(filename);
    drawTexture(rect, croppingMode);
}
void GlRect::drawTexture(const QRectF &rect, qreal croppingMode) {
    if(!init) {
        glEnable(GL_TEXTURE_2D);
        if(!texture) {
            glGenTextures(1, &texture);
            qDebug("%s", qPrintable(QString("[OPENGL] Création de la texture #%1 => %2 (%3 x %4)").arg(texture).arg(filenameOld).arg(size.width()).arg(size.height())));
        }
        glBindTexture  (GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, QGLWidget::convertToGLFormat(image).bits());
        glDisable(GL_TEXTURE_2D);
        init = true;
    }
    if(init) {
        QRectF targetRect = rect;
        QRectF textureRect = QRectF(0, 0, 1, 1);
        QRectF partialTextureRect = QRectF(0,0,-1,-1);

        if(croppingMode <= -3)
            textureRect = QRectF(QPointF(0, 0), targetRect.size());
        else if(croppingMode == -2) {
            textureRect = QRectF(QPointF(0, 0), size);
            targetRect.setSize(textureRect.size() / qMax(textureRect.width(), textureRect.height()) * qMax(rect.width(), rect.height()));
            qreal scaleDepassement = qMax(targetRect.width() / rect.width(), targetRect.height() / rect.height());
            if(scaleDepassement > 1)
                targetRect.setSize(targetRect.size() / scaleDepassement);
            targetRect = QRectF(QPointF(rect.x() + rect.width() / 2 - targetRect.width() / 2, rect.y() + rect.height() / 2 - targetRect.height() / 2), QSizeF(targetRect.width(), targetRect.height()));
        }
        else if(croppingMode == -1) {
            textureRect = QRectF(QPointF(0, 0), size);
            if((partialTextureRect.width() > 0) && (partialTextureRect.height() > 0))
                textureRect = QRectF(QPointF(partialTextureRect.x()     * size.width(), partialTextureRect.y()      * size.height()),
                                     QSizeF (partialTextureRect.width() * size.width(), partialTextureRect.height() * size.height()));
        }
        else if(croppingMode >= 0) {
            textureRect.setSize(targetRect.size() / qMax(targetRect.width(), targetRect.height()) * qMax(size.width(), size.height()));
            qreal scaleDepassement = qMax(textureRect.width() / size.width(), textureRect.height() / size.height());
            if(scaleDepassement > 1)
                textureRect.setSize(textureRect.size() / scaleDepassement);
            qreal panXMax = textureRect.width() - size.width(), panYMax = textureRect.height() - size.height();
            qreal panX = ((2*croppingMode)-1) * panXMax/2, panY = ((2*croppingMode)-1) * panYMax/2;
            textureRect = QRectF(QPointF(panX + size.width() / 2 - textureRect.width() / 2, panY + size.height() / 2 - textureRect.height() / 2), QSizeF(textureRect.width(), textureRect.height()));
        }
        textureRect = QRectF(QPointF(textureRect.x()     / size.width(), textureRect.y()      / size.height()),
                             QSizeF (textureRect.width() / size.width(), textureRect.height() / size.height()));

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        if(croppingMode == -4) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            croppingMode = -3;
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        GlRect::drawRect(targetRect, 0, textureRect);
        glDisable(GL_TEXTURE_2D);
    }
}

void GlRect::drawRoundedRect(const QRectF &rect, bool border, qreal precision) {
    if(border)  GlRect::drawRect(rect, -precision);
    else        GlRect::drawRect(rect,  precision);
}
void GlRect::drawRect(const QRectF &rect, qreal roundPrecision, const QRectF &texCoord) {
    if(roundPrecision != 0) {
        if(roundPrecision > 0) {
            qreal borderRadius = rect.height() / 2;
            glBegin(GL_POLYGON);
            glVertex2f    (rect.topLeft()    .x() + borderRadius, rect.topLeft()    .y());
            glVertex2f    (rect.topRight()   .x() - borderRadius, rect.topRight()   .y());
            for(qreal angle = 0 ; angle < M_PI ; angle += qAbs(roundPrecision))
                glVertex2f(rect.topRight()   .x() - borderRadius + borderRadius * qCos(M_PI/2 - angle),   rect.center().y() - borderRadius * qSin(M_PI/2 - angle));
            glVertex2f    (rect.bottomRight().x() - borderRadius, rect.bottomRight().y());
            glVertex2f    (rect.bottomLeft() .x() + borderRadius, rect.bottomLeft() .y());
            for(qreal angle = 0 ; angle < M_PI ; angle += qAbs(roundPrecision))
                glVertex2f(rect.bottomLeft() .x() + borderRadius + borderRadius * qCos(3*M_PI/2 - angle), rect.center().y() - borderRadius * qSin(3*M_PI/2 - angle));
            glEnd();
        }
        if(roundPrecision < 0) {
            qreal borderRadius = rect.height() / 2;
            glBegin(GL_LINE_LOOP);
            glVertex2f    (rect.topLeft()    .x() + borderRadius, rect.topLeft()    .y());
            glVertex2f    (rect.topRight()   .x() - borderRadius, rect.topRight()   .y());
            for(qreal angle = 0 ; angle < M_PI ; angle += qAbs(roundPrecision))
                glVertex2f(rect.topRight()   .x() - borderRadius + borderRadius * qCos(M_PI/2 - angle),   rect.center().y() - borderRadius * qSin(M_PI/2 - angle));
            glVertex2f    (rect.bottomRight().x() - borderRadius, rect.bottomRight().y());
            glVertex2f    (rect.bottomLeft() .x() + borderRadius, rect.bottomLeft() .y());
            for(qreal angle = 0 ; angle < M_PI ; angle += qAbs(roundPrecision))
                glVertex2f(rect.bottomLeft() .x() + borderRadius + borderRadius * qCos(3*M_PI/2 - angle), rect.center().y() - borderRadius * qSin(3*M_PI/2 - angle));
            glEnd();
        }
    }
    else {
        glBegin(GL_QUADS);
        glTexCoord2f(texCoord.bottomLeft() .x(), texCoord.bottomLeft() .y()); glVertex2f(rect.topLeft()    .x(), rect.topLeft()    .y());
        glTexCoord2f(texCoord.bottomRight().x(), texCoord.bottomRight().y()); glVertex2f(rect.topRight()   .x(), rect.topRight()   .y());
        glTexCoord2f(texCoord.topRight()   .x(), texCoord.topRight()   .y()); glVertex2f(rect.bottomRight().x(), rect.bottomRight().y());
        glTexCoord2f(texCoord.topLeft()    .x(), texCoord.topLeft()    .y()); glVertex2f(rect.bottomLeft() .x(), rect.bottomLeft() .y());
        glEnd();
    }
}




void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(optionV4.text);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}
