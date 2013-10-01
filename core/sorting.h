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
    explicit Sorting(bool filter, const QString &title, quint16 index = 0, QWidget *parent = 0);
    ~Sorting();


public:
    bool isFilter, asNumber, asDate, sortAscending;
    UiBool displayLinked;
    QString tagName;
    qint16 left, leftLength;


private:
    QRegExp regexp;
public:
    void setTagname(const QString &_tagName);
public:
    inline bool orderBy(bool val) const {
        if(sortAscending)   return val;
        else                return !val;
    }

private:
    QHash<QString,QString> criteriaFormatedCache;
public:
    const QString getCriteriaFormated(const QString &criteria);
    bool isAcceptableWithFilters(const QString &criteria);
    const QString getAcceptableWithFilters(const QString &criteria);
    const QString getMatchName() const;

public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

signals:
    void actionned();

private slots:
    void action();

private:
    Ui::Sorting *ui;
};

#endif // SORTING_H
