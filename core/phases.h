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


private:
    UiReal phasesByDaysTo;
    QMap<QString, MetadataElement> elementsToProcess;
public:
    bool needCalulation;
public:
    inline void addCheck(const MetadataElement &value) {
        elementsToProcess.insert(value.toString(), value);
    }
    inline void addCheckStart() {
        elementsToProcess.clear();
    }
    void addCheckEnd();


private:
    QString tagNameCategory, tagName;
public:
    inline const QString getTagName()         const { return tagName; }
    inline const QString getTagNameCategory() const { return tagNameCategory; }


protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);

public:
    bool isAcceptable(bool strongCheck, const MetadataElement &value) const;
    const QString getPhaseFor(const MetadataElement &value) const;
    const QString getVerbosePhaseFor(const QString &phaseId) const;

public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);

public slots:
    void action();
    void actionNames();
    void actionSelection();
signals:
    void actionned(QString,QString);

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);

public:
    void reset(const QString &filterText = "", QStringList checks = QStringList());

private:
    Ui::Phases *ui;
};

#endif // PHASES_H
