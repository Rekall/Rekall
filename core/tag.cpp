#include "tag.h"

Tag::Tag(DocumentBase *_document, qint16 _documentVersion) :
    QObject(_document) {
    document          = _document;
    documentVersion   = _documentVersion;
    timelineWasInside = false;
    player       = 0;
    progression       = progressionDest = decounter = 0;
    isInProgress      = false;
    breathing         = false;
    viewerFirstPos = timelineFirstPos = true;
    viewerFirstPosVisible = timelineFirstPosVisible = false;
    blinkTime         = 0;
    timelineFilesAction = 0;
    timeStart = timeEnd = 0;
    tagScale = 0;
    tagDestScale = 1;

    viewerTimeText          .setStyle(QSize( 70, Global::viewerTagHeight), Qt::AlignCenter,    Global::font);
    viewerDocumentText      .setStyle(QSize(500, Global::viewerTagHeight), Qt::AlignVCenter,   Global::font);
    timelineTimeStartText   .setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignRight | Qt::AlignVCenter, Global::fontSmall);
    timelineTimeEndText     .setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignLeft  | Qt::AlignVCenter, Global::fontSmall);
    timelineTimeDurationText.setStyle(QSize( 70, Global::timelineTagHeightDest), Qt::AlignCenter,  Global::fontSmall);
}

void Tag::create(TagType _type, qreal _timeStart, qreal _duration, bool debug) {
    if(document->getFunction() == DocumentFunctionRender) {
        setType(_type, _timeStart);
        Global::renders.insert(document->getName(documentVersion), this);
        player = new PlayerVideo();
        player->load(this, ((document->getType() == DocumentTypeVideo) || (document->getType() == DocumentTypeImage)), document->file.absoluteFilePath());
        if(debug)
            _duration = Global::aleaF(50, 180);
    }
    else {
        if(debug) {
            qreal val = Global::alea(0, 100);
            if(val < 50)      setType(TagTypeContextualTime,     _timeStart);
            else if(val < 80) setType(TagTypeContextualMilestone, _timeStart);
            else              setType(TagTypeGlobal, 0);
        }
        else
            setType(_type, _timeStart);

        if((debug) && (Global::aleaF(0, 100) > 80)) {
            qreal tirage = Global::aleaF(0, 100);
            if(tirage > 66)       linkedRenders << "Captation 1";
            else if(tirage > 33)  linkedRenders << "Captation 2";
            else                  linkedRenders << "Captation 3";
        }
        if(debug) {
            qreal tirage = Global::aleaF(0, 100);
            if(tirage > 50)       _duration = Global::aleaF( 2,  5);
            else if(tirage > 10)  _duration = Global::aleaF( 5, 12);
            else                  _duration = Global::aleaF(12, 25);
        }
    }
    if(getType() == TagTypeContextualTime)
        setTimeEnd(timeStart + _duration);
}


void Tag::setType(TagType _type, qreal time) {
    type = _type;
    if(getType() == TagTypeContextualTime) {
        if((timeStart == 0) && (timeEnd == 0))  timeStart = time;
        else                                    timeStart = time - 5;
        timeEnd = timeStart + 10;
    }
    else
        timeStart = timeEnd = time;
    //Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}

void Tag::setTimeStart(qreal _timeStart) {
    qreal mediaDuration = document->getMediaDuration(documentVersion);
    qreal mediaOffset   = document->getMediaOffset(documentVersion);

    if(mediaDuration <= 0) mediaDuration = timeEnd;
    timeStart = qBound(timeEnd - mediaDuration + mediaOffset, _timeStart, timeEnd);
    //Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}
void Tag::setTimeEnd(qreal _timeEnd) {
    if(getType() == TagTypeContextualTime) {
        qreal mediaDuration = document->getMediaDuration(documentVersion);
        qreal mediaOffset   = document->getMediaOffset(documentVersion);
        if(mediaDuration <= 0) mediaDuration = 999999;
        timeEnd = qBound(timeStart, _timeEnd, timeStart + mediaDuration - mediaOffset);
        //Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
    }
    else
        timeEnd = timeStart;
}
void Tag::moveTo(qreal _val) {
    qreal _duration = getDuration();
    timeStart = qMax(0., _val);
    timeEnd   = timeStart + _duration;
    //Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
}






void Tag::fireEvents() {
    //Progression
    progressionDest = progress(Global::time);
    progression = progression + (progressionDest - progression) / Global::inertie;
    decounter = qMin(0., Global::time - getTimeStart());

    //Enter / Leave
    qreal progressionAbs = progressAbs(Global::time);
    qint16 oscValue = -1;
    if(((0. <= progressionAbs) && (progressionAbs <= 1.)) && (!timelineWasInside)) {
        timelineWasInside = true;
        blinkTime = Global::tagBlinkTime;
        if(document->getFunction() == DocumentFunctionRender) {
            if(!player) {
                player = new PlayerVideo();
                player->load(this, ((document->getType() == DocumentTypeVideo) || (document->getType() == DocumentTypeImage)), document->file.absoluteFilePath());
            }
            Global::video->load(this);
        }
        if(getType() != TagTypeGlobal)
            oscValue = 1;
    }
    else if(((0. > progressionAbs) || (progressionAbs > 1.)) && (timelineWasInside)) {
        timelineWasInside = false;
        if(document->getFunction() == DocumentFunctionRender)
            Global::video->unload(this);
        if(getType() != TagTypeGlobal)
            oscValue = 0;
    }
    if(oscValue >= 0)
        Global::udp->send("127.0.0.1", 57120, "/rekall", QList<QVariant>() << document->getTypeStr(documentVersion) << document->getAuthor(documentVersion) << document->getName(documentVersion) << getTimeStart() << getTimeEnd() << oscValue << document->baseColor.redF() << document->baseColor.greenF() << document->baseColor.blueF() << document->baseColor.alphaF());
}

const QRectF Tag::paintTimeline(bool before) {
    if(before) {
        tagScale = tagScale + (tagDestScale - tagScale) / Global::inertie;
        timelinePos = timelinePos + (timelineDestPos - timelinePos) / Global::inertie;


        if((Global::tagHorizontalCriteria->isTimeline()) && (getType() == TagTypeGlobal))
            timelineBoundingRect = QRectF(QPointF(Global::timelineGL->scroll.x()-Global::timelineGlobalDocsWidth, 0), QSizeF(qMax(Global::timelineTagHeight, getDuration(true) * Global::timeUnit), Global::timelineTagHeight));
        else {
            qreal pos   = Global::tagHorizontalCriteria->getCriteriaFormatedReal(getCriteriaHorizontal(this), getTimeStart());
            qreal width = Global::tagHorizontalCriteria->getCriteriaFormatedRealDuration(getDuration(true));
            timelineBoundingRect = QRectF(QPointF(pos * Global::timeUnit, 0), QSizeF(qMax(Global::timelineTagHeight, width * Global::timeUnit), Global::timelineTagHeight));
        }

        bool isLargeTag = (document->getFunction() == DocumentFunctionRender) && (Global::tagHorizontalCriteria->isTimeline());
        if(isLargeTag)
            timelineBoundingRect.setHeight(Global::timelineTagThumbHeight);
        if(timelineBoundingRect.width() < timelineBoundingRect.height())
            timelineBoundingRect.adjust(-(timelineBoundingRect.height() - timelineBoundingRect.width())/2, 0, (timelineBoundingRect.height() - timelineBoundingRect.width())/2, 0);

        QColor colorDestTmp = (Global::selectedTag == this)?(Global::colorTimeline):(document->baseColor);
        if(document->status == DocumentStatusWaiting)
            colorDestTmp.setAlphaF(0.1);
        if((document->status == DocumentStatusProcessing) || (Global::selectedTag == this))
            colorDestTmp.setAlphaF(Global::breathingFast);
        if(!Global::tagHorizontalCriteria->isTimeline()) {
            if((Global::timerPlay) && !((0.001 < progression) && (progression < 0.999))) colorDestTmp.setAlphaF(0.2);
            else                                                                         colorDestTmp.setAlphaF(1.0);
        }

        if(!((colorDestTmp.red() == 0) && (colorDestTmp.green() == 0) && (colorDestTmp.blue() == 0)))
            colorDest = colorDestTmp;
        realTimeColor.setRedF  (realTimeColor.redF()   + (colorDest.redF()   - realTimeColor.redF()  ) / Global::inertie);
        realTimeColor.setGreenF(realTimeColor.greenF() + (colorDest.greenF() - realTimeColor.greenF()) / Global::inertie);
        realTimeColor.setBlueF (realTimeColor.blueF()  + (colorDest.blueF()  - realTimeColor.blueF() ) / Global::inertie);
        realTimeColor.setAlphaF(realTimeColor.alphaF() + (colorDest.alphaF() - realTimeColor.alphaF()) / Global::inertie);
        if(breathing)
            realTimeColor = realTimeColor.lighter(100 + (1-Global::breathing)*15);

        glPushMatrix();
        glTranslatef(qRound(timelinePos.x()), qRound(timelinePos.y()), 0);

        glTranslatef(qRound(timelineBoundingRect.center().x()), qRound(timelineBoundingRect.center().y()), 0);
        glScalef(tagScale, tagScale, 1);
        glTranslatef(-qRound(timelineBoundingRect.center().x()), -qRound(timelineBoundingRect.center().y()), 0);

        //Drawing
        if(Global::timelineGL->visibleRect.intersects(timelineBoundingRect.translated(timelinePos + QPointF(0, Global::timelineHeaderSize.height())))) {
            //Thumbnail strip
            if(isLargeTag) {
                //Thumb adapt
                if((document->getType() == DocumentTypeVideo) && (document->thumbnails.count()))
                    timelineBoundingRect.setHeight((Global::thumbsEach * Global::timeUnit) * document->thumbnails.first().size.height() / document->thumbnails.first().size.width());

                //Strip
                if((document->getType() == DocumentTypeVideo) && (document->thumbnails.count())) {
                    //Media offset
                    qreal mediaOffset    = document->getMediaOffset(documentVersion);
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
                    qreal mediaDuration  = document->getMediaDuration(documentVersion);
                    qreal mediaOffset    = document->getMediaOffset(documentVersion) / mediaDuration;
                    qreal sampleMax      = getDuration() / mediaDuration;

                    glBegin(GL_LINES);
                    Global::timelineGL->qglColor(realTimeColor);
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
                else {
                    Global::timelineGL->qglColor(realTimeColor);
                    GlRect::drawRect(timelineBoundingRect);
                }
            }
            else if(document->getType() == DocumentTypeMarker) {
                //Bar
                Global::timelineGL->qglColor(realTimeColor);
                if((getType() == TagTypeContextualMilestone) || (getType() == TagTypeGlobal)) {
                    timelineBoundingRect.setWidth(3);
                    GlRect::drawRect(timelineBoundingRect);
                }
                else {
                    GlRect::drawRect(QRectF(timelineBoundingRect.topLeft(),  QSizeF( 3, timelineBoundingRect.height())));
                    GlRect::drawRect(QRectF(timelineBoundingRect.topRight(), QSizeF(-3, timelineBoundingRect.height())));
                    GlRect::drawRect(QRectF(timelineBoundingRect.topLeft() + QPointF(0, timelineBoundingRect.height() / 2 - 1), QSizeF(timelineBoundingRect.width(), 2)));
                }
            }
            else {
                //Bar
                Global::timelineGL->qglColor(realTimeColor);
                if(isTagLastVersion(this))
                    GlRect::drawRoundedRect(timelineBoundingRect.adjusted(1, 1, -1, -1), false, M_PI/4);
                GlRect::drawRoundedRect(timelineBoundingRect.adjusted(1, 1, -1, -1), true, M_PI/4);
            }

            //Text
            if((Global::selectedTag == this) && (getType() != TagTypeGlobal)) {
                QPoint textPos;
                Global::timelineGL->qglColor(realTimeColor);

                textPos = QPoint(timelineBoundingRect.left() - 2 - timelineTimeStartText.size.width(), 1 + timelineBoundingRect.center().y() - timelineTimeStartText.size.height()/2);
                timelineTimeStartText.drawText(Sorting::timeToString(getTimeStart()), textPos);

                if(getType() == TagTypeContextualTime) {
                    textPos = QPoint(timelineBoundingRect.right() + 2, 1 + timelineBoundingRect.center().y() - timelineTimeEndText.size.height()/2);
                    timelineTimeEndText.drawText(Sorting::timeToString(getTimeEnd()), textPos);

                    if(isTagLastVersion(this))
                        Global::timelineGL->qglColor(Qt::white);
                    textPos = QPoint(timelineBoundingRect.center().x() - timelineTimeDurationText.size.width()/2, 1 + timelineBoundingRect.center().y() - timelineTimeDurationText.size.height()/2);
                    timelineTimeDurationText.drawText(Sorting::timeToString(getDuration()), textPos);
                }
            }
        }


        //History tags
        if(historyTags.count()) {
            QColor colorAlpha = realTimeColor;

            //Anchors
            QPointF historyChordBegCtr = timelineBoundingRect.center();
            QPointF historyChordBegTop(timelineBoundingRect.center().x(), timelineBoundingRect.top()    + 1);
            QPointF historyChordBegBtm(timelineBoundingRect.center().x(), timelineBoundingRect.bottom() - 1);
            GLfloat historyChordPts[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
            glLineWidth(2);
            foreach(Tag *historyTag, historyTags) {
                if((historyTag == this) || (!historyTag) || (!historyTag->isAcceptableWithSortFilters(true)))
                    continue;

                //Color
                if(((historyTag->getType() == TagTypeGlobal) && (getType() != TagTypeGlobal)) || ((getType() == TagTypeGlobal) && (historyTag->getType() != TagTypeGlobal)))    colorAlpha.setAlphaF(0.1);
                else                                                                                                                                        colorAlpha.setAlphaF(0.4);
                Global::timelineGL->qglColor(colorAlpha);

                //Anchors
                QPointF historyChordEndCtr = historyTag->timelineBoundingRect.center() - timelinePos + historyTag->timelinePos;
                QPointF historyChordEndTop = QPointF(historyChordEndCtr.x(), historyTag->timelineBoundingRect.top()    + 1 - timelinePos.y() + historyTag->timelinePos.y());
                QPointF historyChordEndBtm = QPointF(historyChordEndCtr.x(), historyTag->timelineBoundingRect.bottom() - 1 - timelinePos.y() + historyTag->timelinePos.y());

                QPointF historyChordBeg = historyChordBegCtr, historyChordEnd = historyChordEndCtr;
                if(     historyChordEndCtr.y() < historyChordBegCtr.y())   { historyChordBeg = historyChordBegTop;   historyChordEnd = historyChordEndBtm;   }
                else if(historyChordEndCtr.y() > historyChordBegCtr.y())   { historyChordBeg = historyChordBegBtm;   historyChordEnd = historyChordEndTop;   }
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

        //Hash tags
        if(hashTags.count()) {
            QColor colorAlpha = realTimeColor;

            //Anchors
            QPointF hashChordBegCtr = timelineBoundingRect.center();
            QPointF hashChordBegTop(timelineBoundingRect.center().x(), timelineBoundingRect.top()    + 1);
            QPointF hashChordBegBtm(timelineBoundingRect.center().x(), timelineBoundingRect.bottom() - 1);
            GLfloat hashChordPts[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
            glLineWidth(1);
            glLineStipple(5, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            foreach(Tag *hashTag, hashTags) {
                if((hashTag == this) || (!hashTag) || (!hashTag->isAcceptableWithSortFilters(true)))
                    continue;

                //Color
                if(((hashTag->getType() == TagTypeGlobal) && (getType() != TagTypeGlobal)) || ((getType() == TagTypeGlobal) && (hashTag->getType() != TagTypeGlobal)))
                    colorAlpha.setAlphaF(0.2);
                else
                    colorAlpha.setAlphaF(0.4);
                Global::timelineGL->qglColor(colorAlpha);


                //Anchors
                QPointF hashChordEndCtr = hashTag->timelineBoundingRect.center() - timelinePos + hashTag->timelinePos;
                //QPointF hashChordEndTop = QPointF(hashChordEndCtr.x(), hashTag->timelineBoundingRect.top()    + 1 - timelinePos.y() + hashTag->timelinePos.y());
                //QPointF hashChordEndBtm = QPointF(hashChordEndCtr.x(), hashTag->timelineBoundingRect.bottom() - 1 - timelinePos.y() + hashTag->timelinePos.y());

                QPointF hashChordBeg = hashChordBegCtr, hashChordEnd = hashChordEndCtr;
                /*
                if(hashChordEndCtr.y() == hashChordBegCtr.y())      { hashChordBeg = hashChordBegCtr;   hashChordEnd = hashChordEndCtr;   }
                else if(hashChordEndCtr.y() < hashChordBegCtr.y())  { hashChordBeg = hashChordBegTop;   hashChordEnd = hashChordEndBtm;   }
                else                                                { hashChordBeg = hashChordBegBtm;   hashChordEnd = hashChordEndTop;   }
                */
                hashChordPts[0][0] = hashChordBeg.x(); hashChordPts[0][1] = hashChordBeg.y();
                hashChordPts[1][0] = hashChordBeg.x(); hashChordPts[1][1] = hashChordBeg.y() + (hashChordEnd.y() - hashChordBeg.y()) * 0.66;
                hashChordPts[2][0] = hashChordEnd.x(); hashChordPts[2][1] = hashChordEnd.y() + (hashChordBeg.y() - hashChordEnd.y()) * 0.66;
                hashChordPts[3][0] = hashChordEnd.x(); hashChordPts[3][1] = hashChordEnd.y();
                glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &hashChordPts[0][0]);
                glEnable(GL_MAP1_VERTEX_3);
                glBegin(GL_LINE_STRIP);
                for(qreal t = 0 ; t <= 1.05; t += 0.05)
                    glEvalCoord1f(t);
                glEnd();
                glDisable(GL_MAP1_VERTEX_3);
            }
            glLineWidth(1);
            glDisable(GL_LINE_STIPPLE);
        }


        //Linked tags
        if((Global::timelineGL->showLinkedTags > 0.01) && (linkedTags.count())) {
            QColor colorAlpha = realTimeColor;
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
            QColor colorSourceAlpha = realTimeColor;
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
                    QColor colorDestAlpha = linkedRenderTag->realTimeColor;
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
        if((Global::selectedTagHoverSnapped >= 0) && (Global::selectedTagHover == this) && (Global::selectedTagInAction)) {
            Tag *snappedTag = (Tag*)Global::selectedTagInAction;
            qint16 pos = Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth + Global::timeUnit * Global::selectedTagHoverSnapped - timelinePos.x();

            Global::timelineGL->qglColor(Global::colorAlternateStrong);
            glLineStipple(5, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex2f(pos, timelineBoundingRect.center().y());
            glVertex2f(pos, snappedTag->timelineBoundingRect.translated(snappedTag->timelinePos).translated(-timelinePos).center().y());
            glEnd();
            glDisable(GL_LINE_STIPPLE);
        }
        glPopMatrix();
    }

    if(timelineFirstPosVisible) {
        timelineFirstPosVisible = false;
        Global::timelineGL->ensureVisible(timelineBoundingRect.translated(timelinePos).topLeft());
    }
    return timelineBoundingRect.translated(timelinePos);
}

const QRectF Tag::paintViewer(quint16 tagIndex) {
    viewerPos = viewerPos + (viewerDestPos - viewerPos) / Global::inertie;
    viewerBoundingRect = QRectF(QPointF(0, 0), QSizeF(Global::viewerGL->width(), Global::viewerTagHeight));
    QRectF thumbnailRect;
    bool hasThumbnail = false;
    if(document->thumbnails.count()) {
        thumbnailRect = QRectF(QPointF(75, 11), QSizeF(105, 70));
        hasThumbnail = true;
        viewerBoundingRect.setHeight(thumbnailRect.height() + 2*thumbnailRect.top());
    }


    if(Global::viewerGL->visibleRect.intersects(viewerBoundingRect.translated(viewerPos))) {
        glPushMatrix();
        glTranslatef(qRound(viewerPos.x()), qRound(viewerPos.y()), 0);

        glTranslatef(qRound(viewerBoundingRect.center().x()), qRound(viewerBoundingRect.center().y()), 0);
        glScalef(tagScale, tagScale, 1);
        glTranslatef(-qRound(viewerBoundingRect.center().x()), -qRound(viewerBoundingRect.center().y()), 0);

        //Selection
        if(Global::selectedTag == this) {
            QColor color = Global::colorTimeline;
            if((document->status == DocumentStatusProcessing) || (Global::selectedTag == this))
                color.setAlphaF(Global::breathingFast);
            Global::viewerGL->qglColor(color);
            GlRect::drawRect(viewerBoundingRect);
        }

        //Alternative background
        if(tagIndex % 2) {
            Global::viewerGL->qglColor(Global::colorAlternate);
            GlRect::drawRect(viewerBoundingRect);
        }

        //Progression
        isInProgress = false;
        bool isBlinking = false;
        QRectF progressionRect(viewerBoundingRect.topLeft(), QSizeF(viewerBoundingRect.width(), 5));
        if((getType() == TagTypeContextualMilestone) && (blinkTime)) {
            isInProgress = true;
            blinkTime = qMax(0., blinkTime-20);
            if(qFloor(blinkTime / 250) % 2) {
                if(document->getType() == DocumentTypeMarker) {
                    isBlinking = true;
                    Global::viewerGL->qglColor(realTimeColor);
                    GlRect::drawRect(viewerBoundingRect);
                }
                else {
                    Global::viewerGL->qglColor(Global::colorTimeline);
                    GlRect::drawRect(progressionRect);
                }
            }
        }
        else if((0.001 < progression) && (progression < 0.999)) {
            isInProgress = true;
            if(document->getType() == DocumentTypeMarker) {
                Global::viewerGL->qglColor(realTimeColor);
                GlRect::drawRect(QRectF(progressionRect.topLeft(), QSizeF(progressionRect.width() * progression, viewerBoundingRect.height())));
            }
            else {
                Global::viewerGL->qglColor(Global::colorTimeline);
                GlRect::drawRect(QRectF(progressionRect.topLeft(), QSizeF(progressionRect.width() * progression, progressionRect.height())));
            }
        }

        //Bar
        QColor barColor = realTimeColor;
        if((decounter == 0) && (!isInProgress) && (Global::timerPlay))  barColor.setAlphaF(0.2);
        else                                                            barColor.setAlphaF(1.0);


        Global::viewerGL->qglColor(barColor);
        if(hasThumbnail)
            GlRect::drawRect(thumbnailRect.adjusted(-5, -5, 5, 5));
        else if(document->getType() == DocumentTypeMarker)
            GlRect::drawRect(QRectF(viewerBoundingRect.topLeft(), QSizeF(5, viewerBoundingRect.height())));
        else {
            if(isTagLastVersion(this))
                GlRect::drawRoundedRect(viewerBoundingRect.adjusted(7, 7, -75, -7).translated(QPointF(60, 0)), false, M_PI/4);
            GlRect::drawRoundedRect(viewerBoundingRect.adjusted(7, 7, -75, -7).translated(QPointF(60, 0)), true, M_PI/4);
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
            viewerTimeText.drawText(Sorting::timeToString(getTimeStart()));
        }


        //Thumb
        QPoint textePos(75, 0);
        if(hasThumbnail) {
            Global::viewerGL->qglColor(QColor(255, 255, 255, barColor.alpha()));
            document->thumbnails.first().drawTexture(thumbnailRect, Global::breathingPics);
            textePos = thumbnailRect.topRight().toPoint() + QPoint(10, -10);
        }

        //Texte
        if((isBlinking) || (isInProgress))                                                                                                  Global::viewerGL->qglColor(Qt::white);
        else if(((Global::selectedTag == this) || ((isTagLastVersion(this) && (!hasThumbnail)))) && (document->getType() != DocumentTypeMarker)) Global::viewerGL->qglColor(Qt::black);
        else                                                                                                                                Global::viewerGL->qglColor(barColor);
        QString texte = document->getName(documentVersion);
        if(getType() == TagTypeContextualTime)
            texte += QString(" (%1)").arg(Sorting::timeToString(getDuration()));
        viewerDocumentText.drawText(texte, textePos);

        glPopMatrix();
    }
    if(viewerFirstPosVisible) {
        viewerFirstPosVisible = false;
        Global::viewerGL->ensureVisible(viewerBoundingRect.translated(viewerDestPos).topLeft());
    }
    return viewerBoundingRect.translated(viewerPos);
}

bool Tag::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool dbl, bool, bool, bool press) {
    if(timelineContains(pos)) {
        if(press) {
            if(Global::selectedTag != this) {
                Global::selectedTag      = this;
                Global::selectedTagHover = this;
                Global::mainWindow->refreshMetadata(this, true);
            }
            else if(Global::selectedTagInAction != this) {
                Global::selectedTagInAction  = this;
                Global::selectedTagStartDrag = timelineProgress(pos) * getDuration();
                if((e->button() & Qt::LeftButton) == Qt::LeftButton) {
                    if(     (timelineProgress(pos) < 0.1) && (getType() == TagTypeContextualTime))   Global::selectedTagMode = TagSelectionStart;
                    else if((timelineProgress(pos) > 0.9) && (getType() == TagTypeContextualTime))   Global::selectedTagMode = TagSelectionEnd;
                    else                                                                        Global::selectedTagMode = TagSelectionMove;
                }
            }
            if(dbl) {
                if(document->chutierItem) {
                    document->chutierItem->fileShowInOS();
                    tagScale     = 3;
                    tagDestScale = 1;
                }
            }
            return true;
        }
        if(!Global::selectedTag)
            Global::mainWindow->refreshMetadata(this, false);
        Global::selectedTagHover = this;

        return true;
    }
    return (Global::selectedTagInAction == this);
}
bool Tag::mouseViewer(const QPointF &pos, QMouseEvent *, bool dbl, bool, bool, bool press) {
    if(viewerContains(pos)) {
        Global::selectedTagHover = this;

        if(press) {
            Global::timeline->seek(getTimeStart(), true, false);
            Global::selectedTag         = this;
            Global::selectedTagInAction = 0;
            if(document->chutierItem)
                Global::chutier->setCurrentItem(document->chutierItem);
        }
        if((dbl) && (document->chutierItem)) {
            /*
            document->chutierItem->fileShowInOS();
            tagScale     = 3;
            tagDestScale = 1;
            */
            Global::mainWindow->showPreviewTab();
        }
        return true;
    }
    return false;
}


void Tag::snapTime(qreal *time) const {
    if(qAbs(*time-getTimeStart()) < 1) {
        *time = getTimeStart();
        Global::selectedTagHoverSnapped = *time;
    }
    else if(qAbs(*time-getTimeEnd()) < 1) {
        *time = getTimeEnd();
        Global::selectedTagHoverSnapped = *time;
    }
}


bool Tag::tagHistoryFilters() const {
    return (   Global::showHistory) ||
            ((!Global::showHistory) && (isTagLastVersion(this)));
}
bool Tag::isAcceptableWithSortFilters(bool strongCheck) const {
    return (tagHistoryFilters()) && (document->isAcceptableWithSortFilters(strongCheck, documentVersion));
}
bool Tag::isAcceptableWithColorFilters(bool strongCheck) const {
    return (tagHistoryFilters()) && (document->isAcceptableWithColorFilters(strongCheck, documentVersion));
}
bool Tag::isAcceptableWithClusterFilters(bool strongCheck) const {
    return (tagHistoryFilters()) && (document->isAcceptableWithClusterFilters(strongCheck, documentVersion));
}
bool Tag::isAcceptableWithFilterFilters(bool strongCheck) const {
    return (tagHistoryFilters()) && (document->isAcceptableWithFilterFilters(strongCheck, documentVersion));
}
bool Tag::isAcceptableWithHorizontalFilters(bool strongCheck) const {
    return (tagHistoryFilters()) && (document->isAcceptableWithHorizontalFilters(strongCheck, documentVersion));
}

const QString Tag::getAcceptableWithClusterFilters() const {
    return document->getAcceptableWithClusterFilters(documentVersion);
}


bool Tag::sortColor(const Tag *first, const Tag *second) {
    if((!first) || (!second))
        return false;

    QString firstStr = Tag::getCriteriaColor(first), secondStr = Tag::getCriteriaColor(second);
    if(firstStr == secondStr)
        return first->document->getName(first->getDocumentVersion()) < second->document->getName(second->getDocumentVersion());
    else
        return firstStr < secondStr;
}
bool Tag::sortViewer(const Tag *first, const Tag *second) {
    if((!first) || (!second))
        return false;
    if((first->progressionDest == second->progressionDest) || (((0. < first->progressionDest) && (first->progressionDest < 1.)) && ((0. < second->progressionDest) && (second->progressionDest < 1.))))
        return first->getTimeStart() < second->getTimeStart();
    else
        return first->progressionDest > second->progressionDest;
}
bool Tag::sortEvents(const Tag *first, const Tag *second) {
    if((!first) || (!second))
        return false;
    return first->getTimeStart() < second->getTimeStart();
}
bool Tag::sortAlpha(const Tag *first, const Tag *second) {
    if((!first) || (!second))
        return false;
    return first->document->getName(first->getDocumentVersion()) < second->document->getName(second->getDocumentVersion());
}


const QString Tag::getCriteriaSort(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaSort(tag->documentVersion);
}
const QString Tag::getCriteriaColor(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaColor(tag->documentVersion);
}
const QString Tag::getCriteriaCluster(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaCluster(tag->documentVersion);
}
const MetadataElement Tag::getCriteriaPhase(const Tag *tag) {
    return tag->document->getCriteriaPhase(tag->documentVersion);
}
const QString Tag::getCriteriaFilter(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaFilter(tag->documentVersion);
}
const QString Tag::getCriteriaFilterFormated(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaFilterFormated(tag->documentVersion);
}
const QString Tag::getCriteriaHorizontal(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaHorizontal(tag->documentVersion);
}
const QString Tag::getCriteriaHorizontalFormated(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaHorizontalFormated(tag->documentVersion);
}


const QString Tag::getCriteriaSortFormated(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaSortFormated(tag->documentVersion);
}
const QString Tag::getCriteriaColorFormated(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaColorFormated(tag->documentVersion);
}
const QString Tag::getCriteriaClusterFormated(const Tag *tag) {
    if(!tag)    return "";
    return tag->document->getCriteriaClusterFormated(tag->documentVersion);
}

bool Tag::isTagLastVersion(const Tag *tag) {
    if(tag) return (tag->documentVersion < 0) || (tag->documentVersion == tag->document->getMetadataCountM());
    else    return false;
}

QDomElement Tag::serialize(QDomDocument &xmlDoc) const {
    QDomElement xmlData = xmlDoc.createElement("tag");
    xmlData.setAttribute("timeStart",       getTimeStart());
    xmlData.setAttribute("timeEnd",         getTimeEnd());
    xmlData.setAttribute("type",            getType());
    xmlData.setAttribute("documentVersion", documentVersion);
    if(linkedTags.count()) {
        QDomElement linkedTagsXml = xmlDoc.createElement("linkedTags");
        foreach(Tag *linkedTag, linkedTags) {
            QDomElement linkedTagXml = xmlDoc.createElement("linkedTag");
            linkedTagXml.setAttribute("documentVersion", linkedTag->documentVersion);
            linkedTagsXml.appendChild(linkedTagXml);
        }
        xmlData.appendChild(linkedTagsXml);
    }
    return xmlData;
}
void Tag::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
