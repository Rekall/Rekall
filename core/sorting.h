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

#ifndef SORTING_H
#define SORTING_H

#include <QWidget>
#include "qmath.h"
#include "misc/options.h"

namespace Ui {
class Sorting;
}

class Sorting : public QWidget {
    Q_OBJECT
    
public:
    explicit Sorting(const QString &title, quint16 index = 0, bool _needWord = false, bool _isHorizontal = false, QWidget *parent = 0);
    ~Sorting();

private:
    bool sortAscending, isUpdating, needWord, isHorizontal;
    bool asNumber, asDate, asTimeline;
    QPair<qreal,qreal> asNumberRange;
    qint16 left, leftLength;
    QString tagNameCategory, tagName;
public:
    inline const QString getTagName()         const { return tagName;         }
    inline const QString getTagNameCategory() const { return tagNameCategory; }
    inline       qint16  getTrunctionLeft()   const { return left;            }
    inline       qint16  getTrunctionLength() const { return leftLength;      }
    inline       bool    isTimeline()         const { return asTimeline;      }
    inline       bool    isNumber()           const { return asNumber;        }
    inline       bool    isDate()             const { return asDate;          }

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
signals:
    void displayed(bool);

private:
    QHash<QString,QString> criteriaFormatedCache;
    QStringList criteriaFormatedRealCacheRaw, criteriaFormatedRealCacheFormated;

public:
    const QString getCriteria(const QString &criteria) const;
    const QString getCriteriaFormated(const QString &criteria);
    const QString getCriteriaFormated(qreal criteria) const;
    qreal getCriteriaFormatedReal(const QString &criteria, qreal timeValue) const;
    inline qreal getCriteriaFormatedRealDuration(qreal durationValue) const {
        if(asTimeline)  return durationValue;
        else            return 0;
    }
    bool isAcceptable(bool strongCheck, const QString &criteria) const;
    const QString getAcceptableWithFilters(const QString &criteria) const;
    const QString getMatchName() const;

public:
    void addCheckStart();
    void addCheck(const QString& sorting, const QString &sortingFormated, const QString &complement);
    void addCheckEnd();

public:
    QDomElement serialize(QDomDocument &xmlDoc) const;
    void deserialize(const QDomElement &xmlElement);

signals:
    void actionned(QString,QString);

public slots:
    void action();
    void actionSelection();

public:
    static const QString timeToString(qreal time, bool millisec = false);
    static qreal stringToTime(const QString &timeStr);
    static qreal toDouble(const MetadataElement &elmt, bool *ok = 0);
    static qreal toDouble(const QString &str, bool *ok = 0);

public:
    const QString styleSheet2() const;
    void setStyleSheet2(const QString &str);

public:
    void reset(const QString &filterText = "", QString matchText = "", QStringList checks = QStringList());

protected:
    void mouseReleaseEvent(QMouseEvent *);

private:
    Ui::Sorting *ui;
};

#endif // SORTING_H
