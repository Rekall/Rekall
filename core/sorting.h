#ifndef SORTING_H
#define SORTING_H

#include <QWidget>
#include "misc/options.h"

namespace Ui {
class Sorting;
}

class Sorting : public QWidget {
    Q_OBJECT
    
public:
    explicit Sorting(const QString &title, quint16 index = 0, bool _needWord = false, QWidget *parent = 0);
    ~Sorting();


public:
    bool asNumber, asDate, sortAscending;
    QString tagName, tagNameCategory;
    qint16 left, leftLength;
    bool isUpdating, needWord;


private:
    QRegExp regexp;
public:
    void setTagname(const QString &_tagName);
public:
    inline bool orderBy(bool val) const {
        if(sortAscending)   return val;
        else                return !val;
    }

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);


private:
    QHash<QString,QString> criteriaFormatedCache;
public:
    const QString getCriteriaFormated(const QString &criteria);
    bool isAcceptable(bool strongCheck, const QString &criteria);
    const QString getAcceptableWithFilters(const QString &criteria);
    const QString getMatchName() const;

public:
    void addCheckStart();
    void addCheck(const QString &check, const QString &value = QString());

public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

signals:
    void actionned(QString,QString);

public slots:
    void action();
    void actionSelection();

private:
    Ui::Sorting *ui;
};

#endif // SORTING_H
