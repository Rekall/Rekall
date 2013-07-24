#include "feedlist.h"
#include "ui_feedlist.h"

FeedList::FeedList(QWidget *parent) :
    QWidget(parent, Qt::Tool),
    ui(new Ui::FeedList) {
    ui->setupUi(this);
    startTimer(5000);
}

FeedList::~FeedList() {
    delete ui;
}

void FeedList::addFeed(FeedItemBase *feedItem) {
    ui->feeds->addTopLevelItem(feedItem);
    timerEvent(0);
    show();
}

void FeedList::timerEvent(QTimerEvent *) {
    for(quint16 i = 0 ; i < ui->feeds->topLevelItemCount() ; i++)
        ((FeedItemBase*)ui->feeds->topLevelItem(i))->update(Global::dateToString(((FeedItemBase*)ui->feeds->topLevelItem(i))->date));
}
