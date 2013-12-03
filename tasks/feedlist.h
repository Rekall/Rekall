#ifndef FEEDLIST_H
#define FEEDLIST_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class FeedList;
}

class FeedList : public QWidget, public FeedListBase {
    Q_OBJECT
    
public:
    explicit FeedList(QWidget *parent = 0);
    ~FeedList();

private:
    QToolBox *toolbox;
    quint16   notifCounter;
    QList<QTreeWidgetItem*> agos;
    QList<FeedItemBase*>    feeds;
    QTreeWidgetItem *agoNow, *agoSecs, *agoMin, *agoHours, *agoDays, *agoWeeks, *agoMonthes, *agoYears;

public:
    void setToolbox(QToolBox*);
    void addFeed(FeedItemBase *feedItem);

protected:
    void timerEvent(QTimerEvent *);
private slots:
    void timerEvent();
    
private:
    Ui::FeedList *ui;
};

#endif // HISTORYLIST_H
