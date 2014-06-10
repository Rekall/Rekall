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

#include "rekall.h"
#include "ui_rekall.h"

Rekall::Rekall(QWidget *parent) :
    RekallBase(parent),
    ui(new Ui::Rekall) {
    ui->setupUi(this);
    setAcceptDrops(true);
    currentProject = 0;
    annotationTag  = 0;

    annotationIsUpdating = chutierIsUpdating = metadataIsUpdating = false;

    Global::userInfos = new UserInfos();

    Global::mainWindow = this;
    QApplication::setQuitOnLastWindowClosed(false);

    Watcher *watcher  = new Watcher(this);
    Global::watcher   = watcher;
    Global::taskList->setToolbox(ui->toolBoxLeft);
    Global::feedList->setToolbox(ui->toolBoxLeft);

    Global::chutier = ui->chutier->getTree();
    Global::udp = new Udp(0, 5678);
    settings = new QSettings(Global::pathApplication.absoluteFilePath() + "/Rekall.ini", QSettings::IniFormat, this);
    if(true) {
        httpUpload = new FileUploadController();
        settings->beginGroup("listener");
        http = new HttpListener(settings, httpUpload, this);
        connect(httpUpload, SIGNAL(fileUploaded(QString,QString,QString)), SLOT(fileUploaded(QString,QString,QString)));
    }

    Global::font.setFamily("Calibri");
    Global::font.setPixelSize(11);
    Global::fontSmall.setFamily("Calibri");
    Global::fontSmall.setPixelSize(10);

    UiFileItem::configure(ui->chutier, false);
    UiFileItem::forbiddenDirs << "rekall_cache";

    currentProject = new Project(this);
    Global::currentProject = currentProject;
    connect(currentProject, SIGNAL(displayMetadata()), SLOT(displayMetadata()));

    Global::groupes               = new Sorting(tr("Group by documents"),           11);
    Global::groupes->allowEmptyCriterias = true;
    Global::tagFilterCriteria     = new Sorting(tr("Filter by documents"),           6);
    Global::tagSortCriteria       = new Sorting(tr("Sort by documents"),             2);
    Global::tagColorCriteria      = new Sorting(tr("Color depends on documents"),    5);
    Global::tagTextCriteria       = new Sorting(tr("Text on timeline is documents"), 0);
    Global::tagClusterCriteria    = new Sorting(tr("Hightlight on documents"),       7, true);
    Global::tagHorizontalCriteria = new Sorting(tr("Horizontal scale is documents"), 1, false, true);
    Global::tagClusterCriteria->init();
    ui->timeline->timelineControl->init();


    QString styleAdditionnal = "QWidget#Sorting, QWidget#Phases, QWidget#TimelineControl { background-color: transparent;}";
    Global::tagFilterCriteria    ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::tagSortCriteria      ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::tagColorCriteria     ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::tagTextCriteria      ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::tagClusterCriteria   ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::tagHorizontalCriteria->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::groupes               ->setStyleSheet(styleSheet() + styleAdditionnal);
    Global::groupes               ->setStyleSheet2(Global::tagFilterCriteria->styleSheet2());
    ui->timeline->timelineControl->setStyleSheet(styleSheet() + styleAdditionnal);
    ui->timeline->timelineControl->setStyleSheet2(Global::tagFilterCriteria->styleSheet2());

    connect(Global::groupes,                SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagFilterCriteria,     SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagSortCriteria,       SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagColorCriteria,      SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagTextCriteria,       SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagClusterCriteria,    SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(Global::tagHorizontalCriteria, SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));
    connect(ui->timeline->timelineControl, SIGNAL(displayed(bool)), (Timeline*)Global::timeline, SLOT(actionDisplayed(bool)));

    connect(Global::groupes,                SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagFilterCriteria,     SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagSortCriteria,       SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagColorCriteria,      SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagTextCriteria,       SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagClusterCriteria,    SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    connect(Global::tagHorizontalCriteria, SIGNAL(actionned(QString,QString)), (Timeline*)Global::timeline, SLOT(actionChanged(QString,QString)));
    Global::groupes               ->action();
    Global::tagFilterCriteria    ->action();
    Global::tagSortCriteria      ->action();
    Global::tagColorCriteria     ->action();
    Global::tagTextCriteria      ->action();
    Global::tagClusterCriteria   ->action();
    Global::tagHorizontalCriteria->action();


    Global::showHelp.setAction(ui->actionInlineHelp);
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionOpenRandom, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionOpenRecentClear, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionClose, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionPaste, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionRemove, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionMarker, SIGNAL(triggered(bool)), SLOT(action()));
    connect(ui->actionMarkerLong, SIGNAL(triggered(bool)), SLOT(action()));
    connect(ui->actionSRT, SIGNAL(triggered(bool)), SLOT(action()));
    connect(&Global::showHelp, SIGNAL(triggered(bool)), SLOT(showHelp(bool)));

    ui->metadata->setColumnWidth(0, 135);
    ui->metadataSlider->setVisible(false);
    ui->metadataSliderIcon ->setVisible(ui->metadataSlider->isVisible());
    ui->metadataSliderIcon2->setVisible(ui->metadataSlider->isVisible());
    HtmlDelegate *delegate0 = new HtmlDelegate(false);
    HtmlDelegate *delegate1 = new HtmlDelegate(true);
    connect(delegate1, SIGNAL(closeEditor(QWidget*)), SLOT(actionMetadata()));
    ui->metadata->setItemDelegateForColumn(0, delegate0);
    ui->metadata->setItemDelegateForColumn(1, delegate1);
    //ui->metadata->setItemDelegateForColumn(options.index, new UiTreeDelegate(options, ui->view->model(), this));

    ui->chutier->showNew(false);
    ui->chutier->showImport(false);
    ui->chutier->showRemove(false);
    ui->chutier->showDuplicate(false);
    connect(ui->chutier->getTree(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(chutierItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->chutier->getTree(), SIGNAL(itemSelectionChanged()), SLOT(chutierItemChanged()));
    connect(ui->actionPlay, SIGNAL(triggered()), ui->timeline, SLOT(actionPlay()));
    connect(ui->actionRewind, SIGNAL(triggered()), ui->timeline, SLOT(actionRewind()));

    personsTreeWidget = ui->persons;
    personsTreeWidget->setColumnWidth(0, 40);
    personsTreeWidget->setColumnWidth(1, 100);
    personsTreeWidget->setColumnWidth(2, 100);
    personsTreeWidget->setColumnWidth(3, 100);


    //GPS
    ui->metadataOpenGps->setVisible(false);
    ui->metadataOpenFinder->setVisible(false);
    ui->metadataOpen->setVisible(false);
    gps = new QWebView(0);
    gps->setWindowFlags(Qt::Tool);
    gps->load(QUrl::fromLocalFile(Global::pathApplication.absoluteFilePath() + "/tools/maps.html"));


    //Display splash
    splash = new Splash();
    splash->show();
    QTimer::singleShot(1500, this, SLOT(closeSplash()));
    QCoreApplication::processEvents();

    refreshMenus();

    startTimer(1000);
}

Rekall::~Rekall() {
    settings->sync();
    delete ui;
}

void Rekall::fileUploaded(const QString &gpsCoord, const QString &filename, const QString &file) {
    QFileInfo fileInfo(file);
    qDebug("Upload %s @ %s = %s (%d)", qPrintable(gpsCoord), qPrintable(filename), qPrintable(fileInfo.absoluteFilePath()), fileInfo.exists());
    Document *document = new Document(Global::currentProject);
    document->updateFile(fileInfo);

    displayDocumentName(tr("Imported file"));
    displayPixmap(DocumentTypeImage, document->file.fileName(), QPixmap(document->file.fileName()));
    displayGps(QList< QPair<QString,QString> >() << qMakePair(gpsCoord, tr("Imported file")));
}

void Rekall::dragEnterEvent(QDragEnterEvent *event) {
    if(parseMimeData(event->mimeData(), "rekall", true))
        return event->acceptProposedAction();
}
void Rekall::dropEvent(QDropEvent *event) {
    if(parseMimeData(event->mimeData(), "rekall"))
        return event->acceptProposedAction();
}
bool Rekall::parseMimeData(const QMimeData *mime, const QString &source, bool test) {
    bool retour = false;
    QFileInfoList    droppedFiles;
    QList<Document*> droppedDocuments;
    QList<Person*>   droppedPersons;

    if(mime->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray encoded = mime->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        while(!stream.atEnd()) {
            int row, col;
            QMap<int,  QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;
            QMapIterator<int, QVariant> roleDataMapIterator(roleDataMap);
            while(roleDataMapIterator.hasNext()) {
                roleDataMapIterator.next();
                QFileInfo file = QFileInfo(roleDataMapIterator.value().toString());
                if((col == 2) && (file.exists())) {
                    droppedFiles << file;
                    Document *document = currentProject->getDocument(file.absoluteFilePath());
                    if(document)
                        droppedDocuments << document;
                }
            }
        }
    }
    foreach(const QUrl &url, mime->urls()) {
        QFileInfo file(url.toLocalFile());
        if(file.exists()) {
            if(file.suffix().toLower() == "vcf")
                droppedPersons.append(Person::fromFile(file.absoluteFilePath(), Global::mainWindow));
            else {
                Document *document = currentProject->getDocument(file.absoluteFilePath());
                if(document)    droppedDocuments << document;
                else            droppedFiles     << file;
            }
        }
    }
    foreach(Document *droppedDocument, droppedDocuments)
        droppedFiles.removeOne(droppedDocument->file);
    if(mime->hasFormat("text/plain"))
        droppedPersons.append(Person::fromString(mime->data("text/plain"), Global::mainWindow));

    qDebug("--------------- %s -------------------", qPrintable(source));
    qDebug("Texte > %s", qPrintable(mime->text()));
    qDebug("Html  > %s", qPrintable(mime->html()));
    foreach(const QUrl &url, mime->urls())
        qDebug("URL> %s", qPrintable(url.toString()));
    foreach(const QString &format, mime->formats())
        qDebug("Data %s > %s", qPrintable(format), qPrintable(QString(mime->data(format))));
    foreach(const QFileInfo &file, droppedFiles)
        qDebug("File item  > %s", qPrintable(file.absoluteFilePath()));
    foreach(Document *document, droppedDocuments)
        qDebug("Document item > %s", qPrintable(document->file.absoluteFilePath()));
    foreach(Person *person, droppedPersons)
        qDebug("Person > %s", qPrintable(person->getFullname()));
    qDebug("--------------- %s -------------------", qPrintable(source));



    if(source == "rekall") {
        if(test) {
            if(droppedPersons.count())
                retour = true;
        }
        else {
            foreach(Person *droppedPerson, droppedPersons) {
                currentProject->addPerson(droppedPerson);
                retour = true;
            }
        }
    }
    else if(source == "timeline") {
        if(test) {
            if((droppedDocuments.count()) || (droppedFiles.count()))
                retour = true;
        }
        else {
            if(droppedFiles.count()) {
                chutierIsUpdating = metadataIsUpdating = true;
                if(!currentProject->open(droppedFiles, ui->chutier))
                    ui->timeline->setWorkspace(3);
                chutierIsUpdating = metadataIsUpdating = false;
            }

            foreach(const QFileInfo &droppedFile, droppedFiles) {
                Document *document = currentProject->getDocument(droppedFile.absoluteFilePath());
                if(document)
                    droppedDocuments << document;
            }
            foreach(Document *droppedDocument, droppedDocuments) {
                droppedDocument->createTag(TagTypeContextualTime, currentProject->getTimelineCursorTime(Global::timelineGL->mapFromGlobal(QCursor::pos()) + Global::timelineGL->scroll), 5);
                if(droppedDocument->chutierItem)
                    Global::chutier->setCurrentItem(droppedDocument->chutierItem);
                retour = true;
            }
            Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
            //Global::groupes->needCalulation = true;
        }
    }
    return retour;
}





void Rekall::action() {
    if((sender() == 0) || (sender() == ui->actionOpen) || (sender() == ui->actionOpenRandom) || (sender() == ui->actionSave) || (sender() == ui->actionClose) || (openRecentAction.contains((QAction*)sender()))) {
        QString dirToOpen;
        bool shouldClose = false;
        if(sender() == ui->actionSave) {
            currentProject->save();
        }
        else if(sender() == ui->actionClose)
            shouldClose = true;
        else {
            Global::falseProject = (sender() == ui->actionOpenRandom);
            if(openRecentAction.contains((QAction*)sender()))
                dirToOpen = ((QAction*)sender())->text();
            else if(sender())
                dirToOpen = QFileDialog::getExistingDirectory(0, tr("Select a folder where you store your files"), Global::pathCurrentDefault.absoluteFilePath());
            if((!dirToOpen.isEmpty()) && (QFileInfo(dirToOpen).exists()))
                shouldClose = true;
        }

        if(shouldClose) {
            currentMetadatas.clear();
            currentProject->close();
            ui->chutier->getTree()->clear();
            Global::pathCurrent = QFileInfo();
            displayMetadata();
            ui->player->unload();
        }

        if((!dirToOpen.isEmpty()) && (QFileInfo(dirToOpen).exists())) {
            chutierIsUpdating = metadataIsUpdating = true;
            Global::pathCurrent = QFileInfo(dirToOpen);
            refreshMenus(Global::pathCurrent);
            if(!currentProject->open(QFileInfoList() << Global::pathCurrent, ui->chutier))
                ui->timeline->setWorkspace(3);
            ui->chutier->getTree()->collapseAll();
            for(quint16 i = 0 ; i < ui->chutier->getTree()->topLevelItemCount() ; i++)
                ui->chutier->getTree()->expandItem(ui->chutier->getTree()->topLevelItem(i));
            chutierIsUpdating = metadataIsUpdating = false;
        }
        refreshMenus();
    }
    else if(sender() == ui->actionOpenRecentClear)
        refreshMenus(QFileInfo(), true);
    else if(sender() == ui->actionPaste)
        parseMimeData(QApplication::clipboard()->mimeData(), "rekall");
    else if(sender() == ui->metadataOpenGps)
        gps->show();
    else if((sender() == ui->metadataOpen) && (currentMetadatas.count())) {
        foreach(Metadata *currentMetadata, currentMetadatas) {
            if(currentMetadata->chutierItem)
                currentMetadata->chutierItem->fileShowInOS();
            else
                UiFileItem::fileShowInOS(currentMetadata->getMetadata("Rekall", "URL").toString());
        }
    }
    else if((sender() == ui->metadataOpenFinder) && (currentMetadatas.count())) {
        foreach(Metadata *currentMetadata, currentMetadatas) {
            if(currentMetadata->chutierItem)
                currentMetadata->chutierItem->fileShowInFinder();
            else if(currentMetadata->thumbnails.count())
                UiFileItem::fileShowInFinder(currentMetadata->thumbnails.first().currentFilename);
        }
    }
    else if(sender() == ui->actionMarker) {
        Global::timeline->actionMarkerAddStart();
        Global::timeline->actionMarkerAddEnd();
    }
    else if(sender() == ui->actionMarkerLong) {
        Global::timeline->actionMarkerAddStart();
    }
    else if(sender() == ui->actionSRT) {
        QString fileToOpen = QFileDialog::getOpenFileName(0, tr("Select a SRT file"), Global::pathCurrentDefault.absoluteFilePath(), tr("Subtitles files (*.srt)"));
        QFile file(fileToOpen);
        if((!fileToOpen.isEmpty()) && (file.exists()) && (file.open(QFile::ReadOnly))) {
            QTextStream in(&file);
            int state = 0;
            quint16 srtIndex = 0;
            qreal srtStart = -1, srtEnd = -1;
            QString srtText;
            while(!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if(state == 0) {
                    srtIndex = line.toInt();
                    srtText  = "";
                    srtStart = -1;
                    srtEnd   = -1;
                    state = 1;
                }
                else if(state == 1) {
                    QStringList times = line.split(" --> ", QString::SkipEmptyParts);
                    if(times.count() >= 2) {
                        srtStart = Global::getDurationFromString(times.at(0));
                        srtEnd   = Global::getDurationFromString(times.at(1));
                    }
                    state = 2;
                }
                else if(state == 2) {
                    if(line.isEmpty()) {
                        srtText.chop(3);
                        state = 0;
                        Document *marker = new Document(Global::currentProject);
                        marker->updateImport(QString("%1 : %2").arg(srtIndex).arg(srtText));
                        marker->createTag(TagTypeContextualTime, srtStart, srtEnd - srtStart);
                        marker->updateFeed();
                    }
                    else
                        srtText += line + " | ";
                }
            }
            file.close();
            Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
        }
    }
    else if(sender() == ui->annotation) {
        if((!annotationIsUpdating) && (annotationTag)) {
            annotationTag->getDocument()->setMetadata("Rekall", "Name", ui->annotation->text(), annotationTag->getDocumentVersion());
            Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
        }
    }
    else if(sender() == ui->actionRemove) {
        foreach(void* selectedTag, Global::selectedTags) {
            Tag *tag = (Tag*)selectedTag;
            tag->getDocument()->removeTag(tag);
        }
        Global::selectedTagHover = 0;
        Global::selectedTags.clear();
        Global::selectedTagsInAction.clear();
        Global::mainWindow->displayMetadataAndSelect();
        Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = true;
        //Global::groupes->needCalulation = true;
    }
}
void Rekall::annotationFocusChanged(bool val) {
    if(val) {
        annotationStateBeforeFocus = Global::timerPlay;
        Global::play(false);
    }
    else {
        Global::play(annotationStateBeforeFocus);
    }
}
void Rekall::annotationFinished() {
    ui->timeline->setFocus();
}
void Rekall::changeAnnotation(void *_tag, bool giveFocus) {
    annotationIsUpdating = true;
    annotationTag = (Tag*)_tag;
    if(annotationTag) {
        ui->annotation->setText(annotationTag->getDocument()->getName(annotationTag->getDocumentVersion()));
        //ui->annotation->setStyleSheet(QString("color: %1;").arg(annotationTag->getRealTimeColor().name()));
        ui->annotation->setEnabled(true);
        if(giveFocus) {
            ui->annotation->setFocus();
            ui->annotation->selectAll();
        }
    }
    else {
        ui->annotation->setText("");
        ui->annotation->setEnabled(false);
    }
    annotationIsUpdating = false;
}

void Rekall::actionForceGL() {
    ui->toolBoxRight->resize(ui->toolBoxRight->width()+1, ui->toolBoxRight->height());
}
qint16 Rekall::findDocumentVersionWithMetadata(Metadata* metadata) {
    if(ui->metadataSlider->isVisible())
        return ui->metadataSlider->value();
    else if(metadata->tempStorage)
        return ((Tag*)metadata->tempStorage)->getDocumentVersion();
    return -1;
}
void Rekall::actionMetadata() {
    if((ui->metadata->currentItem()) && (ui->metadata->currentItem()->parent())) {
        QString key      = HtmlDelegate::removeHtml(ui->metadata->currentItem()->text(0));
        QString value    = HtmlDelegate::removeHtml(ui->metadata->currentItem()->text(1));
        QString category = HtmlDelegate::removeHtml(ui->metadata->currentItem()->parent()->text(0));
        if(category == "General")   category = "Rekall";
        foreach(Metadata *currentMetadata, currentMetadatas)
            currentMetadata->setMetadata(category, key, value, findDocumentVersionWithMetadata(currentMetadata));
        ui->metadata->currentItem()->setText(0, ui->metadata->currentItem()->text(0).remove("<i>").remove("</i>"));
        ui->metadata->currentItem()->setText(1, ui->metadata->currentItem()->text(1).remove("<i>").remove("</i>"));
    }
    Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::metaChanged = true;
    //Global::groupes->needCalulation = true;
}

void Rekall::closeSplash() {
    splash->close();
    showMaximized();
    updateGeometry();
    ui->timelineSplitter  ->setSizes(QList<int>() << ui->timelineSplitter->height() * 0.50 << ui->timelineSplitter->height() * 0.50);
    ui->fileSplitter      ->setSizes(QList<int>() << 200                                   << ui->fileSplitter->width()      - 200);
    ui->conduiteSplitter  ->setSizes(QList<int>() << ui->conduiteSplitter->width()  - 300  << 300);
    ui->previewSplitter   ->setSizes(QList<int>() << ui->previewSplitter->height()    * 0.30  << ui->previewSplitter->height() * 0.70);
    ui->annotationSplitter->setSizes(QList<int>() << ui->annotationSplitter->height() - 35 << 35);
    //trayMenu->showMessage("Rekall", "Ready!", QSystemTrayIcon::NoIcon);
}



void Rekall::chutierItemChanged(QTreeWidgetItem *, QTreeWidgetItem *) {
    if(!chutierIsUpdating) {
        QList<QTreeWidgetItem*> items = ui->chutier->getTree()->selectedItems();
        Global::selectedTags.clear();
        foreach(QTreeWidgetItem *item, items) {
            Document *document = currentProject->getDocument(((UiFileItem*)item)->filename.file.absoluteFilePath());
            if(document) {
                foreach(Tag *tag, document->tags) {
                    if(tag->getDocumentVersion() == document->getMetadataCountM()) {
                        Global::selectedTags.append(tag);
                        Global::timelineGL->ensureVisible(tag->getTimelineBoundingRect().translated(tag->timelineDestPos).topLeft());
                        Global::viewerGL  ->ensureVisible(tag->getViewerBoundingRect().translated(tag->viewerDestPos).topLeft());
                    }
                }
            }
        }
        displayMetadata();
    }
}
void Rekall::personItemChanged(QTreeWidgetItem *, QTreeWidgetItem *) {
    displayMetadata();
}
void Rekall::displayMetadataAndSelect(void *tag) {
    chutierIsUpdating = true;
    ui->chutier->getTree()->clearSelection();
    if(tag) {
        DocumentBase *metadata = ((Tag*)tag)->getDocument();
        if(metadata) {
            UiFileItem *chutierItem = metadata->chutierItem;
            if(chutierItem)
                ui->chutier->getTree()->setCurrentItem(chutierItem);
        }
    }
    chutierIsUpdating = false;
    displayMetadata();
}
void Rekall::displayMetadata(QTreeWidgetItem *, QTreeWidgetItem *) {
    currentMetadatas.clear();

    chutierIsUpdating = true;

    //Ajoute la sélection de tags
    foreach(void *tag, Global::selectedTags) {
        Metadata *metadata = ((Tag*)tag)->getDocument();
        if((metadata) && (!currentMetadatas.contains(metadata))) {
            UiFileItem *chutierItem = ((Document*)metadata)->chutierItem;
            if(chutierItem) {
                //ui->chutier->getTree()->setCurrentItem(chutierItem);
                ((Document*)metadata)->chutierItem->setSelected(true);
            }

            metadata->tempStorage = tag;
            currentMetadatas << metadata;
        }
    }

    //Ajoute la sélection au survol
    if((!currentMetadatas.count()) && (Global::selectedTagHover)) {
        Metadata *metadata = ((Tag*)Global::selectedTagHover)->getDocument();
        if((metadata) && (!currentMetadatas.contains(metadata))) {
            metadata->tempStorage = 0;
            currentMetadatas << metadata;
        }
    }

    //Ajoute la sélection de chutier
    foreach(QTreeWidgetItem *item, ui->chutier->getTree()->selectedItems()) {
        Metadata *metadata = currentProject->getDocument(((UiFileItem*)item)->filename.file.absoluteFilePath());
        if((metadata) && (!currentMetadatas.contains(metadata))) {
            metadata->tempStorage = 0;
            currentMetadatas << metadata;
        }
    }
    chutierIsUpdating = false;

    if(!metadataIsUpdating) {
        metadataIsUpdating = true;
        ui->metadata->clear();
        if(currentMetadatas.count()) {
            QStringList expandItems;
            QString metadataPrefixGlobal = QString("<span style='font-family: Calibri, Arial; font-size: %1px; ").arg(Global::font.pixelSize());;
            QString metadataPrefix0 = metadataPrefixGlobal + "color: #F5F8FA'>";
            QString metadataPrefix1 = metadataPrefixGlobal + "color: %1'>";
            QString metadataSuffix = "</span>";
            for(quint16 i = 0 ; i < ui->metadata->topLevelItemCount() ; i++) {
                if(ui->metadata->topLevelItem(i)->isExpanded())
                    expandItems << HtmlDelegate::removeHtml(ui->metadata->topLevelItem(i)->text(0));
                for(quint16 j = 0 ; j < ui->metadata->topLevelItem(i)->childCount() ; j++) {
                    if(ui->metadata->topLevelItem(i)->child(j)->isExpanded())
                        expandItems << HtmlDelegate::removeHtml(ui->metadata->topLevelItem(i)->child(j)->text(0));
                }
            }
            if(expandItems.count() == 0)
                expandItems << "General" << "Contact details";

            QTreeWidgetItem *metadataRootItem = new QTreeWidgetItem(ui->metadata->invisibleRootItem(), QStringList() << metadataPrefix0 + tr("Details") + metadataSuffix);
            metadataRootItem->setFlags(Qt::ItemIsEnabled);
            if(expandItems.contains("Details"))
                ui->metadata->expandItem(metadataRootItem);

            //Versions
            if(sender() != ui->metadataSlider) {
                if(currentMetadatas.count() == 1) {
                    ui->metadataSlider->setMaximum(currentMetadatas.first()->getMetadataCountM());
                    if(Global::selectedTags.count()) {
                        Tag *tag = (Tag*)Global::selectedTags.first();
                        if(tag)
                            ui->metadataSlider->setValue(tag->getDocumentVersion());
                    }
                }

                if((currentMetadatas.count() == 1) && (currentMetadatas.first()->getMetadataCount() > 1)) ui->metadataSlider->setVisible(true);
                else                                                                                      ui->metadataSlider->setVisible(false);
                ui->metadataSliderIcon ->setVisible(ui->metadataSlider->isVisible());
                ui->metadataSliderIcon2->setVisible(ui->metadataSlider->isVisible());
            }
            else if((ui->metadataSlider->isVisible()) && (Global::selectedTags.count() == 1) && (currentMetadatas.count())) {
                Tag *tag = (Tag*)currentMetadatas.first()->tempStorage;
                if(tag) {
                    Document *document = (Document*)tag->getDocument();
                    foreach(Tag *tag, document->tags)
                        if(tag->getDocumentVersion() == ui->metadataSlider->value())
                            Global::selectedTags = QList<void*>() << tag;
                }
            }

            //Sum up
            foreach(Metadata *currentMetadata, currentMetadatas) {
                QMapIterator<QString, QMetaMap> metaIterator(currentMetadata->getMetadata(findDocumentVersionWithMetadata(currentMetadata)));
                while(metaIterator.hasNext()) {
                    metaIterator.next();

                    QTreeWidgetItem *rootItem = 0;
                    QString metaIteratorKey = metaIterator.key();
                    if(metaIteratorKey == "Rekall")
                        metaIteratorKey = "General";

                    QStringList itemText;
                    if(metaIteratorKey == "General")  itemText << metadataPrefix0 + "General" + metadataSuffix;
                    else                              itemText << metadataPrefix0 + metaIteratorKey + metadataSuffix;

                    for(quint16 i = 0 ; i < ui->metadata->topLevelItemCount() ; i++) {
                        bool itemExists = true;
                        for(quint16 j = 0 ; j < itemText.count() ; j++) {
                            if(HtmlDelegate::removeHtml(ui->metadata->topLevelItem(i)->text(j)) != HtmlDelegate::removeHtml(itemText.at(j)))
                                itemExists = false;
                        }
                        if(itemExists) {
                            rootItem = ui->metadata->topLevelItem(i);
                            break;
                        }
                    }
                    if(!rootItem) {
                        if(metaIteratorKey == "General")  rootItem = new QTreeWidgetItem(ui->metadata->invisibleRootItem(), itemText);
                        else                              rootItem = new QTreeWidgetItem(metadataRootItem, itemText);
                        rootItem->setFlags(Qt::ItemIsEnabled);
                    }

                    QMapIterator<QString,MetadataElement> ssMetaIterator(metaIterator.value());
                    while(ssMetaIterator.hasNext()) {
                        ssMetaIterator.next();

                        QString color = "#000000", defaultColor = "#C8C8C8";
                        if(ssMetaIterator.key() == "Name") {
                            if(currentMetadatas.count() == 1) {
                                ui->metadataTitle->setText(ssMetaIterator.value().toString().toUpper());
                                color = currentMetadata->baseColor.name();
                            }
                            else {
                                ui->metadataTitle->setText(QString("Multiple selection (%1)").arg(currentMetadatas.count()));
                                color = defaultColor;
                            }
                            ui->metadataTitle->setStyleSheet(QString("border-bottom: 1px solid %1; color: %1; margin: 0px 20px 0px 20px;").arg(color));
                            ui->metadataSlider->setStyleSheet(QString("QSlider::handle { background-color: %1; }").arg(color));
                            color = currentMetadata->baseColor.name();
                        }
                        else if(ssMetaIterator.key() == Global::tagColorCriteria->getTagName()) {
                            color = currentMetadata->baseColor.name();
                            ui->metadataSubTitle->setStyleSheet(QString("color: %1; font-size: %2px").arg(color).arg(Global::fontSmall.pixelSize()));
                            if(currentMetadatas.count() == 1)
                                ui->metadataSubTitle->setText(ssMetaIterator.value().toString().toUpper());
                            else
                                ui->metadataSubTitle->setText("");
                        }

                        if(color == "#000000")
                            color = defaultColor;

                        itemText = QStringList() << metadataPrefix0 + ssMetaIterator.key() + metadataSuffix
                                                 << metadataPrefix1.arg(color) + ssMetaIterator.value().toString() + metadataSuffix;

                        QTreeWidgetItem *item = 0;
                        for(quint16 i = 0 ; i < rootItem->childCount() ; i++) {
                            bool itemExists = true;
                            for(quint16 j = 0 ; j < /*itemText.count()*/1 ; j++) {
                                if(HtmlDelegate::removeHtml(rootItem->child(i)->text(j)) != HtmlDelegate::removeHtml(itemText.at(j)))
                                    itemExists = false;
                            }
                            if(itemExists) {
                                item = rootItem->child(i);
                                break;
                            }
                        }
                        if(item) {
                            if(HtmlDelegate::removeHtml(item->text(1)) != HtmlDelegate::removeHtml(itemText.at(1))) {
                                item->setText(0, "<i>" + item->text(0) + "</i>");
                                item->setText(1, "<i>" + item->text(1) + "</i>");
                            }
                        }
                        else {
                            item = new QTreeWidgetItem(rootItem, itemText);
                            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
                        }
                    }

                    if(expandItems.contains(metaIteratorKey))    ui->metadata->expandItem(rootItem);
                    else                                         ui->metadata->collapseItem(rootItem);
                }

                if((currentMetadata->chutierItem) || (currentMetadata->getType() == DocumentTypeWeb)) {
                    ui->metadataOpenFinder->setVisible(true);
                    ui->metadataOpen      ->setVisible(true);
                }
                else {
                    ui->metadataOpenFinder->setVisible(false);
                    ui->metadataOpen      ->setVisible(false);
                }

                if(currentMetadatas.last() == currentMetadata) {
                    displayDocumentName(QString("%1 (%2)").arg(currentMetadata->getName()).arg(currentMetadata->getMetadata("Rekall", "Folder").toString()));
                    displayPixmap(currentMetadata->getType(), currentMetadata->getThumbnail(findDocumentVersionWithMetadata(currentMetadata)));
                    displayGps(currentMetadata->getGps());
                }
            }
        }
        metadataIsUpdating = false;
    }
}



void Rekall::displayDocumentName(const QString &documentName) {
    gps->setWindowTitle(tr("Location — %1").arg(documentName));
}
void Rekall::displayPixmap(DocumentType documentType, const QPair<QString, QPixmap> &picture) {
    displayPixmap(documentType, picture.first, picture.second);
}
void Rekall::displayPixmap(DocumentType documentType, const QString &filename, const QPixmap &picture) {
    ui->preview->setMaximumHeight(ui->toolBoxRight->height() / 2.5);
    ui->preview->preview(documentType, filename, picture);
}
void Rekall::showPreviewTab() {
    ui->toolBoxRight->setCurrentIndex(1);
}


void Rekall::displayGps(const QList< QPair<QString,QString> > &gpsCoords) {
    QString mapsParam;
    for(quint16 i = 0 ; i < gpsCoords.count() ; i++) {
        QPair<QString,QString> gpsCoord = gpsCoords.at(i);
        if(gpsCoord.first.length()) {
            QStringList gpsData = gpsCoord.first.split(",");
            bool isGpsVal = false; gpsData.first().toDouble(&isGpsVal);
            if((isGpsVal) && (gpsData.count() > 1)) mapsParam += QString("['%1',%2,%3],").arg(gpsCoord.second.trimmed()).arg(gpsData.at(0).trimmed()).arg(gpsData.at(1).trimmed());
            else                                    mapsParam += QString("['%1','%2'],").arg(gpsCoord.second.trimmed()).arg(gpsCoord.first.trimmed());
        }
    }
    if(mapsParam.isEmpty())
        ui->metadataOpenGps->setVisible(false);
    else {
        mapsParam.chop(1);
        gps->page()->mainFrame()->evaluateJavaScript("initializeMaps([" + mapsParam + "])");
        ui->metadataOpenGps->setVisible(true);
    }
}



void Rekall::showHelp(bool visible) {
    ui->chutierLabel->setVisible(visible);
    ui->chutierLabel2->setVisible(visible);
    ui->tasksLabel->setVisible(visible);
    ui->feedsLabel->setVisible(visible);
    ui->peopleLabel->setVisible(visible);
    ui->previewLabel->setVisible(visible);
    ui->conduiteLabel->setVisible(visible);
}

void Rekall::timerEvent(QTimerEvent *) {
    if(Global::userInfos->updateDecounter < 0)
        Global::userInfos->update();
    Global::userInfos->updateDecounter--;
}

void Rekall::closeEvent(QCloseEvent *) {
    /*
#ifdef Q_OS_MAC
        ProcessSerialNumber psn;
        if(GetCurrentProcess(&psn) == noErr)
            TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
#endif
    */
}

void Rekall::setVisbility(bool visibility) {
    if(visibility) {
        /*
#ifdef Q_OS_MAC
        ProcessSerialNumber psn;
        if(GetCurrentProcess(&psn) == noErr)
            TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endifshow
*/
        show();
        raise();
    }
    else {
        close();
    }
}


void Rekall::refreshMenus(const QFileInfo &path, bool clear) {
    QFileInfoList recentPaths;
    if(path.exists())
        recentPaths << path;

    if(true) {
        QSettings settings;
        quint16 recentsCount = settings.beginReadArray("recents");
        for(quint16 recentsIndex = 0 ; recentsIndex < recentsCount ; recentsIndex++) {
            settings.setArrayIndex(recentsIndex);
            QFileInfo recentPath(settings.value("path").toString());
            if((recentPath.exists()) && (!recentPaths.contains(recentPath)))
                recentPaths << recentPath;
        }
        settings.endArray();
    }

    if(clear)
        recentPaths.clear();

    if(true) {
        QSettings settings;
        settings.beginWriteArray("recents", recentPaths.count());
        for(quint16 recentsIndex = 0 ; recentsIndex < recentPaths.count() ; recentsIndex++) {
            settings.setArrayIndex(recentsIndex);
            settings.setValue("path", recentPaths.at(recentsIndex).absoluteFilePath());
        }
        settings.endArray();
    }

    foreach(QAction *action, openRecentAction)
        delete action;
    openRecentAction.clear();
    if(recentPaths.count()) {
        foreach(const QFileInfo &recentPath, recentPaths) {
            QAction *action = new QAction(recentPath.absoluteFilePath(), this);
            connect(action, SIGNAL(triggered()), SLOT(action()));
            openRecentAction << action;
        }
        ui->menuOpen->insertActions(ui->actionOpenRecentNo, openRecentAction);
        ui->actionOpenRecentNo   ->setVisible(false);
        ui->actionOpenRecentClear->setVisible(true);
    }
    else {
        ui->actionOpenRecentNo   ->setVisible(true);
        ui->actionOpenRecentClear->setVisible(false);
    }

    ui->actionSave ->setEnabled(Global::pathCurrent.exists());
    ui->actionClose->setEnabled(Global::pathCurrent.exists());
}
