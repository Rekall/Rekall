#include "rekall.h"
#include "ui_rekall.h"

Rekall::Rekall(QWidget *parent) :
    RekallBase(parent),
    ui(new Ui::Rekall) {
    ui->setupUi(this);
    setAcceptDrops(true);
    currentProject = 0;
    metaIsChanging = false;
    openProject    = true;

    Global::userInfos = new UserInfos();
    updateUserInfos = -1;

    Global::mainWindow = this;

    QApplication::setQuitOnLastWindowClosed(false);

    Watcher *watcher  = new Watcher(this);
    Global::watcher   = watcher;
    Global::taskList->setToolbox(ui->toolBoxLeft);
    Global::feedList->setToolbox(ui->toolBoxLeft);

    Global::chutier = ui->chutier->getTree();
    Global::udp = new Udp(0, 5678);
    httpUpload = new FileUploadController();
    settings = new QSettings("rekall.ini", QSettings::IniFormat, this);
    settings->beginGroup("listener");
    http = new HttpListener(settings, httpUpload, this);
    connect(httpUpload, SIGNAL(fileUploaded(QString, QString, QTemporaryFile*)), SLOT(fileUploaded(QString, QString, QTemporaryFile*)), Qt::QueuedConnection);

    Global::font.setFamily("Gotham");
    Global::font.setPixelSize(10);
    Global::fontSmall.setFamily("Gotham");
    Global::fontSmall.setPixelSize(9);
    Global::fontLarge.setFamily("Gotham");
    Global::fontLarge.setPixelSize(20);

    UiFileItem::configure(ui->chutier, false);
    UiFileItem::forbiddenDirs << "rekall_cache";
    UiFileItem::syncWith(QFileInfoList() << Global::pathCurrent, ui->chutier->getTree());
    ui->chutier->getTree()->collapseAll();

    currentProject = new Project(this);
    Global::currentProject = currentProject;
    connect(currentProject, SIGNAL(displayMetaData()), SLOT(displayMetadata()));
    connect(currentProject, SIGNAL(refreshMetadata()), SLOT(refreshAndLastMetadata()));

    inspector = new Inspector(this);
    inspector->toolbarButton = ui->actionInspector;
    connect(ui->actionInspector, SIGNAL(triggered()), SLOT(showInspector()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(action()));
    connect(ui->actionPaste, SIGNAL(triggered()), SLOT(action()));

    ui->metadata->setColumnWidth(0, 135);
    ui->metadataSlider->setVisible(false);

    ui->chutier->showNew(false);
    ui->chutier->showImport(true);
    connect(ui->chutier->getTree(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(chutierItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->actionPlay, SIGNAL(triggered()), ui->timeline, SLOT(actionPlay()));
    connect(ui->actionRewind, SIGNAL(triggered()), ui->timeline, SLOT(actionRewind()));

    ui->persons->setColumnWidth(0, 40);
    ui->persons->setColumnWidth(1, 100);
    ui->persons->setColumnWidth(2, 100);
    ui->persons->setColumnWidth(3, 100);


    //GPS
    ui->metadataOpenGps->setVisible(false);
    gps = new QWebView(0);
    gps->setWindowFlags(Qt::Tool);
    gps->load(QUrl::fromLocalFile(Global::pathApplication.absoluteFilePath() + "/tools/maps.html"));


    //Display splash
    splash = new Splash();
    splash->show();
    QTimer::singleShot(1500, this, SLOT(closeSplash()));
    QCoreApplication::processEvents();

    startTimer(1000);
}

Rekall::~Rekall() {
    settings->sync();
    delete ui;
}

void Rekall::fileUploaded(QString gpsCoord, QString filename, QTemporaryFile *file) {
    if((filename.toLower().endsWith("jpg")) || (filename.toLower().endsWith("jpeg"))) {
        QPair<QString, QPixmap> picture;
        picture.first = file->fileName();
        picture.second = QPixmap(file->fileName());
        Global::mainWindow->displayDocumentName(tr("Imported file"));
        Global::mainWindow->displayPixmap(picture);
        Global::mainWindow->displayGps(qMakePair(gpsCoord, tr("Imported file")));
    }
}

void Rekall::dragEnterEvent(QDragEnterEvent *event) {
    if(parseMimeData(event->mimeData(), true))
        return event->acceptProposedAction();
}
void Rekall::dropEvent(QDropEvent *event) {
    if(parseMimeData(event->mimeData()))
        return event->acceptProposedAction();
}

bool Rekall::parseMimeData(const QMimeData *mime, bool test) {
    bool retour = false;

    qDebug("------------------------------------");
    qDebug("Texte > %s", qPrintable(mime->text()));
    qDebug("Html  > %s", qPrintable(mime->html()));
    foreach(const QUrl &url, mime->urls())
        qDebug("URL> %s", qPrintable(url.toString()));
    foreach(const QString &format, mime->formats())
        qDebug("Data %s > %s", qPrintable(format), qPrintable(QString(mime->data(format))));
    qDebug("------------------------------------");


    if(test) {
        if((mime->hasUrls()) || (mime->hasFormat("text/plain")))
            retour = true;
    }
    else {
        if (mime->hasUrls()) {
            QList<QUrl> urlList = mime->urls();
            for(int i = 0; i < urlList.size() && i < 32; ++i) {
                QString file = urlList.at(i).toLocalFile();
                if(QFileInfo(file).suffix().toLower() == "vcf") {
                    currentProject->persons.append(Person::fromFile(ui->persons, file));
                    QList<Person*> persons = Person::fromFile(ui->persons, file);
                    if(persons.count()) {
                        currentProject->persons.append(persons);
                        refreshPerson();
                    }
                }
                else {

                }
                //Global::watcher->sync(urlList.at(i).toLocalFile());
            }
            retour |= true;
        }
        if(mime->hasFormat("text/plain")) {
            QList<Person*> persons = Person::fromString(ui->persons, mime->data("text/plain"));
            if(persons.count()) {
                currentProject->persons.append(persons);
                refreshPerson();
            }
            retour |= true;
        }
    }
    return retour;
}

/*
void Rekall::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        const UiFileItem *file = qobject_cast<const UiFileItem*>(event->mimeData());
        if(file) {
            // access myData's data directly (not through QMimeData's API)
        }
    }
}
void Rekall::dropEvent(QDropEvent *) {
*/

void Rekall::action() {
    if(sender() == ui->actionSave)
        currentProject->save();
    else if(sender() == ui->actionPaste)
        parseMimeData(QApplication::clipboard()->mimeData());
    else if(sender() == ui->metadataOpenGps)
        gps->show();
}

void Rekall::actionMetadata(QTreeWidgetItem *item, int) {
    if((!metaIsChanging) && (item) && (currentDocument)) {
        QString category, meta, value;
        if(item->parent())
            category = item->parent()->text(0);
        meta  = item->text(0);
        value = item->text(1);
    }
}

void Rekall::closeSplash() {
    splash->close();
    showMaximized();
    inspector->show();
    updateGeometry();
    ui->timelineSplitter->setSizes(QList<int>() << ui->timelineSplitter->height() * 0.50 << ui->timelineSplitter->height() * 0.50);
    ui->fileSplitter->setSizes(QList<int>()     << ui->fileSplitter->width()      * 0.25 << ui->fileSplitter->width()      * 0.75);
    ui->conduiteSplitter->setSizes(QList<int>() << ui->conduiteSplitter->width()  * 0.75 << ui->conduiteSplitter->width()  * 0.25);
    ui->personsSplitter->setSizes(QList<int>()  << ui->personsSplitter->width()   * 0.35 << ui->personsSplitter->width()   * 0.65);
    inspector->move(QDesktopWidget().screenGeometry().topRight() - QPoint(inspector->width(), 0));
    //trayMenu->showMessage("Rekall", "Ready!", QSystemTrayIcon::NoIcon);
}


void Rekall::refreshPerson() {
    ui->tabs->setCurrentIndex(2);
    if(ui->persons->currentItem()) {
        Person *person = (Person*)ui->persons->currentItem();
        ui->personPhoto->setPixmap(person->card.getPhoto());
        ui->personFirstame->setText(person->card.getFirstname());
        ui->personLastname->setText(person->card.getLastname());

        ui->personInfos->clear();
        QMapIterator<QString, QPair<PersonCardHeader, PersonCardValues> > personCardInfoIterator(person->card);
        while(personCardInfoIterator.hasNext()) {
            personCardInfoIterator.next();
            if(personCardInfoIterator.value().first.category != "photo") {
                QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << personCardInfoIterator.value().first.toString() << personCardInfoIterator.value().second.toString());
                ui->personInfos->addTopLevelItem(item);
            }
        }
    }
}

void Rekall::displayMetadata() {
    ui->tabs->setCurrentIndex(1);
    refreshMetadata();
}
void Rekall::refreshMetadata() {
    if(!metaIsChanging)
        chutierItemChanged(ui->chutier->getTree()->currentItem(), ui->chutier->getTree()->currentItem());
}
void Rekall::refreshAndLastMetadata() {
    if(!metaIsChanging)
        chutierItemChanged(ui->chutier->getTree()->currentItem(), 0);
}
void Rekall::chutierItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *itemB) {
    ui->chutier->getTree()->setCurrentItem(item);
    metaIsChanging = true;
    QStringList expandItems;
    for(quint16 i = 0 ; i < ui->metadata->topLevelItemCount() ; i++)
        if(ui->metadata->topLevelItem(i)->isExpanded())
            expandItems << ui->metadata->topLevelItem(i)->text(0);
    if(expandItems.count() == 0)
        expandItems << "Rekall";

    currentDocument = currentProject->getDocumentAndSelect(((UiFileItem*)item)->filename.file.absoluteFilePath());
    if(currentDocument) {
        ui->metadata->clear();
        ui->toolBoxRight->setItemText(0, tr("INFOS — %1 (%2)").arg(currentDocument->getMetadata("Document Name").toString()).arg(currentDocument->getMetadata("Document Folder").toString()));

        //Versions
        ui->metadataSlider->setMaximum(currentDocument->getMetadataCountM());
        if(item != itemB)
            ui->metadataSlider->setValue(currentDocument->getMetadataCountM());
        if(currentDocument->getMetadataCount() > 1) ui->metadataSlider->setVisible(true);
        else                                        ui->metadataSlider->setVisible(false);

        if(!Global::selectedTag) {
            foreach(Tag *tag, currentDocument->tags) {
                if(tag->getDocumentVersion() == ui->metadataSlider->value()) {
                    Global::timelineGL->ensureVisible(tag->timelineDestPos);
                    Global::viewerGL  ->ensureVisible(tag->viewerDestPos);
                }
            }
        }

        //Sum up
        QMapIterator<QString, QMetaMap> metaIterator(currentDocument->getMetadata(ui->metadataSlider->value()));
        while(metaIterator.hasNext()) {
            metaIterator.next();
            QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->metadata->invisibleRootItem(), QStringList() << metaIterator.key());
            rootItem->setFlags(Qt::ItemIsEnabled);

            QMapIterator<QString,MetadataElement> ssMetaIterator(metaIterator.value());
            while(ssMetaIterator.hasNext()) {
                ssMetaIterator.next();
                QTreeWidgetItem *item = new QTreeWidgetItem(rootItem, QStringList() << ssMetaIterator.key() << ssMetaIterator.value().toString());
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
            }

            if(expandItems.contains(metaIterator.key()))    ui->metadata->expandItem(rootItem);
            else                                            ui->metadata->collapseItem(rootItem);
        }
        Global::mainWindow->displayDocumentName(QString("%1 (%2)").arg(currentDocument->getMetadata("Document Name").toString()).arg(currentDocument->getMetadata("Document Folder").toString()));
        Global::mainWindow->displayPixmap(currentDocument->getThumbnail(ui->metadataSlider->value()));
        Global::mainWindow->displayGps(currentDocument->getGps());
    }
    metaIsChanging = false;
}
void Rekall::displayDocumentName(const QString &documentName) {
    gps->setWindowTitle(tr("Location — %1").arg(documentName));
}
void Rekall::displayPixmap(const QPair<QString, QPixmap> &_picture) {
    ui->preview->displayPixmap(_picture);
}
void Rekall::displayGps(const QPair<QString,QString> &gpsCoord) {
    QStringList gpsData = gpsCoord.first.split(",");
    if(gpsData.count() > 1) {
        gps->page()->mainFrame()->evaluateJavaScript("initializeMaps([" + QString("['%1',%2,%3],").arg(gpsCoord.second).arg(gpsData.at(0).trimmed()).arg(gpsData.at(1).trimmed()) + "])");
        ui->metadataOpenGps->setVisible(true);
    }
    else
        ui->metadataOpenGps->setVisible(false);
}



void Rekall::showInspector() {
    inspector->toolbarButton = ui->actionInspector;
    if(inspector->isVisible())   inspector->close();
    else                         inspector->show();
}

void Rekall::timerEvent(QTimerEvent *) {
    if(updateUserInfos < 0) {
        updateUserInfos = 600;
        Global::userInfos->update();
    }
    updateUserInfos--;

    if(openProject) {
        openProject = false;
        currentProject->open(QDir(Global::pathCurrent.absoluteFilePath()));
    }
}

void Rekall::closeEvent(QCloseEvent *) {
    inspector->hide();
    /*
#ifdef Q_OS_MAC
        ProcessSerialNumber psn;
        if(GetCurrentProcess(&psn) == noErr)
            TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
#endif
    */
}

void Rekall::setVisbility(bool sh) {
    if(sh) {
        /*
#ifdef Q_OS_MAC
        ProcessSerialNumber psn;
        if(GetCurrentProcess(&psn) == noErr)
            TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif
*/
        show();
        inspector->show();
        raise();
    }
    else {
        close();
    }
}
