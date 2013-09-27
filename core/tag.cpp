#include "tag.h"

Tag::Tag(DocumentBase *_document, qint16 _documentVersion) :
    QObject(_document) {
    document          = _document;
    documentVersion   = _documentVersion;
    timelineWasInside = false;
    playerVideo       = 0;
    progression       = progressionDest = decounter = 0;
    isInProgress      = false;
    mouseHover        = false;
    mouseHoverPreview = false;
    breathing         = false;
    blinkTime         = 0;
    timelineFilesAction = 0;
    timeStart = timeEnd = 0;

    viewerTimeText          .setStyle(QSize( 70, Global::viewerTagHeight), Qt::AlignCenter,    Global::font);
    viewerDocumentText      .setStyle(QSize(500, Global::viewerTagHeight), Qt::AlignVCenter,   Global::font);
    timelineTimeStartText   .setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignRight | Qt::AlignVCenter, Global::fontSmall);
    timelineTimeEndText     .setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignLeft  | Qt::AlignVCenter, Global::fontSmall);
    timelineTimeDurationText.setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignCenter,  Global::fontSmall);
}

void Tag::create(TagType _type, qreal _timeStart, qreal _duration, bool debug) {
    if(document->function == DocumentFunctionRender) {
        setType(_type, _timeStart);
        Global::renders.insert(document->getMetadata("Rekall", "Document Name").toString(), this);
        playerVideo = new PlayerVideo();
        playerVideo->load(document->file.absoluteFilePath());
        //document->setMetadata("Rekall", "Media Offset", Global::aleaF(142, 142), documentVersion);
        document->setMetadata("Rekall", "Timeline thumbnail", "picture", documentVersion);
        connect(&document->renderActive, SIGNAL(triggered(bool)), SLOT(renderActiveChanged()));
        if(debug)
            _duration = Global::aleaF(140, 180);
    }
    else {
        if(debug) {
            qreal val = Global::alea(0, 100);
            if(val < 50)      setType(TagTypeContextualTime, _timeStart);
            else if(val < 75) setType(TagTypeContextualMilestone, _timeStart);
            else              setType(TagTypeGlobal, 0);
        }
        else
            setType(_type, _timeStart);

        if((debug) && (Global::aleaF(0, 1) > 50)) {
            if(document->type == DocumentTypeImage)
                document->setMetadata("Rekall", "Timeline thumbnail", "picture", documentVersion);
        }
        if((debug) && (Global::aleaF(0, 100) > 80)) {
            qreal tirage = Global::aleaF(0, 100);
            if(tirage > 66)       linkedRenders << "Captation 1";
            else if(tirage > 33)  linkedRenders << "Captation 2";
            else                  linkedRenders << "Captation 3";
        }
        if(debug)
            _duration = Global::aleaF(2, 20);
    }
    if(type == TagTypeContextualTime)
        setTimeEnd(timeStart + _duration);
}


void Tag::setType(TagType _type, qreal time) {
    type = _type;
    if(type == TagTypeContextualTime) {
        if((timeStart == 0) && (timeEnd == 0))  timeStart = time;
        else                                    timeStart = time - 5;
        timeEnd = timeStart + 10;
    }
    else
        timeStart = timeEnd = time;
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}

void Tag::setTimeStart(qreal _timeStart) {
    qreal mediaDuration = document->getMetadata("Rekall", "Media Duration").toDouble();
    qreal mediaOffset   = document->getMetadata("Rekall", "Media Offset").toDouble();
    if(mediaDuration <= 0) mediaDuration = timeEnd;
    timeStart = qBound(timeEnd - mediaDuration + mediaOffset, _timeStart, timeEnd);
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}
void Tag::setTimeEnd(qreal _timeEnd) {
    if(type == TagTypeContextualTime) {
        qreal mediaDuration = document->getMetadata("Rekall", "Media Duration").toDouble();
        qreal mediaOffset   = document->getMetadata("Rekall", "Media Offset").toDouble();
        if(mediaDuration <= 0) mediaDuration = 999999;
        timeEnd = qBound(timeStart, _timeEnd, timeStart + mediaDuration - mediaOffset);
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    }
    else
        timeEnd = timeStart;
}
void Tag::moveTo(qreal _val) {
    qreal _duration = getDuration();
    timeStart = qMax(0., _val);
    timeEnd   = timeStart + _duration;
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}



void Tag::fireEvents() {
    //Progression
    progressionDest = progress(Global::time);
    progression = progression + (progressionDest - progression) / Global::inertie;
    decounter = qMin(0., Global::time - timeStart);

    //Enter / Leave
    qreal progressionAbs = progressAbs(Global::time);
    if(((0. <= progressionAbs) && (progressionAbs <= 1.)) && (!timelineWasInside)) {
        timelineWasInside = true;
        Global::udp->send("127.0.0.1", 57120, "/transport", QList<QVariant>() << document->getMetadata("Rekall", "Document Name", documentVersion).toString() << timeStart << timeEnd << 1.);
        blinkTime = Global::tagBlinkTime;
        document->renderActive = document->renderActive.val();
    }
    else if(((0. > progressionAbs) || (progressionAbs > 1.)) && (timelineWasInside)) {
        timelineWasInside = false;
        Global::udp->send("127.0.0.1", 57120, "/transport", QList<QVariant>() << document->getMetadata("Rekall", "Document Name", documentVersion).toString() << timeStart << timeEnd << 0.);
        document->renderActive = document->renderActive.val();
    }
}
void Tag::renderActiveChanged() {
    if(document->function == DocumentFunctionRender) {
        qreal progressionAbs = progressAbs(Global::time);
        if(((0. <= progressionAbs) && (progressionAbs <= 1.)) && (document->renderActive)) {
            Global::video->load(this);
            playerVideo->setVolume(0.5);
        }
        else if(((0. > progressionAbs) || (progressionAbs > 1.)) || (!document->renderActive)) {
            Global::video->unload(this);
            playerVideo->setVolume(0);
        }
    }
}


const QRectF Tag::paintTimeline(bool before) {
    if(before) {
        timelinePos = timelinePos + (timelineDestPos - timelinePos) / Global::inertie;
        if(type == TagTypeGlobal)   timelineBoundingRect = QRectF(QPointF(Global::timelineGL->scroll.x()-Global::timelineNegativeHeaderWidth, 0), QSizeF(qMax(Global::timelineTagHeight, getDuration() * Global::timeUnit), Global::timelineTagHeight));
        else                        timelineBoundingRect = QRectF(QPointF(timeStart * Global::timeUnit, 0), QSizeF(qMax(Global::timelineTagHeight, getDuration() * Global::timeUnit), Global::timelineTagHeight));

        bool isLargeTag = (document->getMetadata("Rekall", "Timeline thumbnail").toString() == "picture");
        if(isLargeTag)
            timelineBoundingRect.setHeight(Global::timelineTagThumbHeight);
        if(timelineBoundingRect.width() < timelineBoundingRect.height())
            timelineBoundingRect.adjust(-(timelineBoundingRect.height() - timelineBoundingRect.width())/2, 0, (timelineBoundingRect.height() - timelineBoundingRect.width())/2, 0);

        QColor colorDestTmp = (Global::selectedTagHover == this)?(Global::colorSelection):(document->getColor());
        if(getDocument()->status == DocumentStatusWaiting)
            colorDestTmp.setAlphaF(0.1);
        if(getDocument()->status == DocumentStatusProcessing)
            colorDestTmp.setAlphaF(Global::breathingFast);

        if(!((colorDestTmp.red() == 0) && (colorDestTmp.green() == 0) && (colorDestTmp.blue() == 0)))
            colorDest = colorDestTmp;
        color.setRedF  (color.redF()   + (colorDest.redF()   - color.redF()  ) / Global::inertie);
        color.setGreenF(color.greenF() + (colorDest.greenF() - color.greenF()) / Global::inertie);
        color.setBlueF (color.blueF()  + (colorDest.blueF()  - color.blueF() ) / Global::inertie);
        color.setAlphaF(color.alphaF() + (colorDest.alphaF() - color.alphaF()) / Global::inertie);
        if(breathing)
            color = color.lighter(100 + (1-Global::breathing)*15);



        glPushMatrix();
        if(type == TagTypeGlobal)
            glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height());

        glTranslatef(qRound(timelinePos.x()), qRound(timelinePos.y()), 0);

        //Drawing
        if(Global::timelineGL->visibleRect.intersects(timelineBoundingRect.translated(timelinePos + QPointF(0, Global::timelineHeaderSize.height())))) {
            //Thumbnail strip
            if(isLargeTag) {
                //Thumb adapt
                if((document->type == DocumentTypeVideo) && (document->thumbnails.count()))
                    timelineBoundingRect.setHeight((Global::thumbsEach * Global::timeUnit) * document->thumbnails.first().size.height() / document->thumbnails.first().size.width());

                //Strip
                if(document->function == DocumentFunctionContextual) {
                    Global::timelineGL->qglColor(color);
                    GlRect::drawRect(timelineBoundingRect);
                }

                if((document->type == DocumentTypeVideo) && (document->thumbnails.count())) {
                    //Media offset
                    qreal mediaOffset    = document->getMetadata("Rekall", "Media Offset").toDouble();
                    qreal timeThumbStart = (mediaOffset / Global::thumbsEach)                    * Global::thumbsEach;
                    qreal timeThumbEnd   = ((mediaOffset + getDuration()) / Global::thumbsEach) * Global::thumbsEach;

                    Global::timelineGL->qglColor(Qt::white);
                    for(qreal timeThumbX = timeThumbStart ; timeThumbX < timeThumbEnd ; timeThumbX += Global::thumbsEach) {
                        QRectF thumbRect = QRectF(QPointF((timeThumbX-timeThumbStart) * Global::timeUnit, 0), QSizeF(Global::thumbsEach * Global::timeUnit, timelineBoundingRect.height())).translated(timelineBoundingRect.topLeft());
                        thumbRect.setRight(qMin(thumbRect.right(), timelineBoundingRect.right()));
                        document->thumbnails[qMin(qFloor(timeThumbX / Global::thumbsEach), document->thumbnails.count()-1)].drawTexture(thumbRect, 0);
                    }
                }
                else if(document->waveform.count()) {
                    qreal mediaDuration  = document->getMetadata("Rekall", "Media Duration").toDouble();
                    qreal mediaOffset    = document->getMetadata("Rekall", "Media Offset").toDouble() / mediaDuration;
                    qreal sampleMax      = getDuration() / mediaDuration;

                    glBegin(GL_LINES);
                    Global::timelineGL->qglColor(color);
                    for(qreal timeX = 0 ; timeX < timelineBoundingRect.width() ; timeX++) {
                        quint16 waveformIndex = qMin((int)((mediaOffset + timeX/timelineBoundingRect.width()*sampleMax) * document->waveform.count()), document->waveform.count()-1);
                        glVertex2f(timelineBoundingRect.left() + timeX, timelineBoundingRect.center().y() - document->waveform.at(waveformIndex).first  * document->waveform.normalisation * timelineBoundingRect.height()/2);
                        glVertex2f(timelineBoundingRect.left() + timeX, timelineBoundingRect.center().y() - document->waveform.at(waveformIndex).second * document->waveform.normalisation * timelineBoundingRect.height()/2 + 1);
                    }
                    glEnd();
                }
                else if(document->thumbnails.count()) {
                    Global::timelineGL->qglColor(Qt::white);
                    document->thumbnails.first().drawTexture(timelineBoundingRect, Global::thumbnailSlider);
                }
            }
            else {
                //Bar
                qreal shape = (document->type == DocumentTypeMarker)?(M_PI/2):(M_PI/4);
                Global::timelineGL->qglColor(color);
                if(isTagLastVersion(this))
                    GlRect::drawRoundedRect(timelineBoundingRect.adjusted(1, 1, -1, -1), false, shape);
                GlRect::drawRoundedRect(timelineBoundingRect.adjusted(1, 1, -1, -1), true, shape);
            }
            //Text
            if((Global::selectedTagHover == this) && (type != TagTypeGlobal)) {
                QPoint textPos;
                Global::timelineGL->qglColor(color);

                textPos = QPoint(timelineBoundingRect.left() - 2 - timelineTimeStartText.size.width(), 1 + timelineBoundingRect.center().y() - timelineTimeStartText.size.height()/2);
                timelineTimeStartText.drawText(Global::timeToString(timeStart), textPos);

                if(type == TagTypeContextualTime) {
                    textPos = QPoint(timelineBoundingRect.right() + 2, 1 + timelineBoundingRect.center().y() - timelineTimeEndText.size.height()/2);
                    timelineTimeEndText.drawText(Global::timeToString(timeEnd), textPos);

                    if(isTagLastVersion(this))
                        Global::timelineGL->qglColor(Qt::white);
                    textPos = QPoint(timelineBoundingRect.center().x() - timelineTimeDurationText.size.width()/2, 1 + timelineBoundingRect.center().y() - timelineTimeDurationText.size.height()/2);
                    timelineTimeDurationText.drawText(Global::timeToString(getDuration()), textPos);
                }
            }
        }


        glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height());


        //History tags
        if(historyTags.count()) {
            QColor colorAlpha = color;
            colorAlpha.setAlphaF(0.4);
            Global::timelineGL->qglColor(colorAlpha);
            //Anchors
            QPointF historyChordBegCtr = timelineBoundingRect.center();
            QPointF historyChordBegTop(timelineBoundingRect.center().x(), timelineBoundingRect.top()    + 1);
            QPointF historyChordBegBtm(timelineBoundingRect.center().x(), timelineBoundingRect.bottom() - 1);
            GLfloat historyChordPts[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
            glLineWidth(2);
            foreach(Tag *historyTag, historyTags) {
                if((historyTag == this) || (!historyTag))
                    continue;

                //Anchors
                QPointF historyChordEndCtr = historyTag->timelineBoundingRect.center() - timelinePos + historyTag->timelinePos;
                QPointF historyChordEndTop = QPointF(historyChordEndCtr.x(), historyTag->timelineBoundingRect.top()    + 1 - timelinePos.y() + historyTag->timelinePos.y());
                QPointF historyChordEndBtm = QPointF(historyChordEndCtr.x(), historyTag->timelineBoundingRect.bottom() - 1 - timelinePos.y() + historyTag->timelinePos.y());

                QPointF historyChordBeg, historyChordEnd;
                if(historyChordEndCtr.y() < historyChordBegCtr.y())    { historyChordBeg = historyChordBegTop;   historyChordEnd = historyChordEndBtm;   }
                else                                                  { historyChordBeg = historyChordBegBtm;   historyChordEnd = historyChordEndTop;   }
                historyChordPts[0][0] = historyChordBeg.x(); historyChordPts[0][1] = historyChordBeg.y();
                historyChordPts[1][0] = historyChordBeg.x(); historyChordPts[1][1] = historyChordBeg.y() + (historyChordEnd.y() - historyChordBeg.y()) * 0.66;
                historyChordPts[2][0] = historyChordEnd.x(); historyChordPts[2][1] = historyChordEnd.y() + (historyChordBeg.y() - historyChordEnd.y()) * 0.66;
                historyChordPts[3][0] = historyChordEnd.x(); historyChordPts[3][1] = historyChordEnd.y();
                glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &historyChordPts[0][0]);
                glEnable(GL_MAP1_VERTEX_3);
                glBegin(GL_LINE_STRIP);
                for(qreal t = 0 ; t <= 1.05; t += 0.05)
                    glEvalCoord1f(t);
                glEnd();
                glDisable(GL_MAP1_VERTEX_3);
            }
            glLineWidth(1);
        }


        //Linked tags
        if((Global::timelineGL->showLinkedTags > 0.01) && (linkedTags.count())) {
            QColor colorAlpha = color;
            colorAlpha.setAlphaF(0.4);
            Global::timelineGL->qglColor(colorAlpha);
            //Anchors
            QPointF linkedChordBegLeft (timelineBoundingRect.left(), timelineBoundingRect.center().y());
            GLfloat linkedChordPts[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
            glLineWidth(2);
            foreach(Tag *linkedTag, linkedTags) {
                if((linkedTag == this) || (!linkedTag))
                    continue;

                //Anchors
                QPointF linkedChordEndCtr = linkedTag->timelineBoundingRect.center() - timelinePos + linkedTag->timelinePos;
                QPointF linkedChordEndRight = QPointF(linkedTag->timelineBoundingRect.right() - timelinePos.x() + linkedTag->timelinePos.x(), linkedChordEndCtr.y());

                linkedChordPts[0][0] = linkedChordBegLeft .x(); linkedChordPts[0][1] = linkedChordBegLeft .y();
                linkedChordPts[1][0] = linkedChordBegLeft .x(); linkedChordPts[1][1] = linkedChordBegLeft .y() + (linkedChordEndRight.y() - linkedChordBegLeft.y()) * 0.66;
                linkedChordPts[2][0] = linkedChordEndRight.x(); linkedChordPts[2][1] = linkedChordEndRight.y() + (linkedChordBegLeft.y() - linkedChordEndRight.y()) * 0.66;
                linkedChordPts[3][0] = linkedChordEndRight.x(); linkedChordPts[3][1] = linkedChordEndRight.y();
                glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &linkedChordPts[0][0]);
                glEnable(GL_MAP1_VERTEX_3);
                glBegin(GL_LINE_STRIP);
                for(qreal t = 0 ; t <= (1.05 * Global::timelineGL->showLinkedTags); t += 0.05)
                    glEvalCoord1f(t);
                glEnd();
                glDisable(GL_MAP1_VERTEX_3);
            }
            glLineWidth(1);
        }


        //Linked renders
        if((Global::timelineGL->showLinkedRenders > 0.01) && (linkedRenders.count())) {
            QColor colorSourceAlpha = color;
            colorSourceAlpha.setAlphaF(0.4);

            Global::timelineGL->qglColor(colorSourceAlpha);
            glLineWidth(2);
            glBegin(GL_LINES);
            foreach(const QString &linkedRender, linkedRenders) {
                if(Global::renders.contains(linkedRender)) {
                    Tag *linkedRenderTag = (Tag*)Global::renders.value(linkedRender);
                    QRectF linkedRenderRect = linkedRenderTag->timelineBoundingRect.translated(-timelinePos + linkedRenderTag->timelinePos);
                    QPointF linkedRenderPoint;
                    qreal linkedRenderAnchor = 0;
                    if(linkedRenderRect.center().y() > timelineBoundingRect.center().y()) {
                        linkedRenderPoint  = QPointF(timelineBoundingRect.center().x(), Global::timelineGL->showLinkedRenders * linkedRenderRect.top());
                        linkedRenderAnchor = -Global::timelineTagHeight/2;
                    }
                    else {
                        linkedRenderPoint  = QPointF(timelineBoundingRect.center().x(), Global::timelineGL->showLinkedRenders * linkedRenderRect.bottom());
                        linkedRenderAnchor = Global::timelineTagHeight/2;
                    }
                    QColor colorDestAlpha = linkedRenderTag->color;
                    colorDestAlpha.setAlphaF(0.4 * Global::timelineGL->showLinkedRenders);


                    Global::timelineGL->qglColor(colorSourceAlpha);
                    glVertex2f(timelineBoundingRect.center().x(), timelineBoundingRect.center().y());
                    Global::timelineGL->qglColor(colorDestAlpha);
                    glVertex2f(linkedRenderPoint.x(), linkedRenderPoint.y() + linkedRenderAnchor);
                    glVertex2f(linkedRenderPoint.x(), linkedRenderPoint.y() + linkedRenderAnchor);
                    glVertex2f(linkedRenderPoint.x() + Global::timelineTagHeight/2, linkedRenderPoint.y());
                    glVertex2f(linkedRenderPoint.x(), linkedRenderPoint.y() + linkedRenderAnchor);
                    glVertex2f(linkedRenderPoint.x() - Global::timelineTagHeight/2, linkedRenderPoint.y());
                }
            }
            glEnd();
            glLineWidth(1);
        }

        //Snapping
        if((Global::selectedTagHoverSnapped >= 0) && (Global::selectedTagHoverSnap == this) && (Global::selectedTag)) {
            Tag *snappedTag = (Tag*)Global::selectedTag;
            qint16 pos = Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth + Global::timeUnit * Global::selectedTagHoverSnapped - timelinePos.x();

            Global::timelineGL->qglColor(Global::colorAlternateStrong);
            glLineStipple(5, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex2f(pos, timelineBoundingRect.center().y());
            glVertex2f(pos, snappedTag->timelineBoundingRect.translated(snappedTag->timelinePos).translated(-timelinePos).center().y());
            glEnd();
            glDisable(GL_LINE_STIPPLE);
        }

        glScissor(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth, 0, Global::timelineGL->width() - Global::timelineHeaderSize.width() - Global::timelineNegativeHeaderWidth, Global::timelineGL->height());
        glPopMatrix();
    }
    else if(mouseHoverPreview) {
        if((getDocument()->type == DocumentTypeImage) && (getDocument()->thumbnails.count())) {
            Global::timelineGL->qglColor(Global::colorAlternateStrong);
            GlRect::drawRect(Global::timelineGL->visibleRect);
            Global::timelineGL->qglColor(Qt::white);
            getDocument()->thumbnails.first().drawTexture(Global::timelineGL->visibleRect);
        }
        Global::mainWindow->displayDocumentName(QString("%1 (%2)").arg(getDocument()->getMetadata("Rekall", "Document Name").toString()).arg(getDocument()->getMetadata("Rekall", "Document Folder").toString()));
        Global::mainWindow->displayPixmap(getDocument()->getThumbnail(documentVersion));
        Global::mainWindow->displayGps(getDocument()->getGps());
    }
    return timelineBoundingRect.translated(timelinePos);
}

const QRectF Tag::paintViewer(quint16 tagIndex) {
    viewerPos = viewerPos + (viewerDestPos - viewerPos) / Global::inertie;
    viewerBoundingRect = QRectF(QPointF(0, 0), QSizeF(Global::viewerGL->width(), Global::viewerTagHeight));
    QRectF thumbnailRect;
    bool hasThumbnail = false;
    if(getDocument()->thumbnails.count()) {
        thumbnailRect = QRectF(QPointF(75, 11), QSizeF(105, 70));
        hasThumbnail = true;
        viewerBoundingRect.setHeight(thumbnailRect.height() + 2*thumbnailRect.top());
    }


    if(Global::viewerGL->visibleRect.intersects(viewerBoundingRect.translated(viewerPos))) {
        glPushMatrix();
        glTranslatef(qRound(viewerPos.x()), qRound(viewerPos.y()), 0);

        //Selection
        if(Global::selectedTagHover == this) {
            Global::viewerGL->qglColor(Global::colorSelection);
            GlRect::drawRect(viewerBoundingRect);
        }

        //Alternative background
        if(tagIndex % 2) {
            Global::viewerGL->qglColor(Global::colorAlternate);
            GlRect::drawRect(viewerBoundingRect);
        }

        //Progression
        isInProgress = false;
        QRectF progressionRect(viewerBoundingRect.topLeft(), QSizeF(viewerBoundingRect.width(), 5));
        if((type == TagTypeContextualMilestone) && (blinkTime)) {
            isInProgress = true;
            blinkTime = qMax(0., blinkTime-20);
            if(qFloor(blinkTime / 250) % 2) {
                Global::viewerGL->qglColor(Global::colorProgression);
                GlRect::drawRect(progressionRect);
            }
        }
        else if((0.001 < progression) && (progression < 0.999)) {
            isInProgress = true;
            Global::viewerGL->qglColor(Global::colorProgression);
            GlRect::drawRect(QRectF(progressionRect.topLeft(), QSizeF(progressionRect.width() * progression, progressionRect.height())));
        }

        //Bar
        QColor barColor = color;
        if((decounter == 0) && (!isInProgress) && (Global::timerPlay))  barColor.setAlphaF(0.2);
        else                                                            barColor.setAlphaF(1.0);


        Global::viewerGL->qglColor(barColor);
        if(hasThumbnail)
            GlRect::drawRect(thumbnailRect.adjusted(-5, -5, 5, 5));
        else {
            qreal shape = (document->type == DocumentTypeMarker)?(M_PI/2):(M_PI/4);
            if(isTagLastVersion(this))
                GlRect::drawRoundedRect(viewerBoundingRect.adjusted(7, 7, -75, -7).translated(QPointF(60, 0)), false, shape);
            GlRect::drawRoundedRect(viewerBoundingRect.adjusted(7, 7, -75, -7).translated(QPointF(60, 0)), true, shape);
        }


        //Décompte
        if((Global::timerPlay) && (-5 <= decounter) && (decounter < 0)) {
            Global::viewerGL->qglColor(Global::colorText);
            qreal angleMax = 2*M_PI * -decounter/5, cote = (Global::viewerTagHeight / 2) * 0.6;
            QRect rect(QPoint(0, 0), QSize(70, viewerBoundingRect.height()));
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(rect.center().x(), rect.center().y());
            for(qreal angle = 0 ; angle < angleMax ; angle += 0.1)
                glVertex2f(rect.center().x() + cote * qCos(-M_PI/2 + angle), rect.center().y() + cote * qSin(-M_PI/2 + angle));
            glEnd();
        }
        //Temps
        else if((decounter < 0) || (!Global::timerPlay)) {
            Global::viewerGL->qglColor(Global::colorText);
            viewerTimeText.drawText(Global::timeToString(timeStart));
        }


        //Thumb
        QPoint textePos(75, 0);
        if(hasThumbnail) {
            Global::viewerGL->qglColor(QColor(255, 255, 255, barColor.alpha()));
            getDocument()->thumbnails.first().drawTexture(thumbnailRect, Global::breathingPics);
            textePos = thumbnailRect.topRight().toPoint() + QPoint(10, -10);
        }

        //Texte
        if((Global::selectedTagHover == this) || ((isTagLastVersion(this) && (!hasThumbnail))))  Global::viewerGL->qglColor(Qt::black);
        else                                                                                     Global::viewerGL->qglColor(barColor);
        QString texte = document->getMetadata("Rekall", "Document Name", documentVersion).toString();
        if(type == TagTypeContextualTime)
            texte += QString(" (%1)").arg(Global::timeToString(getDuration()));
        viewerDocumentText.drawText(texte, textePos);

        glPopMatrix();
    }
    return viewerBoundingRect.translated(viewerPos);
}

bool Tag::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool dbl, bool, bool action, bool) {
    if(timelineContains(pos)) {
        if(action) {
            mouseHover = true;
            if((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
                mouseHoverPreview = true;
            Global::selectedTagHover = this;
        }
        Global::selectedTagHoverSnap = this;
    }
    else if(action)
        mouseHover = mouseHoverPreview = false;

    if((action) && (mouseHover) && (((e->button() & Qt::LeftButton) == Qt::LeftButton) || ((e->button() & Qt::RightButton) == Qt::RightButton))) {
        if(Global::selectedTag != this) {
            Global::selectedTag = this;
            Global::selectedTagStartDrag = timelineProgress(pos) * getDuration();
            if((e->button() & Qt::LeftButton) == Qt::LeftButton) {
                if((timelineProgress(pos) < 0.1) && (type == TagTypeContextualTime))        Global::selectedTagMode = TagSelectionStart;
                else if((timelineProgress(pos) > 0.9) && (type == TagTypeContextualTime))   Global::selectedTagMode = TagSelectionEnd;
                else                                                                        Global::selectedTagMode = TagSelectionMove;
            }
            Global::viewerGL->ensureVisible(viewerPos);
        }
        if(document->chutierItem)
            Global::chutier->setCurrentItem(document->chutierItem);
    }
    if((action) && (dbl) && (mouseHover))
        Global::currentProject->askDisplayMetaData();

    return mouseHover;
}
bool Tag::mouseViewer(const QPointF &pos, QMouseEvent *e, bool dbl, bool, bool, bool) {
    if(viewerContains(pos)) {
        mouseHover = true;
        if((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
            mouseHoverPreview = true;
    }
    else
        mouseHover = mouseHoverPreview = false;
    if((mouseHover) && ((e->button() & Qt::LeftButton) == Qt::LeftButton)) {
        Global::selectedTag = this;
        Global::selectedTagMode = TagSelectionMove;
        Global::timelineGL->ensureVisible(timelinePos);
        if(document->chutierItem)
            Global::chutier->setCurrentItem(document->chutierItem);
    }
    if((dbl) && (mouseHover))
        Global::currentProject->askDisplayMetaData();
    if(mouseHover)
        Global::selectedTagHover = this;
    return mouseHover;
}

bool Tag::contains(qreal time) const {
    return ((timeStart <= time) && (time <= (timeStart + qMax(1., getDuration()))));
}
qreal Tag::progress(qreal time) const {
    return qBound(0., progressAbs(time), 1.);
}
qreal Tag::progressAbs(qreal time) const {
    return (time - timeStart) / qMax(1., getDuration());
}
qreal Tag::getDuration() const {
    return timeEnd - timeStart;
}

void Tag::snapTime(qreal *time) {
    if(qAbs(*time-timeStart) < 1) {
        *time = timeStart;
        Global::selectedTagHoverSnapped = *time;
    }
    else if(qAbs(*time-timeEnd) < 1) {
        *time = timeEnd;
        Global::selectedTagHoverSnapped = *time;
    }
}


bool Tag::isAcceptableWithFilters() {
    return (   Global::tagFilterCriteria->displayLinked) ||
            ((!Global::tagFilterCriteria->displayLinked) && (isTagLastVersion(this)));
}
bool Tag::isAcceptableWithSortFilters() {
    return (isAcceptableWithFilters()) && (getDocument()->isAcceptableWithSortFilters(documentVersion));
}
bool Tag::isAcceptableWithColorFilters() {
    return (isAcceptableWithFilters()) && (getDocument()->isAcceptableWithColorFilters(documentVersion));
}
bool Tag::isAcceptableWithClusterFilters() {
    return (isAcceptableWithFilters()) && (getDocument()->isAcceptableWithClusterFilters(documentVersion));
}
const QString Tag::getAcceptableWithClusterFilters() {
    return getDocument()->getAcceptableWithClusterFilters(documentVersion);
}


bool Tag::sortCriteria(Tag *first, Tag *second) {
    return Tag::getCriteriaColor(first) < Tag::getCriteriaColor(second);
}

const QString Tag::getCriteriaSort(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaSort(tag->documentVersion);
}
const QString Tag::getCriteriaColor(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaColor(tag->documentVersion);
}
const QString Tag::getCriteriaCluster(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaCluster(tag->documentVersion);
}
const MetadataElement Tag::getCriteriaSortRaw(Tag *tag) {
    return tag->document->getCriteriaSortRaw(tag->documentVersion);
}

const QString Tag::getCriteriaSortFormated(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaSortFormated(tag->documentVersion);
}
const QString Tag::getCriteriaColorFormated(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaColorFormated(tag->documentVersion);
}
const QString Tag::getCriteriaClusterFormated(Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaClusterFormated(tag->documentVersion);
}

bool Tag::isTagLastVersion(Tag *tag) {
    if(tag) return (tag->documentVersion < 0) || (tag->documentVersion == tag->document->getMetadataCountM());
    else    return false;
}



bool Tag::sortViewer(Tag *first, Tag *second) {
    if((!first) || (!second))
        return false;
    if((first->progressionDest == second->progressionDest) || (((0. < first->progressionDest) && (first->progressionDest < 1.)) && ((0. < second->progressionDest) && (second->progressionDest < 1.))))
        return first->timeStart < second->timeStart;
    else
        return first->progressionDest > second->progressionDest;
}
bool Tag::sortEvents(Tag *first, Tag *second) {
    if((!first) || (!second))
        return false;
    return first->timeStart < second->timeStart;
}


bool Tag::timelineContains(const QPointF &pos) {
    return (timelineBoundingRect.translated(timelinePos).translated(0, Global::timelineHeaderSize.height()).contains(pos));
}
bool Tag::viewerContains(const QPointF &pos) {
    return (viewerBoundingRect.translated(viewerPos).contains(pos));
}

qreal Tag::timelineProgress(const QPointF &pos) {
    QRectF rect = timelineBoundingRect.translated(timelinePos);
    return qBound(0., (pos.x() - rect.x()) / rect.width(), 1.);
}
qreal Tag::viewerProgress(const QPointF &pos) {
    QRectF rect = viewerBoundingRect.translated(viewerPos);
    return qBound(0., (pos.y() - rect.y()) / rect.height(), 1.);
}



QDomElement Tag::serialize(QDomDocument &xmlDoc) {
    QDomElement xmlData = xmlDoc.createElement("tag");
    xmlData.setAttribute("timeStart",       timeStart);
    xmlData.setAttribute("timeEnd",         timeEnd);
    xmlData.setAttribute("type",            type);
    xmlData.setAttribute("documentVersion", documentVersion);
    if(linkedTags.count()) {
        QDomElement linkedTagsXml = xmlDoc.createElement("linkedTags");
        foreach(Tag *linkedTag, linkedTags) {
            QDomElement linkedTagXml = xmlDoc.createElement("linkedTag");
            linkedTagXml.setAttribute("TODO", "TODO");
            linkedTagsXml.appendChild(linkedTagXml);
        }
        xmlData.appendChild(linkedTagsXml);
    }
    return xmlData;
}
void Tag::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
