#ifndef PHASES_H
#define PHASES_H

#include <QWidget>
#include "misc/options.h"
#include "qmath.h"

namespace Ui {
class Phases;
}

class Phase : public QTreeWidgetItem {
public:
    explicit Phase(QTreeWidget *parent, const QDateTime &date, const QString &name);
public:
    void setValues(const QDateTime &date, const QString &name);
    inline const QDateTime getDate() const {
        return QDateTime::fromString(getDateStr(), "yyyy/MM/dd hh:mm:ss");
    }
    inline const QString getDateStr() const {
        return text(1);
    }
    inline const QString getName() const {
        return text(0);
    }
};

class Phases : public QWidget {
    Q_OBJECT
    
public:
    explicit Phases(QWidget *parent = 0);
    ~Phases();

public:
    UiReal phasesByDaysTo;
    QMap<QString, MetadataElement> metaElements;
    bool needCalulation;
    QString tagNameCategory, tagName;
public:
    void addCheck(const MetadataElement &value) { metaElements.insert(value.toString(), value); }

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);
    void addCheckEnd();
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);

public:
    const QString getPhaseFor(const MetadataElement &value) const;
    bool isAcceptable(bool strongCheck, const MetadataElement &value) const;
    const QString getVerbosePhaseFor(const QString &phaseId) const;

public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

public slots:
    void action();
    void actionNames();
    void actionSelection();
signals:
    void actionned(QString,QString);

private:
    Ui::Phases *ui;
};

#endif // PHASES_H
