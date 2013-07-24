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

public:
    void addFeed(FeedItemBase *feedItem);

protected:
    void timerEvent(QTimerEvent *);
    
private:
    Ui::FeedList *ui;
};

#endif // HISTORYLIST_H
