#include "project.h"

Project::Project(QWidget *parent) :
    ProjectBase(parent) {
    categoryColorOpacity = categoryColorOpacityDest = 0;
    textureStrips.setTexture(":/textures/res_texture_strips.png");
    nbTagsPerCategories = 1;
    timelineFilesMenu = new QMenu(Global::mainWindow);
}

void Project::open(const QFileInfoList &files, UiTreeView *view, bool debug) {
    foreach(const QFileInfo &file, files) {
        QDir dir(file.absoluteFilePath());
        if(file.isFile())
            dir.cdUp();
        UiFileItem::syncWith(QFileInfoList() << dir.absolutePath(), view->getTree());
        open(dir, dir, debug);
    }
}
void Project::open(const QDir &dir, const QDir &dirBase, bool debug) {
    QFileInfoList files = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
    foreach(const QFileInfo &file, files) {
        if((file.isFile()) && (UiFileItem::conformFile(file))) {
            Document *document = new Document(this, dirBase);
            document->chutierItem = UiFileItem::find(file, Global::chutier);
            if(file.absoluteFilePath().contains("Test.txt"))
                document->chutierItem->setData(1, Qt::EditRole, true);

            if(document->updateFile(file))
                if(debug)
                    document->createTagBasedOnPrevious();
            document->updateFeed();

            if(debug) {
                if(file.baseName() == "Plan de salle 2") {
                    for(quint16 i = 1 ; i < 10 ; i++) {
                        if(document->updateFile(file, -1, i))
                            document->createTagBasedOnPrevious();
                        document->updateFeed();
                    }
                }
            }
        }
        else if((file.isDir()) && (UiFileItem::conformFile(file)))
            open(QDir(file.absoluteFilePath() + "/"), dirBase, debug);
    }


    if(debug) {
        for(quint16 i = 1 ; i < 2 ; i++) {
            Document *document = new Document(this);
            if(document->updateImport(QString("Marker #%1").arg(i)))
                document->createTagBasedOnPrevious();
            document->updateFeed();
        }

        Tag *previousTag = 0;
        foreach(Document *document, documents) {
            foreach(Tag *tag, document->tags) {
                if(Global::alea(0, 100) > 80)
                    tag->linkedTags.append(previousTag);
                previousTag = tag;
            }
        }
    }

    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::metaChanged = true;
}

Document* Project::getDocument(const QString &name) {
    foreach(Document *document, documents)
        if(document->file.absoluteFilePath() == name)
            return document;
    return 0;
}
Document* Project::getDocumentAndSelect(const QString &name) {
    Document *documentRetour = 0;
    foreach(Document *document, documents) {
        if(document->file.absoluteFilePath() == name) {
            documentRetour = document;
            foreach(Tag *tag, document->tags) {
                tag->mouseHover = true;
                Global::selectedTagHover = tag;
            }
        }
        else
            foreach(Tag *tag, document->tags)
                tag->mouseHover = false;
    }
    return documentRetour;
}

qreal Project::totalTime() const {
    qreal total = 0;
    foreach(Document *document, documents)
        foreach(Tag *tag, document->tags)
            total = qMax(total, tag->timeEnd);
    return total;
}

void Project::fireEvents() {
    if(!(Global::tagColorCriteria))
        return;

    //Gather and set color information
    if(Global::eventsSortChanged) {
        Global::colorForMeta.clear();
        qreal documentPerColorCount = 0;
        foreach(Document *document, documents) {
            QString colorKey = document->getCriteriaColorFormated();
            if((document->isAcceptableWithColorFilters()) && (document->isAcceptableWithSortFilters())) {
                if(!Global::colorForMeta.contains(colorKey))
                    Global::colorForMeta[colorKey] = QPair<QColor, qreal>(Qt::black, 0);
                Global::colorForMeta.insert(colorKey, QPair<QColor, qreal>(Global::colorForMeta.value(colorKey).first, Global::colorForMeta.value(colorKey).second + 1));
                documentPerColorCount++;
            }
        }
        qreal index = 0;
        QMutableMapIterator<QString, QPair<QColor, qreal> > colorForMetaIterator(Global::colorForMeta);
        while(colorForMetaIterator.hasNext()) {
            colorForMetaIterator.next();
            QColor color;
            color.setHsvF(0.7 * index / (qreal)(Global::colorForMeta.count()-1), 0.7, 0.7);
            colorForMetaIterator.setValue(qMakePair(color, colorForMetaIterator.value().second / documentPerColorCount));
            index++;
        }

        eventsTags.clear();
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if(tag->isAcceptableWithSortFilters())
                    eventsTags.append(tag);

        //Sorting criteria
        qSort(eventsTags.begin(), eventsTags.end(), Tag::sortEvents);

        Global::eventsSortChanged = false;
    }

    if(Global::metaChanged) {
        emit(refreshMetadata());
        Global::metaChanged = false;
    }

    //Opacity
    if((Global::selectedTagHover) && (Global::timelineGL->showLegendDest))  categoryColorOpacityDest = 0.3;
    else                                                                    categoryColorOpacityDest = 1;
    categoryColorOpacity = categoryColorOpacity + (categoryColorOpacityDest - categoryColorOpacity) / Global::inertie;
    QMutableMapIterator<QString, QPair<QColor, qreal> > colorForMetaIterator(Global::colorForMeta);
    while(colorForMetaIterator.hasNext()) {
        colorForMetaIterator.next();
        QColor color = colorForMetaIterator.value().first;
        if((color.alphaF() == 1) && (categoryColorOpacityDest == 1)) {}
        else
            color.setAlphaF(categoryColorOpacity);
        if(Global::selectedTagHover) {
            if(Tag::getCriteriaColorFormated((Tag*)Global::selectedTagHover) == colorForMetaIterator.key())
                color.setAlphaF(1);
        }
        colorForMetaIterator.setValue(qMakePair(color, colorForMetaIterator.value().second));
    }

    //Fire events
    foreach(Tag *tag, eventsTags)
        tag->fireEvents();

    //Progress = viewer reorder
    Global::viewerSortChanged = Global::timerPlay;
}

const QRectF Project::paintTimeline(bool before) {
    QRectF retour;

    if(before) {
        //Gather info about tags and classify them
        if(Global::timelineSortChanged) {
            //Clear
            nbTagsPerCategories = 1;
            timelineSortTags.clear();
            QMap<QString, QMap<QString, Cluster*> > clustersToLink;
            QMapIterator<QPair<QString, QString>, Cluster*> timelineClustersIterator(timelineClusters);
            while(timelineClustersIterator.hasNext()) {
                timelineClustersIterator.next();
                timelineClustersIterator.value()->clear();
            }

            //Find phases
            if(Global::phases->needCalulation) {
                Global::phases->metaElements.clear();
                foreach(Document *document, documents)
                    foreach(Tag *tag, document->tags)
                        if(tag->isAcceptableWithSortFilters())
                            Global::phases->addToMetaElements(Tag::getCriteriaSortRaw(tag));
                Global::phases->analyse();
            }

            //Browse documents
            foreach(Document *document, documents) {
                QList<Tag*> documentHistoryTags;

                //Add tags to list + create history links
                foreach(Tag *tag, document->tags) {
                    //Add to linked tags container for this document
                    if(document->getMetadataCount() > 1)
                        documentHistoryTags.append(tag);

                    //Resets histry
                    tag->historyTags.clear();

                    //Add to timeline if displayable
                    if(tag->isAcceptableWithSortFilters()) {
                        QString phase   = Global::phases->getPhaseFor(Tag::getCriteriaSortRaw(tag));
                        QString sorting = Tag::getCriteriaSort(tag);
                        QString cluster = Tag::getCriteriaCluster(tag);
                        if((!cluster.isEmpty()) && (tag->isAcceptableWithClusterFilters()) && (!Global::tagClusterCriteria->getMatchName().isEmpty())) {
                            cluster = tag->getAcceptableWithClusterFilters();
                            QPair<QString,QString> key = qMakePair(sorting, cluster);
                            if(!timelineClusters.contains(key))
                                timelineClusters.insert(key, new Cluster(this));
                            timelineClusters[key]->add(tag);
                            clustersToLink[cluster][sorting] = timelineClusters.value(key);
                        }
                        timelineSortTags[phase][sorting][cluster].append(tag);
                    }
                }

                //History tags
                if(Global::tagFilterCriteria->displayLinked) {
                    if(Global::tagSortCriteria->asDate) {
                        //Sort by version number
                        QList<Tag*> historyTagsVersionBefore;
                        for(quint16 version = 0 ; version < document->getMetadataCount() ; version++) {
                            QList<Tag*> historyTagsVersionNow;
                            foreach(Tag* tag, documentHistoryTags) {
                                if(tag->getDocumentVersion() == version) {
                                    tag->historyTags = historyTagsVersionBefore;
                                    historyTagsVersionNow.append(tag);
                                }
                            }
                            historyTagsVersionBefore = historyTagsVersionNow;
                        }
                    }
                    else {
                        //Add history tags to last version tag
                        foreach(Tag* tag, documentHistoryTags)
                            if(Tag::isTagLastVersion(tag))
                                tag->historyTags = documentHistoryTags;
                    }
                }
            }

            //Sort clusters
            QMapIterator<QString, QMap<QString, Cluster*> > clustersBySortToLinkIterator(clustersToLink);
            while(clustersBySortToLinkIterator.hasNext()) {
                clustersBySortToLinkIterator.next();
                const Cluster *clusterOld = 0;
                QMapIterator<QString, Cluster*> clustersToLinkIterator(clustersBySortToLinkIterator.value());
                while(clustersToLinkIterator.hasNext()) {
                    clustersToLinkIterator.next();
                    clustersToLinkIterator.value()->linkedCluster = clusterOld;
                    clusterOld = clustersToLinkIterator.value();
                }
            }

            //Quantity per categories
            QMapIterator<QString, QMap<QString, QMap<QString, QList<Tag*> > > > categoriesInPhasesIterator(timelineSortTags);
            while(categoriesInPhasesIterator.hasNext()) {
                categoriesInPhasesIterator.next();
                QMapIterator<QString, QMap<QString, QList<Tag*> > > clustersInCategoriesIterator(categoriesInPhasesIterator.value());
                while(clustersInCategoriesIterator.hasNext()) {
                    clustersInCategoriesIterator.next();
                    QMapIterator<QString, QList<Tag*> > tagsInClusterIterator(clustersInCategoriesIterator.value());
                    while(tagsInClusterIterator.hasNext()) {
                        tagsInClusterIterator.next();
                        nbTagsPerCategories = qMax(nbTagsPerCategories, (qreal)tagsInClusterIterator.value().count());
                        qSort(timelineSortTags[categoriesInPhasesIterator.key()][clustersInCategoriesIterator.key()][tagsInClusterIterator.key()].begin(), timelineSortTags[categoriesInPhasesIterator.key()][clustersInCategoriesIterator.key()][tagsInClusterIterator.key()].end(), Tag::sortCriteria);
                    }
                }
            }

            //Unlock
            Global::timelineSortChanged = false;
        }


        //Header
        glEnable(GL_SCISSOR_TEST);
        glPushMatrix();
        glTranslatef(0, Global::timelineHeaderSize.height(), 0);


        //Drawing tags and categories
        bool debug = false;
        QList<QRectF> zones;
        quint16 categoryIndex = 0;
        QPointF tagSortPosOffset = QPointF(0, Global::timelineTagVSpacingSeparator), categoryStart = QPointF(0, 0);
        guiToggles.clear();
        guiCategories.clear();
        QMapIterator<QString, QMap<QString, QMap<QString, QList<Tag*> > > > categoriesInPhasesIterator(timelineSortTags);
        while(categoriesInPhasesIterator.hasNext()) {
            categoriesInPhasesIterator.next();

            QString phase = categoriesInPhasesIterator.key();
            if(debug)
                qDebug("\t > [Phase] %s", qPrintable(phase));

            QMapIterator<QString, QMap<QString, QList<Tag*> > > clustersInCategoriesIterator(categoriesInPhasesIterator.value());
            while(clustersInCategoriesIterator.hasNext()) {
                clustersInCategoriesIterator.next();

                bool drawToggle = false;
                QMapIterator<QString, QList<Tag*> > tagsInClusterIterator(clustersInCategoriesIterator.value());
                while(tagsInClusterIterator.hasNext()) {
                    tagsInClusterIterator.next();
                    foreach(Tag *tag, tagsInClusterIterator.value()) {
                        if(tag->getDocument()->function == DocumentFunctionRender) {
                            drawToggle = true;
                            break;
                        }
                    }
                }

                QString sorting = clustersInCategoriesIterator.key();
                if(debug)
                    qDebug("\t\t > [Sorting] %s", qPrintable(sorting));

                //Caching extraction for heatmap
                while(timelineCategoriesRectCache.count() <= categoryIndex)
                    timelineCategoriesRectCache.append(qMakePair(QRectF(), 1.));
                QRectF tagCategoryRect  = timelineCategoriesRectCache.at(categoryIndex).first;
                qreal nbTagsPerCategory = timelineCategoriesRectCache.at(categoryIndex).second;

                //Draw heatmap
                /*
                QRectF categoryHeatRect = QRectF(QPointF(tagCategoryRect.left() + tagCategoryRect.width() - 2, tagCategoryRect.top()), QSizeF(Global::timelineNegativeHeaderWidth, tagCategoryRect.height()));
                if(Global::timelineGL->visibleRect.intersects(categoryHeatRect.translated(QPointF(0, Global::timelineHeaderSize.height())))) {
                    glScissor(Global::timelineHeaderSize.width() - 2, 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height());
                    quint16 val = nbTagsPerCategory * 255. / nbTagsPerCategories;
                    if(drawToggle)
                        Global::timelineGL->qglColor(Global::colorTextBlack);
                    else
                        Global::timelineGL->qglColor(QColor(255, 255, 255, val/2));
                    GlRect::drawRect(categoryHeatRect);
                    guiCategories.append(qMakePair(categoryHeatRect.translated(QPointF(0, Global::timelineHeaderSize.height())), qMakePair(phase, sorting)));
                    glScissor(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth, 0, Global::timelineGL->width() - Global::timelineHeaderSize.width() - Global::timelineNegativeHeaderWidth, Global::timelineGL->height());
                }
                */
                guiCategories.append(qMakePair(tagCategoryRect.translated(QPointF(0, Global::timelineHeaderSize.height())), qMakePair(phase, sorting)));

                //New category
                qreal yMax = 0;
                bool drawToggleNext = false;
                nbTagsPerCategory = 0;
                QString tagCategory;
                bool tagCategoryIsSelected = false;
                UiBool *drawToggleActive = 0;

                //Draw tags of this categetory
                QMapIterator<QString, QList<Tag*> > tagsInClusterIterator2(clustersInCategoriesIterator.value());
                while(tagsInClusterIterator2.hasNext()) {
                    tagsInClusterIterator2.next();

                    QString cluster = tagsInClusterIterator2.key();
                    if(debug)
                        qDebug("\t\t\t > [Cluster] %s (%d)", qPrintable(cluster), tagsInClusterIterator2.value().count());

                    foreach(Tag *tag, tagsInClusterIterator2.value()) {
                        QRectF tagRect = tag->getTimelineBoundingRect().translated(tagSortPosOffset);
                        QPointF tagPosOffset;

                        //Category in selection
                        tagCategoryIsSelected |= tag->mouseHover;

                        //Get info about category and analysis
                        if(tagCategory.isEmpty()) {
                            tagCategory = " " + Tag::getCriteriaSortFormated(tag).trimmed().toUpper();
                            if(clustersInCategoriesIterator.hasNext()) {
                                clustersInCategoriesIterator.next();
                                if(clustersInCategoriesIterator.value().count())
                                    if(clustersInCategoriesIterator.value().value(clustersInCategoriesIterator.value().keys().first()).count())
                                        if(clustersInCategoriesIterator.value().value(clustersInCategoriesIterator.value().keys().first()).first()->getDocument()->function == DocumentFunctionRender)
                                            drawToggleNext = true;
                                clustersInCategoriesIterator.previous();
                            }
                            drawToggle = (tag->getDocument()->function == DocumentFunctionRender);
                            if(drawToggle)
                                drawToggleActive = &(tag->getDocument()->renderActive);
                        }

                        //Drawing
                        bool tagZoneIntersection = false;
                        while(!tagZoneIntersection) {
                            tagZoneIntersection = true;
                            foreach(const QRectF &zone, zones) {
                                if(tagRect.intersects(zone))
                                    tagZoneIntersection = false;
                            }
                            if(!tagZoneIntersection) {
                                if(tag->type == TagTypeGlobal) {
                                    if(tagPosOffset.x() < Global::timelineNegativeHeaderWidth-2*Global::timelineTagHeight) {
                                        tagPosOffset +=   QPointF(Global::timelineTagHeight, 0);
                                        tagRect.translate(QPointF(Global::timelineTagHeight, 0));
                                    }
                                    else {
                                        tagPosOffset +=   QPointF(-tagPosOffset.x(), Global::timelineTagHeight + Global::timelineTagVSpacing);
                                        tagRect.translate(QPointF(-tagRect.width(),  Global::timelineTagHeight + Global::timelineTagVSpacing));
                                    }
                                }
                                else {
                                    tagPosOffset +=   QPointF(0, Global::timelineTagHeight + Global::timelineTagVSpacing);
                                    tagRect.translate(QPointF(0, Global::timelineTagHeight + Global::timelineTagVSpacing));
                                }
                            }
                            else
                                tag->setTimelinePos(tagPosOffset + tagSortPosOffset + QPointF(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth, 0));
                        }
                        //Drawing
                        retour = retour.united(tag->paintTimeline(before));
                        zones.append(tagRect);
                        yMax = qMax(yMax, tagRect.bottom());
                        nbTagsPerCategory++;
                    }
                }

                //Extract category name and rect, store it for caching
                tagCategoryRect = QRectF(categoryStart, QPointF(Global::timelineHeaderSize.width(), yMax + Global::timelineTagVSpacingSeparator)).translated(Global::timelineGL->scroll.x(), 0);
                timelineCategoriesRectCache[categoryIndex] = qMakePair(tagCategoryRect, nbTagsPerCategory);
                tagCategoryRect.setWidth(tagCategoryRect.width() - 2);

                //Draw category background
                if(Global::timelineGL->visibleRect.intersects(tagCategoryRect.translated(QPointF(0, Global::timelineHeaderSize.height())))) {
                    glDisable(GL_SCISSOR_TEST);
                    if(drawToggle) {
                        Global::timelineGL->qglColor(Global::colorAlternate);
                        GlRect::drawRect(tagCategoryRect);
                        //Global::timelineGL->qglColor(Global::colorBackgroundDark);
                        //GlRect::drawRect(QRectF(tagCategoryRect.topLeft(), QSizeF(Global::timelineGL->width(), tagCategoryRect.height())));
                    }
                    else {
                        Global::timelineGL->qglColor(Global::colorBackground);
                        GlRect::drawRect(tagCategoryRect);
                        Global::timelineGL->qglColor(Global::colorAlternate);
                        GlRect::drawRect(QRectF(tagCategoryRect.topLeft(), QSizeF(Global::timelineGL->width(), tagCategoryRect.height())));
                    }

                    //Draw text
                    if(tagCategoryIsSelected)   Global::timelineGL->qglColor(Global::colorSelection);
                    else                        Global::timelineGL->qglColor(Global::colorText);
                    bool textFound = false;
                    if(timelineCategories.count() > 1000)
                        timelineCategories.clear();
                    foreach(GlText timelineCategory, timelineCategories) {
                        if((timelineCategory.text == tagCategory) && (timelineCategory.size == tagCategoryRect.size().toSize())) {
                            timelineCategory.drawText(tagCategoryRect.topLeft().toPoint());
                            textFound = true;
                        }
                    }
                    if(!textFound) {
                        GlText tagCategoryText;
                        tagCategoryText.setStyle(tagCategoryRect.size().toSize(), Qt::AlignVCenter, Global::font);
                        tagCategoryText.drawText(tagCategory, tagCategoryRect.topLeft().toPoint());
                        timelineCategories << tagCategoryText;
                    }

                    //Draw renders tools
                    if(drawToggle) {
                        QRectF toggleRect(QPointF(tagCategoryRect.right() - 30, tagCategoryRect.center().y()-Global::timelineTagHeight/2), QSizeF(Global::timelineTagHeight, Global::timelineTagHeight));
                        Global::timelineGL->qglColor(Global::colorSelection);
                        if((drawToggleActive) && (drawToggleActive->val()))
                            GlRect::drawRoundedRect(toggleRect, false);
                        GlRect::drawRoundedRect(toggleRect, true);
                        guiToggles.append(qMakePair(toggleRect.translated(QPointF(0, Global::timelineHeaderSize.height())), drawToggleActive));
                    }
                    glEnable(GL_SCISSOR_TEST);
                }
                //Super separator si big change of category
                qreal vSpacing = Global::timelineTagVSpacingSeparator;
                if(((drawToggle) && (!drawToggleNext)) || ((!drawToggle) && (drawToggleNext)))
                    vSpacing = Global::timelineTagVSpacingSeparator;
                if((!clustersInCategoriesIterator.hasNext()) && (categoriesInPhasesIterator.hasNext()))
                    vSpacing = 10;

                //Super category
                if(vSpacing > Global::timelineTagVSpacingSeparator) {
                    tagCategoryRect = QRectF(QPointF(0, yMax + 2*Global::timelineTagVSpacing+1), QPointF(Global::timelineGL->width(), yMax + vSpacing)).translated(Global::timelineGL->scroll.x(), 0);

                    /*
                    glDisable(GL_SCISSOR_TEST);
                    Global::timelineGL->qglColor(Global::colorBackgroundDark);
                    textureStrips.drawTexture(tagCategoryRect, -4);
                    glEnable(GL_SCISSOR_TEST);
                    */
                }

                //New offset
                tagSortPosOffset.setY(yMax + vSpacing + Global::timelineTagVSpacingSeparator);
                categoryStart   .setY(yMax + vSpacing);
                categoryIndex++;
            }
        }

        //Drawing clusters
        if(!Global::tagClusterCriteria->tagName.isEmpty()) {
            glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height());
            QMapIterator<QPair<QString, QString>, Cluster*> timelineClustersIterator(timelineClusters);
            while(timelineClustersIterator.hasNext()) {
                timelineClustersIterator.next();
                timelineClustersIterator.value()->paintTimeline();
            }
            glScissor(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth, 0, Global::timelineGL->width() - Global::timelineHeaderSize.width() - Global::timelineNegativeHeaderWidth, Global::timelineGL->height());
        }

        glPopMatrix();
        glDisable(GL_SCISSOR_TEST);
    }
    else {
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                tag->paintTimeline(before);

        //Current tags
        QList<Tag*> currentTagsWithThumbs, currentTagsWithoutThumbs;
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if((tag->getDocument()) && (tag->type == TagTypeContextualTime) && (tag->getDocument()->function == DocumentFunctionContextual) && (0.001 < tag->progression) && (tag->progression < 0.999)) {
                    if(!tag->getDocument()->thumbnails.count()) currentTagsWithThumbs.append(tag);
                    else                                        currentTagsWithoutThumbs.append(tag);
                }
        //qDebug("------------------------------------");
        foreach(Tag *tag, currentTagsWithThumbs) {
            //qDebug("%s", qPrintable(tag->getDocument()->getMetadata("Rekall", "Document Name").toString()));
        }
        //qDebug("----");
        foreach(Tag *tag, currentTagsWithoutThumbs) {
            //qDebug("%s", qPrintable(tag->getDocument()->getMetadata("Rekall", "Document Name").toString()));
        }
    }
    return retour;
}

const QRectF Project::paintViewer() {
    QRectF retour;
    quint16 tagIndex = 0;
    if(Global::viewerGL) {
        //Gather information
        if(Global::viewerSortChanged) {
            viewerTags.clear();
            foreach(Document *document, documents)
                foreach(Tag *tag, document->tags)
                    if(tag->isAcceptableWithSortFilters())
                        if((tag->getDocument()->function != DocumentFunctionRender) && (tag->type != TagTypeGlobal))
                            viewerTags.append(tag);

            //Sorting criteria
            qSort(viewerTags.begin(), viewerTags.end(), Tag::sortViewer);

            Global::viewerSortChanged = false;
        }

        //Drawing
        Tag *previousTag = 0;
        QPointF tagSortPosOffset = QPointF(0, Global::viewerTagHeight / 2);
        foreach(Tag *tag, viewerTags) {
            QRectF tagRect = tag->getViewerBoundingRect();
            tag->setViewerPos(tagSortPosOffset);

            //Drawings
            retour = retour.united(tag->paintViewer(tagIndex++));

            //Next coordinates
            tagSortPosOffset += QPointF(0, tagRect.height());
            previousTag = tag;
        }
    }
    return retour;
}


QPointF Project::getTimelineCursorPos(qreal time) {
    //Linear cursor
    return QPointF(Global::timelineHeaderSize.width() + Global::timelineNegativeHeaderWidth + Global::timeUnit * time, 0);
}
qreal Project::getTimelineCursorTime(const QPointF &pos) {
    qreal cursorTime = qMax(0., (pos.x() - Global::timelineHeaderSize.width() - Global::timelineNegativeHeaderWidth) / Global::timeUnit);
    Global::selectedTagHoverSnapped = -1;
    if((Global::selectedTagHoverSnap) && (Global::selectedTagHoverSnap != Global::selectedTag))
        ((Tag*)Global::selectedTagHoverSnap)->snapTime(&cursorTime);
    return cursorTime;
}

QPointF Project::getViewerCursorPos(qreal time) {
    //Unlinear cursor
    Tag *lastTag = 0;
    for(qint16 tagIndex = 0 ; tagIndex < viewerTags.count() ; tagIndex++) {
        if((tagIndex < viewerTags.count()-1) && (viewerTags.at(tagIndex)->timeStart <= time) && (time <= viewerTags.at(tagIndex+1)->timeStart)) {
            qreal viewCurrentPos = (time - viewerTags.at(tagIndex)->timeStart) / (viewerTags.at(tagIndex+1)->timeStart - viewerTags.at(tagIndex)->timeStart);
            viewCurrentPos = viewerTags.at(tagIndex)->viewerPos.y() * (1-viewCurrentPos) + viewerTags.at(tagIndex+1)->viewerPos.y() * (viewCurrentPos);
            return QPointF(0, viewCurrentPos);
            break;
        }
        if((!lastTag) || (lastTag->timeEnd < viewerTags.at(tagIndex)->timeEnd))
            lastTag = viewerTags.at(tagIndex);
    }
    if(viewerTags.count()) {
        if(Global::time < viewerTags.first()->timeStart)
            return QPointF(0, (Global::time / viewerTags.first()->timeStart) * viewerTags.first()->viewerPos.y());
        else if(lastTag) {
            qreal viewCurrentPos = (time - viewerTags.last()->timeStart) / (lastTag->timeEnd - viewerTags.last()->timeStart);
            viewCurrentPos = viewerTags.last()->viewerPos.y() * (1-viewCurrentPos) + (viewerTags.last()->viewerPos.y() + viewerTags.last()->getViewerBoundingRect().height()) * (viewCurrentPos);
            return QPointF(0, viewCurrentPos);
        }
    }
    return QPointF();
}
qreal Project::getViewerCursorTime(const QPointF &pos) {
    //Unlinear cursor
    qreal time = 0;
    foreach(Tag *tag, viewerTags)
        if(tag->viewerContains(pos))
            time = tag->timeStart;

    return time;
}

bool Project::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool dbl, bool stay, bool action, bool press) {
    bool ok = false;
    foreach(Document *document, documents)
        foreach(Tag *tag, document->tags)
            ok |= tag->mouseTimeline(pos, e, dbl, stay, action, press);

    if(!ok) {
        Global::selectedTag = 0;
        Global::selectedTagHover = 0;
        if((press) && ((e->button() & Qt::RightButton) == Qt::RightButton)) {
            for(quint16 i = 0 ; i < guiCategories.count() ; i++) {
                if(guiCategories.at(i).first.contains(pos)) {
                    timelineFilesMenu->clear();
                    QMapIterator<QString, QList<Tag*> > tagsInClusterIterator(timelineSortTags.value(guiCategories.at(i).second.first).value(guiCategories.at(i).second.second));
                    while(tagsInClusterIterator.hasNext()) {
                        tagsInClusterIterator.next();
                        foreach(Tag *tag, tagsInClusterIterator.value())
                            tag->timelineFilesAction = timelineFilesMenu->addAction(tag->getDocument()->getMetadata("Rekall", "Document Name").toString());
                    }
                    QAction *retour = timelineFilesMenu->exec(QCursor::pos());
                    if(retour) {
                        QMapIterator<QString, QList<Tag*> > tagsInClusterIterator2(timelineSortTags.value(guiCategories.at(i).second.first).value(guiCategories.at(i).second.second));
                        while(tagsInClusterIterator2.hasNext()) {
                            tagsInClusterIterator2.next();
                            foreach(Tag *tag, tagsInClusterIterator2.value()) {
                                if(retour == tag->timelineFilesAction) {
                                    if(tag->getDocument()->chutierItem)
                                        Global::chutier->setCurrentItem(tag->getDocument()->chutierItem);
                                }
                            }
                        }
                    }
                    return true;
                }
            }
        }
        if(((e->button() & Qt::LeftButton) == Qt::LeftButton)) {
            for(quint16 i = 0 ; i < guiToggles.count() ; i++) {
                if(guiToggles.at(i).first.contains(pos)) {
                    if(press)
                        *guiToggles[i].second = !(*guiToggles[i].second);
                    return true;
                }
            }
        }
    }
    return ok;
}
bool Project::mouseViewer(const QPointF &pos, QMouseEvent *e, bool dbl, bool stay, bool action, bool press) {
    bool ok = false;
    foreach(Document *document, documents)
        foreach(Tag *tag, document->tags)
            ok |= tag->mouseViewer(pos, e, dbl, stay, action, press);
    if(!ok) {
        Global::selectedTag = 0;
        Global::selectedTagHover = 0;
    }
    return ok;
}




QDomElement Project::serialize(QDomDocument &xmlDoc) {
    QDomElement xmlData;
    xmlData = xmlDoc.createElement("project");
    xmlData.setAttribute("attribut", "test");
    foreach(Document *document, documents)
        xmlData.appendChild(document->serialize(xmlDoc));
    return xmlData;
}
void Project::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
void Project::save() {
    QDomDocument xmlDoc("rekall");
    xmlDoc.appendChild(serialize(xmlDoc));
    QFile xmlFile(Global::pathCurrent.absoluteFilePath() + "/rekall_cache/project.xml");
    xmlFile.open(QFile::WriteOnly);
    xmlFile.write(xmlDoc.toByteArray());
    xmlFile.close();
    qDebug("%s", qPrintable(xmlDoc.toString()));
}
