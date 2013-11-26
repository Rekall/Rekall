#ifndef TIMELINECONTROL_H
#define TIMELINECONTROL_H

#include <QWidget>
#include "misc/global.h"

namespace Ui {
class TimelineControl;
}

class TimelineControl : public QWidget {
    Q_OBJECT

public:
    explicit TimelineControl(QWidget *parent = 0);
    ~TimelineControl();

private:
    Ui::TimelineControl *ui;

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);

public slots:
    void action();

};

#endif // TIMELINECONTROL_H
