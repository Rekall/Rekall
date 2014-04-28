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

#include "taskslist.h"
#include "ui_taskslist.h"

qint16 TasksList::runningTasks = 0;
qint16 TasksList::runningWebTasks = 0;

TasksList::TasksList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TasksList) {
    ui->setupUi(this);
    hide();
    move(0, 0);
    runningTasks = runningWebTasks = 0;
    webViewTask  = 0;

    Global::taskList = this;
    webView = new QWebView();
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(webPageLoaded()));

    ui->tasks->setItemDelegateForColumn(0, new HtmlDelegate());
}

TasksList::~TasksList() {
    delete ui;
}

void TasksList::setToolbox(QToolBox *_toolbox) {
    toolbox = _toolbox;
    //connect(toolbox, SIGNAL(currentChanged(int)), SLOT(timerEvent()));
}

void TasksList::addTask(Metadata *metadata, TaskProcessType type, qint16 version, bool needCompleteScan) {
    addTask(TaskProcessData(metadata, type, version, needCompleteScan));
    if(toolbox->currentIndex() != 2)
        oldToolboxIndex = toolbox->currentIndex();
    toolbox->setCurrentIndex(2);
    toolbox->setItemEnabled(2, true);
}
void TasksList::addTask(const TaskProcessData &data) {
    show();
    TaskProcess *task = new TaskProcess(data, ui->tasks->invisibleRootItem(), this);
    connect(task, SIGNAL(finished(TaskProcess*)), SLOT(finished(TaskProcess*)));
    connect(task, SIGNAL(updateList(TaskProcess*,int)), SLOT(updateList(TaskProcess*,int)));
    connect(task, SIGNAL(updateList(TaskProcess*,const QString &)), SLOT(updateList(TaskProcess*,const QString &)));
    connect(task, SIGNAL(analyseWebContent(TaskProcess*)), SLOT(analyseWebContent(TaskProcess*)));
    tasks.append(task);
    task->init();
    nextTask();
}

void TasksList::nextTask() {
    if(runningTasks < 5) {
        if(tasks.count()) {
            runningTasks++;
            foreach(TaskProcess *task, tasks) {
                if(!task->started) {
                    if(task->document.metadata->getType(task->document.version) == DocumentTypeWeb) {
                        if(!runningWebTasks) {
                            runningWebTasks = 1;
                            task->start();
                            return;
                        }
                    }
                    else {
                        task->start();
                        return;
                    }
                }
            }
        }
        else {
            hide();
            if(toolbox->currentIndex() == 2)
                toolbox->setCurrentIndex(oldToolboxIndex);
            toolbox->setItemEnabled(2, false);
            Global::timelineSortChanged = Global::viewerSortChanged = Global::eventsSortChanged = Global::phases->needCalulation = true;
        }
    }
}

void TasksList::updateList(TaskProcess *, int) {
    //Global::feedList->addFeed(new FeedItemBase((FeedItemBaseType)feedAction, "Rekall", task->processedDocument.metadata->file.baseName()));
}
void TasksList::updateList(TaskProcess *task, const QString &message) {
    task->setText(0, message);
}

void TasksList::finished(TaskProcess *task) {
    tasks.removeOne(task);
    ui->tasks->invisibleRootItem()->removeChild(task);
    runningTasks--;
    nextTask();
}


void TasksList::analyseWebContent(TaskProcess *_webViewTask) {
    webViewTask = _webViewTask;
    if((QFileInfo(webViewTask->thumbFilename).exists()) && (false))
        webPageLoadedEnd();
    else
        webView->load(webViewTask->document.metadata->getMetadata("Rekall", "URL", webViewTask->document.version).toString());
}
void TasksList::webPageLoaded() {
    qDebug("Chargement Web");
    if(webViewTask) {
        QWebFrame *webFrame = webView->page()->mainFrame();
        webFrame->setScrollBarPolicy(Qt::Vertical,   Qt::ScrollBarAlwaysOff);
        webFrame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        QSize size = webFrame->contentsSize();
        webView->setGeometry(QRect(QPoint(0, 0), size));

        //Name
        webViewTask->document.metadata->setMetadata("Rekall", "Name", webFrame->title(), webViewTask->document.version);

        //Texte
        QString html = webFrame->toHtml().toLower();
        qint16 metaIndexStart = html.indexOf("<meta"), metaIndexEnd = 0;
        QString nodes = "<metas>\n";
        while(metaIndexStart >= 0) {
            metaIndexEnd = html.indexOf(">", metaIndexStart) + 1;
            if(metaIndexEnd > metaIndexStart) {
                QString node = html.mid(metaIndexStart, metaIndexEnd - metaIndexStart).replace("\">", "\" >").replace(" >", "/>").trimmed();
                if((node.startsWith("<")) && (node.endsWith("/>")))
                    nodes += node + "\n";
            }
            metaIndexStart = html.indexOf("<meta", metaIndexStart+1);
        }
        nodes += "</metas>";
        QDomDocument doc;
        doc.setContent(nodes);
        QDomElement docElem = doc.documentElement();
        QDomNode n = docElem.firstChild();
        QStringList documentKeywords;
        while(!n.isNull()) {
            QDomElement element = n.toElement();
            if((!element.isNull()) && (element.tagName() == "meta") && (element.attributes().contains("name")) && (element.attributes().contains("content"))) {
                QString name    = element.attribute("name");
                QString content = element.attribute("content");
                if((name == "title") || (name == "description") || (name == "keywords")) {
                    QStringList tags = content.split(QRegExp("\\b"), QString::SkipEmptyParts);
                    foreach(QString tag, tags) {
                        tag = tag.trimmed();
                        if(tag.length() > 2)
                            documentKeywords << tag;
                    }
                }
            }
            n = n.nextSibling();
        }
        webViewTask->document.metadata->addKeyword(documentKeywords, webViewTask->document.version);

        //Thumbnail
        QPixmap::grabWidget(webView).toImage().save(webViewTask->thumbFilepath + ".jpg");

        //Commit
        webPageLoadedEnd();
    }
}
void TasksList::webPageLoadedEnd() {
    webViewTask->document.metadata->thumbnails.append(GlRect(webViewTask->thumbFilename));
    webViewTask->sendFinishedSignal();
    webViewTask     = 0;
    runningWebTasks = 0;
    webView->load(QUrl("about:blank"));
}

