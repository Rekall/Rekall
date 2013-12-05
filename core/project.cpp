#include "project.h"

Project::Project(QWidget *parent) :
    ProjectBase(parent) {
    categoryColorOpacity = categoryColorOpacityDest = 0;
    textureStrips.setTexture(":/textures/res_texture_strips.png");
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
            Document *document = new Document(this);
            document->chutierItem = UiFileItem::find(file, Global::chutier);
            if(file.absoluteFilePath().contains("Test.txt"))
                document->chutierItem->setData(1, Qt::EditRole, true);

            if((document->updateFile(file, dirBase)) && (debug))
                document->createTag();
            document->updateFeed();

            if(debug) {
                if(file.baseName() == "NxSimoneMorphing") {
                    for(quint16 i = 1 ; i < 10 ; i++) {
                        if(document->updateFile(file, dirBase, -1, i))
                            document->createTag();
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
                document->createTag();
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

    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::phases->needCalulation = true;
}

Document* Project::getDocument(const QString &name) const {
    foreach(Document *document, documents)
        if(document->file.absoluteFilePath() == name)
            return document;
    return 0;
}
Document* Project::getDocumentAndSelect(const QString &name) const {
    Document *documentRetour = 0;
    foreach(Document *document, documents) {
        if(document->file.absoluteFilePath() == name) {
            documentRetour = document;
            foreach(Tag *tag, document->tags)
                Global::selectedTag = tag;
        }
    }
    return documentRetour;
}

qreal Project::totalTime() const {
    qreal total = 0;
    foreach(Document *document, documents)
        foreach(Tag *tag, document->tags)
            total = qMax(total, tag->getTimeEnd());
    return total;
}

void Project::fireEvents() {
    if(!(Global::tagColorCriteria))
        return;

    //Gather and set color information
    if(Global::eventsSortChanged) {
        //Color
        Global::tagColorCriteria->addCheckStart();
        Global::colorForMeta.clear();
        qreal documentPerColorCount = 0;
        foreach(Document *document, documents) {
            QString colorKey = document->getCriteriaColor();
            if(!colorKey.isEmpty()) {
                if(document->isAcceptableWithColorFilters(false))
                    Global::tagColorCriteria->addCheck(colorKey, document->getCriteriaColorFormated(), "");
                if(document->isAcceptableWithColorFilters(true)) {
                    if(!Global::colorForMeta.contains(colorKey))
                        Global::colorForMeta[colorKey] = QPair<QColor, qreal>(Qt::white, 0);
                    Global::colorForMeta.insert(colorKey, QPair<QColor, qreal>(Global::colorForMeta.value(colorKey).first, Global::colorForMeta.value(colorKey).second + 1));
                    documentPerColorCount++;

                }
            }
        }
        qreal index = 0;
        QMutableMapIterator<QString, QPair<QColor, qreal> > colorForMetaIterator(Global::colorForMeta);
        while(colorForMetaIterator.hasNext()) {
            colorForMetaIterator.next();
            QColor color = Global::getColorScale(index / (qreal)(Global::colorForMeta.count()));
            if(Global::colorForMeta.count() == 7)
                color = Global::getColorScale((index+1)*100);
            colorForMetaIterator.setValue(qMakePair(color, colorForMetaIterator.value().second / documentPerColorCount));
            Global::tagColorCriteria->addCheck(colorForMetaIterator.key(), "", QString("%1%").arg(qRound(colorForMetaIterator.value().second*100), 2, 10, QChar('0')));
            index++;
        }
        foreach(Document *document, documents) {
            QString colorMeta = document->getCriteriaColorFormated();
            if(Global::colorForMeta.contains(colorMeta))            document->baseColor = Global::colorForMeta.value(colorMeta).first;
            else if(document->getFunction() == DocumentFunctionRender)   document->baseColor = Global::colorTagCaptation;
            else                                                    document->baseColor = Global::colorTagDisabled;

        }
        Global::tagColorCriteria->addCheckEnd();


        //Events
        eventsTags.clear();
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if(tag->isAcceptableWithSortFilters(true))
                    eventsTags.append(tag);
        //Sorting events
        qSort(eventsTags.begin(), eventsTags.end(), Tag::sortEvents);

        //Filters
        Global::tagFilterCriteria->addCheckStart();
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if(tag->isAcceptableWithFilterFilters(false))
                    Global::tagFilterCriteria->addCheck(tag->getCriteriaFilter(tag), tag->getCriteriaFilterFormated(tag), "");
        Global::tagFilterCriteria->addCheckEnd();

        //Horizontal
        Global::tagHorizontalCriteria->addCheckStart();
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if(tag->isAcceptableWithHorizontalFilters(false))
                    Global::tagHorizontalCriteria->addCheck(tag->getCriteriaHorizontal(tag), tag->getCriteriaHorizontalFormated(tag), "");
        Global::tagHorizontalCriteria->addCheckEnd();
        Global::ticksChanged = true;


        //Highlight
        Global::tagClusterCriteria->addCheckStart();
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                if(tag->isAcceptableWithClusterFilters(false))
                    Global::tagClusterCriteria->addCheck(tag->getCriteriaCluster(tag), tag->getCriteriaClusterFormated(tag), "");
        Global::tagClusterCriteria->addCheckEnd();


        Global::eventsSortChanged = false;
    }

    if(Global::metaChanged) {
        emit(refreshMetadata());
        Global::metaChanged = false;
    }

    //Opacity
    if((Global::selectedTag) && (Global::timelineGL->showLegendDest))  categoryColorOpacityDest = 0.3;
    else                                                               categoryColorOpacityDest = 1;
    categoryColorOpacity = categoryColorOpacity + (categoryColorOpacityDest - categoryColorOpacity) / Global::inertie;
    QMutableMapIterator<QString, QPair<QColor, qreal> > colorForMetaIterator(Global::colorForMeta);
    while(colorForMetaIterator.hasNext()) {
        colorForMetaIterator.next();
        QColor color = colorForMetaIterator.value().first;
        if((color.alphaF() == 1) && (categoryColorOpacityDest == 1)) {}
        else
            color.setAlphaF(categoryColorOpacity);
        if(Global::selectedTag) {
            if(Tag::getCriteriaColorFormated((Tag*)Global::selectedTag) == colorForMetaIterator.key())
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
            timelineSortTags.clear();
            QMap<QString, QMap<QString, Cluster*> > clustersToLink;
            QMapIterator<QPair<QString, QString>, Cluster*> timelineClustersIterator(timelineClusters);
            while(timelineClustersIterator.hasNext()) {
                timelineClustersIterator.next();
                timelineClustersIterator.value()->clear();
            }

            //Find phases
            if(Global::phases->needCalulation) {
                Global::phases->addCheckStart();
                foreach(Document *document, documents)
                    foreach(Tag *tag, document->tags)
                        Global::phases->addCheck(Tag::getCriteriaPhase(tag));
                Global::phases->addCheckEnd();
            }

            //Browse documents
            Global::tagSortCriteria->addCheckStart();
            quint16 nbDuplicates = 0, nbHistories = 0;
            foreach(Document *document, documents) {
                QList<Tag*> documentHistoryTags;

                //Add tags to list + create history links
                foreach(Tag *tag, document->tags) {
                    //Add to linked tags container for this document
                    if(document->getMetadataCount() > 1)
                        documentHistoryTags.append(tag);

                    //Resets histry
                    tag->historyTags.clear();
                    tag->hashTags.clear();

                    //Add to timeline if displayable
                    if(tag->isAcceptableWithSortFilters(false)) {
                        QString sorting = Tag::getCriteriaSort(tag).toLower();
                        if(tag->isAcceptableWithSortFilters(true)) {
                            QString phase          = Global::phases->getPhaseFor(Tag::getCriteriaPhase(tag)).toLower();
                            QString cluster        = Tag::getCriteriaCluster(tag).toLower();
                            if((!cluster.isEmpty()) && (tag->isAcceptableWithClusterFilters(true)) && (!Global::tagClusterCriteria->getMatchName().isEmpty())) {
                                cluster = tag->getAcceptableWithClusterFilters();
                                QPair<QString,QString> key = qMakePair(sorting, cluster);
                                if(!timelineClusters.contains(key))
                                    timelineClusters.insert(key, new Cluster(this));
                                timelineClusters[key]->add(tag);
                                clustersToLink[cluster][sorting] = timelineClusters.value(key);
                            }
                            timelineSortTags[phase][sorting][cluster].append(tag);
                        }
                        if(tag->getDocument()->getFunction() != DocumentFunctionRender)
                            Global::tagSortCriteria->addCheck(sorting, Tag::getCriteriaSortFormated(tag), "");
                    }
                }

                //More histories with hash detection
                QList<Tag*> hashTags;
                QString documentHash = document->getMetadata("File", "Hash").toString();
                if(!documentHash.isEmpty()) {
                    foreach(Document *documentHashSearch, documents) {
                        if((document != documentHashSearch) && (documentHash == documentHashSearch->getMetadata("File", "Hash").toString())) {
                            nbDuplicates++;
                            foreach(Tag *tagHashToAdd, documentHashSearch->tags) {
                                bool okAddTag = true;
                                foreach(Tag *tag, document->tags)
                                    if(tagHashToAdd->hashTags.contains(tag)) {
                                        okAddTag = false;
                                        break;
                                    }
                                if(okAddTag)
                                    hashTags.append(tagHashToAdd);
                            }
                        }
                    }
                }

                //History tags
                if(documentHistoryTags.count()) {
                    nbHistories++;
                    if(Global::showHistory) {
                        if(Global::tagSortCriteria->isDate()) {
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
                //Add hash tags
                if(Global::showHash)
                    foreach(Tag* tag, document->tags)
                        tag->hashTags.append(hashTags);
            }
            Global::tagSortCriteria->addCheckEnd();

            //Sort clusters
            QMapIterator<QString, QMap<QString, Cluster*> > clustersBySortToLinkIterator(clustersToLink);
            while(clustersBySortToLinkIterator.hasNext()) {
                clustersBySortToLinkIterator.next();
                const Cluster *clusterOld = 0;
                QMapIterator<QString, Cluster*> clustersToLinkIterator(clustersBySortToLinkIterator.value());
                while(clustersToLinkIterator.hasNext()) {
                    clustersToLinkIterator.next();
                    clustersToLinkIterator.value()->setLinkedCluster(clusterOld);
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
                        qSort(timelineSortTags[categoriesInPhasesIterator.key()][clustersInCategoriesIterator.key()][tagsInClusterIterator.key()].begin(), timelineSortTags[categoriesInPhasesIterator.key()][clustersInCategoriesIterator.key()][tagsInClusterIterator.key()].end(), Tag::sortColor);
                    }
                }
            }

            //Update timeline
            Global::timeline->setDuplicates(nbDuplicates/2);
            Global::timeline->setHistories(nbHistories);

            //Unlock
            Global::timelineSortChanged = false;
        }


        //Header
        glEnable(GL_SCISSOR_TEST);
        glPushMatrix();
        glTranslatef(0, Global::timelineHeaderSize.height(), 0);


        //Drawing tags and categories
        bool debug = false; //DEBUG
        QList<QRectF> zones;
        quint16 categoryIndex = 0;
        QPointF tagSortPosOffset = QPointF(0, Global::timelineTagVSpacingSeparator), categoryStart = QPointF(0, 0), phaseStart = QPointF(0, 0);
        guiCategories.clear();
        QMapIterator<QString, QMap<QString, QMap<QString, QList<Tag*> > > > categoriesInPhasesIterator(timelineSortTags);
        while(categoriesInPhasesIterator.hasNext()) {
            categoriesInPhasesIterator.next();

            QString phase        = categoriesInPhasesIterator.key();
            QString phaseVerbose = Global::phases->getVerbosePhaseFor(phase);
            if(debug)
                qDebug("\t > [Phase] |%s| (%s)", qPrintable(phaseVerbose), qPrintable(phase));

            QMapIterator<QString, QMap<QString, QList<Tag*> > > clustersInCategoriesIterator(categoriesInPhasesIterator.value());
            while(clustersInCategoriesIterator.hasNext()) {
                clustersInCategoriesIterator.next();

                bool categoryIsRender = false;

                QString sorting = clustersInCategoriesIterator.key();
                if(debug)
                    qDebug("\t\t > [Sorting] |%s|", qPrintable(sorting));

                //Caching extraction for heatmap
                QRectF tagCategoryRect;

                //New category
                qreal yCategoryMax = 0;
                bool nextCategoryIsRender = false;
                QString tagCategory;
                bool tagCategoryIsSelected = false;

                //Draw tags of this categetory
                QMapIterator<QString, QList<Tag*> > tagsInClusterIterator2(clustersInCategoriesIterator.value());
                while(tagsInClusterIterator2.hasNext()) {
                    tagsInClusterIterator2.next();

                    QString cluster = tagsInClusterIterator2.key();
                    if(debug)
                        qDebug("\t\t\t > [Cluster] |%s| (%d)", qPrintable(cluster), tagsInClusterIterator2.value().count());

                    foreach(Tag *tag, tagsInClusterIterator2.value()) {
                        QRectF tagRect = tag->getTimelineBoundingRect().translated(tagSortPosOffset);
                        QPointF tagPosOffset;

                        //Category in selection
                        tagCategoryIsSelected |= (tag == Global::selectedTag);
                        tagCategoryIsSelected |= (tag == Global::selectedTagHover);
                        tagCategoryIsSelected |= (tag == Global::selectedTagInAction);

                        //Get info about category and analysis
                        if(tagCategory.isEmpty()) {
                            tagCategory = " " + Tag::getCriteriaSortFormated(tag).trimmed().toUpper();
                            if(clustersInCategoriesIterator.hasNext()) {
                                clustersInCategoriesIterator.next();
                                if(clustersInCategoriesIterator.value().count())
                                    if(clustersInCategoriesIterator.value().value(clustersInCategoriesIterator.value().keys().first()).count())
                                        if(clustersInCategoriesIterator.value().value(clustersInCategoriesIterator.value().keys().first()).first()->getDocument()->getFunction() == DocumentFunctionRender)
                                            nextCategoryIsRender = true;
                                clustersInCategoriesIterator.previous();
                            }
                            categoryIsRender = (tag->getDocument()->getFunction() == DocumentFunctionRender);
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
                                qreal maxWidth = Global::timelineGlobalDocsWidth - 2*Global::timelineTagHeight;
                                if(!Global::tagHorizontalCriteria->isTimeline())
                                    maxWidth = 5 * Global::timeUnit - 3 * Global::timelineTagHeight;

                                if(((tag->getType() == TagTypeGlobal) && (Global::tagHorizontalCriteria->isTimeline())) || (!Global::tagHorizontalCriteria->isTimeline())) {
                                    if(tagPosOffset.x() < maxWidth) {
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
                                tag->setTimelinePos(tagPosOffset + tagSortPosOffset + QPointF(Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth, 0));
                        }
                        //Drawing
                        retour = retour.united(tag->paintTimeline(before));
                        zones.append(tagRect);
                        yCategoryMax = qMax(yCategoryMax, tagRect.bottom());
                    }
                }

                //Extract category name and rect, store it for caching
                tagCategoryRect = QRectF(categoryStart, QPointF(Global::timelineHeaderSize.width(), yCategoryMax + Global::timelineTagVSpacingSeparator)).translated(Global::timelineGL->scroll.x(), 0);
                //timelineCategoriesRectCache[categoryIndex] = qMakePair(tagCategoryRect, nbTagsPerCategory);
                tagCategoryRect.setWidth(tagCategoryRect.width() - 2);

                //Draw category background
                if(Global::timelineGL->visibleRect.intersects(tagCategoryRect.translated(QPointF(0, Global::timelineHeaderSize.height())))) {
                    glScissor(0, 0, Global::timelineGL->width(), Global::timelineGL->height() - Global::timelineHeaderSize.height());
                    if(categoryIsRender) {
                        Global::timelineGL->qglColor(Global::colorAlternate);
                        GlRect::drawRect(tagCategoryRect);
                        //Global::timelineGL->qglColor(Global::colorBackgroundDark);
                        //GlRect::drawRect(QRectF(tagCategoryRect.topLeft(), QSizeF(Global::timelineGL->width(), tagCategoryRect.height())));
                    }
                    else {
                        Global::timelineGL->qglColor(Global::colorBackground);
                        GlRect::drawRect(tagCategoryRect);
                        if(categoryIndex%2) Global::timelineGL->qglColor(Global::colorAlternate);
                        else                Global::timelineGL->qglColor(Global::colorAlternateLight);
                        GlRect::drawRect(QRectF(tagCategoryRect.topLeft(), QSizeF(Global::timelineGL->width(), tagCategoryRect.height())));
                    }

                    //Draw text
                    if(tagCategoryIsSelected)   Global::timelineGL->qglColor(Global::colorTimeline);
                    else                        Global::timelineGL->qglColor(Global::colorText);
                    bool textFound = false;
                    if(timelineCategories.count() > 1000)
                        timelineCategories.clear();
                    foreach(GlText timelineCategory, timelineCategories) {
                        if((timelineCategory.text == tagCategory) && (timelineCategory.size == (tagCategoryRect.size().toSize() - QSize(20, 0)))) {
                            timelineCategory.drawText(tagCategoryRect.topLeft().toPoint() + QPoint(20, 0));
                            textFound = true;
                        }
                    }
                    if(!textFound) {
                        GlText tagCategoryText;
                        tagCategoryText.setStyle(tagCategoryRect.size().toSize() - QSize(20, 0), Qt::AlignVCenter, Global::font);
                        tagCategoryText.drawText(tagCategory, tagCategoryRect.topLeft().toPoint() + QPoint(20, 0));
                        timelineCategories << tagCategoryText;
                    }
                    glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height() - Global::timelineHeaderSize.height());
                }
                guiCategories.append(qMakePair(tagCategoryRect.translated(QPointF(0, Global::timelineHeaderSize.height())), qMakePair(phase, sorting)));

                //Super separator si big change of category
                qreal vSpacing = Global::timelineTagVSpacingSeparator+1;
                if(((categoryIsRender) && (!nextCategoryIsRender)) || ((!categoryIsRender) && (nextCategoryIsRender)))
                    vSpacing = Global::timelineTagVSpacingSeparator+1;
                if((!clustersInCategoriesIterator.hasNext()) && (categoriesInPhasesIterator.hasNext()))
                    vSpacing = 10;

                //Super category
                if(vSpacing > Global::timelineTagVSpacingSeparator) {
                    tagCategoryRect = QRectF(QPointF(0, yCategoryMax + 2*Global::timelineTagVSpacing+1), QPointF(Global::timelineGL->width(), yCategoryMax + vSpacing)).translated(Global::timelineGL->scroll.x(), 0);

                    /*
                    glDisable(GL_SCISSOR_TEST);
                    Global::timelineGL->qglColor(Global::colorBackgroundDark);
                    textureStrips.drawTexture(tagCategoryRect, -4);
                    glEnable(GL_SCISSOR_TEST);
                    */
                }

                //New offset
                tagSortPosOffset.setY(yCategoryMax + vSpacing + Global::timelineTagVSpacingSeparator);
                categoryStart   .setY(yCategoryMax + vSpacing);
                categoryIndex++;
            }

            if(timelineSortTags.count() > 1) {
                QRectF phaseRect = QRectF(phaseStart, QPointF(12, categoryStart.y() - Global::timelineTagVSpacingSeparator)).translated(Global::timelineGL->scroll.x(), 0);
                glScissor(0, 0, Global::timelineGL->width(), Global::timelineGL->height() - Global::timelineHeaderSize.height());
                Global::timelineGL->qglColor(Global::colorAlternateMore);
                GlRect::drawRect(phaseRect);

                Global::timelineGL->qglColor(Qt::white);
                glPushMatrix();
                QSize tagPhaseTextSize(phaseRect.size().height(), phaseRect.size().width());
                glTranslatef(phaseRect.bottomLeft().x(), phaseRect.bottomLeft().y(), 0);
                glRotatef(-90, 0, 0, 1);
                bool textFound = false;
                if(timelinePhases.count() > 1000)
                    timelinePhases.clear();
                foreach(GlText timelinePhase, timelinePhases) {
                    if((timelinePhase.text == phaseVerbose) && (timelinePhase.size == tagPhaseTextSize)) {
                        timelinePhase.drawText();
                        textFound = true;
                    }
                }
                if(!textFound) {
                    GlText tagPhaseText;
                    tagPhaseText.setStyle(tagPhaseTextSize, Qt::AlignCenter, Global::font);
                    tagPhaseText.drawText(phaseVerbose);
                    timelinePhases << tagPhaseText;
                }
                glPopMatrix();
                glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height() - Global::timelineHeaderSize.height());
            }
            phaseStart.setY(categoryStart.y());
        }

        //Drawing clusters
        if(!Global::tagClusterCriteria->getTagName().isEmpty()) {
            QMapIterator<QPair<QString, QString>, Cluster*> timelineClustersIterator(timelineClusters);
            while(timelineClustersIterator.hasNext()) {
                timelineClustersIterator.next();
                timelineClustersIterator.value()->paintTimeline();
            }
        }

        glPopMatrix();
        glDisable(GL_SCISSOR_TEST);
    }
    else {
        foreach(Document *document, documents)
            foreach(Tag *tag, document->tags)
                tag->paintTimeline(before);
    }
    glScissor(Global::timelineHeaderSize.width(), 0, Global::timelineGL->width() - Global::timelineHeaderSize.width(), Global::timelineGL->height());
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
                    if(tag->isAcceptableWithSortFilters(true))
                        if((tag->getDocument()->getFunction() == DocumentFunctionContextual) && (tag->getType() != TagTypeGlobal))
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


const QPointF Project::getTimelineCursorPos(qreal time) const {
    //Linear cursor
    return QPointF(Global::timelineHeaderSize.width() + Global::timelineGlobalDocsWidth + Global::timeUnit * time, 0);
}
qreal Project::getTimelineCursorTime(const QPointF &pos) const {
    qreal cursorTime = qMax(0., (pos.x() - Global::timelineHeaderSize.width() - Global::timelineGlobalDocsWidth) / Global::timeUnit);
    Global::selectedTagHoverSnapped.first = Global::selectedTagHoverSnapped.second = -1;
    if((Global::selectedTagHover) && (Global::selectedTagHover != Global::selectedTagInAction))
        ((Tag*)Global::selectedTagHover)->snapTime(&cursorTime);
    return cursorTime;
}

const QPointF Project::getViewerCursorPos(qreal time) const {
    //Unlinear cursor
    Tag *lastTag = 0;
    for(qint16 tagIndex = 0 ; tagIndex < viewerTags.count() ; tagIndex++) {
        if((tagIndex < viewerTags.count()-1) && (viewerTags.at(tagIndex)->getTimeStart() <= time) && (time <= viewerTags.at(tagIndex+1)->getTimeStart())) {
            qreal viewCurrentPos = (time - viewerTags.at(tagIndex)->getTimeStart()) / (viewerTags.at(tagIndex+1)->getTimeStart() - viewerTags.at(tagIndex)->getTimeStart());
            viewCurrentPos = viewerTags.at(tagIndex)->viewerPos.y() * (1-viewCurrentPos) + viewerTags.at(tagIndex+1)->viewerPos.y() * (viewCurrentPos);
            return QPointF(0, viewCurrentPos);
            break;
        }
        if((!lastTag) || (lastTag->getTimeEnd() < viewerTags.at(tagIndex)->getTimeEnd()))
            lastTag = viewerTags.at(tagIndex);
    }
    if(viewerTags.count()) {
        if(Global::time < viewerTags.first()->getTimeStart())
            return QPointF(0, (Global::time / viewerTags.first()->getTimeStart()) * viewerTags.first()->viewerPos.y());
        else if(lastTag) {
            qreal viewCurrentPos = (time - viewerTags.last()->getTimeStart()) / (lastTag->getTimeEnd() - viewerTags.last()->getTimeStart());
            viewCurrentPos = viewerTags.last()->viewerPos.y() * (1-viewCurrentPos) + (viewerTags.last()->viewerPos.y() + viewerTags.last()->getViewerBoundingRect().height()) * (viewCurrentPos);
            return QPointF(0, viewCurrentPos);
        }
    }
    return QPointF();
}
qreal Project::getViewerCursorTime(const QPointF &pos) const {
    //Unlinear cursor
    qreal time = 0;
    foreach(Tag *tag, viewerTags)
        if(tag->viewerContains(pos))
            time = tag->getTimeStart();

    return time;
}

bool Project::mouseTimeline(const QPointF &pos, QMouseEvent *e, bool dbl, bool stay, bool action, bool press) {
    bool mouseOnTag = false;
    Global::selectedTagHover = 0;
    QMapIterator<QString, QMap<QString, QMap<QString, QList<Tag*> > > > categoriesInPhasesIterator(timelineSortTags);
    while(categoriesInPhasesIterator.hasNext()) {
        categoriesInPhasesIterator.next();
        QMapIterator<QString, QMap<QString, QList<Tag*> > > clustersInCategoriesIterator(categoriesInPhasesIterator.value());
        while(clustersInCategoriesIterator.hasNext()) {
            clustersInCategoriesIterator.next();
            QMapIterator<QString, QList<Tag*> > tagsInClusterIterator(clustersInCategoriesIterator.value());
            while(tagsInClusterIterator.hasNext()) {
                tagsInClusterIterator.next();
                foreach(Tag *tag, tagsInClusterIterator.value())
                    mouseOnTag |= tag->mouseTimeline(pos, e, dbl, stay, action, press);
            }
        }
    }

    if(!mouseOnTag) {
        Global::selectedTagInAction = 0;
        Global::selectedTagHover    = 0;
        if(press) {
            for(quint16 i = 0 ; i < guiCategories.count() ; i++) {
                if(guiCategories.at(i).first.contains(pos)) {
                    QList<Tag*> actionTags;
                    QMapIterator<QString, QList<Tag*> > tagsInClusterIterator(timelineSortTags.value(guiCategories.at(i).second.first).value(guiCategories.at(i).second.second));
                    while(tagsInClusterIterator.hasNext()) {
                        tagsInClusterIterator.next();
                        foreach(Tag *tag, tagsInClusterIterator.value())
                            actionTags << tag;
                    }
                    if(actionTags.count()) {
                        timelineFilesMenu->clear();
                        qSort(actionTags.begin(), actionTags.end(), Tag::sortColor);
                        QString lastCriteria;
                        foreach(Tag *tag, actionTags) {
                            QPixmap icon(16, 16);
                            icon.fill(Qt::transparent);
                            QPainter painter(&icon);
                            painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
                            painter.setBrush(tag->getRealTimeColor());
                            painter.setPen(Qt::transparent);
                            painter.drawEllipse(icon.rect().center(), icon.width()/4, icon.height()/4);
                            painter.end();
                            QString criteriaColor = Tag::getCriteriaColor(tag);
                            if(criteriaColor != lastCriteria) {
                                if(!lastCriteria.isEmpty())
                                    timelineFilesMenu->addSeparator();
                                QAction *action = timelineFilesMenu->addAction(Tag::getCriteriaColorFormated(tag));
                                action->setEnabled(false);
                            }
                            tag->timelineFilesAction = timelineFilesMenu->addAction(QIcon(icon), tag->getDocument()->getName(tag->getDocumentVersion()));
                            lastCriteria = criteriaColor;
                        }
                        QAction *retour = timelineFilesMenu->exec(QCursor::pos());
                        if(retour) {
                            QMapIterator<QString, QList<Tag*> > tagsInClusterIterator2(timelineSortTags.value(guiCategories.at(i).second.first).value(guiCategories.at(i).second.second));
                            while(tagsInClusterIterator2.hasNext()) {
                                tagsInClusterIterator2.next();
                                foreach(Tag *tag, tagsInClusterIterator2.value()) {
                                    if(retour == tag->timelineFilesAction) {
                                        if(tag->getDocument()->chutierItem) {
                                            Global::chutier->setCurrentItem(tag->getDocument()->chutierItem);
                                            Global::timelineGL->ensureVisible(tag->getTimelineBoundingRect().translated(tag->timelineDestPos).topLeft());
                                            Global::viewerGL  ->ensureVisible(tag->getViewerBoundingRect()  .translated(tag->viewerDestPos)  .topLeft());
                                        }
                                    }
                                }
                            }
                        }
                        return true;
                    }
                }
            }
        }
    }
    return mouseOnTag;
}
bool Project::mouseViewer(const QPointF &pos, QMouseEvent *e, bool dbl, bool stay, bool action, bool press) {
    bool mouseOnTag = false;
    foreach(Tag *tag, viewerTags)
        mouseOnTag |= tag->mouseViewer(pos, e, dbl, stay, action, press);

    if(!mouseOnTag) {
        Global::selectedTagInAction = 0;
        Global::selectedTag         = 0;
    }
    return mouseOnTag;
}




QDomElement Project::serialize(QDomDocument &xmlDoc) const {
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
