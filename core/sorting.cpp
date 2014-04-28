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

#include "sorting.h"
#include "ui_sorting.h"

Sorting::Sorting(const QString &title, quint16 index, bool _needWord, bool _isHorizontal, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::Sorting) {

    ui->setupUi(this);
    ui->title->setText(title);
    setWindowTitle(title);
    needWord     = _needWord;
    isHorizontal = _isHorizontal;

    ui->filter->addItem("", "");
    if(isHorizontal)
        ui->filter->addItem("time", "Time");
    ui->filter->addItem("date (year)",  "Rekall->Date/Time | 0,4");
    ui->filter->addItem("date (month)", "Rekall->Date/Time | 0,7");
    ui->filter->addItem("date (day)",   "Rekall->Date/Time | 0,10");  // 1234:67:90 23:56:89
    ui->filter->addItem("time (hours)", "Rekall->Date/Time | 11,2");  // 0123:56:89012:45:67
    ui->filter->addItem("type",         "Rekall->Type");
    ui->filter->addItem("author",       "Rekall->Author");
    ui->filter->addItem("keywords",     "Rekall->All");
    ui->filter->addItem("name",         "Rekall->Name");
    ui->filter->addItem("import date",  "Rekall->Import Date/Time | 0,16");
    ui->filter->addItem("import author","Rekall->Import Author");
    ui->filter->addItem("first letter (name)", "Rekall->Name | 0,1");
    ui->filter->addItem("Composite->Light Value");
    ui->filter->addItem("Rekall->Size");
    ui->filter->setCurrentIndex(index);
    ui->checks->sortByColumn(0, Qt::AscendingOrder);
    isUpdating = false;
    actionSelection();
}

void Sorting::showEvent(QShowEvent *) {
    emit(displayed(true));
}
void Sorting::closeEvent(QCloseEvent *) {
    emit(displayed(false));
}
void Sorting::hideEvent(QHideEvent *) {
    emit(displayed(false));
}
const QString Sorting::styleSheet2() const {
    return styleSheet() + ui->globalFrameRed->styleSheet();
}
void Sorting::setStyleSheet2(const QString &str) {
    ui->globalFrameRed->setStyleSheet(str);
}

void Sorting::action() {
    if(needWord) {
        bool needWordValue = !(ui->matches->text().isEmpty());
        ui->checks    ->setVisible(needWordValue);
        ui->checkAll  ->setVisible(needWordValue);
        ui->invertAll ->setVisible(needWordValue);
        ui->uncheckAll->setVisible(needWordValue);
        ui->sorting   ->setVisible(needWordValue);
    }
    if(isUpdating)
        return;

    if(sender() == ui->sorting) {
        ui->checks->sortByColumn(0, Qt::AscendingOrder);
    }
    else if((sender() == ui->checkAll) || (sender() == ui->uncheckAll) || (sender() == ui->invertAll)) {
        QList<QTreeWidgetItem*> items;
        if(ui->checks->selectedItems().count() > 1)
            items = ui->checks->selectedItems();
        else {
            for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++)
                if(!ui->checks->topLevelItem(i)->isHidden())
                    items << ui->checks->topLevelItem(i);
        }
        foreach(QTreeWidgetItem *item, items) {
            if(     (sender() == ui->checkAll)   && (item->checkState(1) == Qt::Unchecked))
                item->setCheckState(1, Qt::Checked);
            else if((sender() == ui->uncheckAll) && (item->checkState(1) == Qt::Checked))
                item->setCheckState(1, Qt::Unchecked);
            else if((sender() == ui->invertAll)  && (item->checkState(1) == Qt::Checked))
                item->setCheckState(1, Qt::Unchecked);
            else if((sender() == ui->invertAll)  && (item->checkState(1) == Qt::Unchecked))
                item->setCheckState(1, Qt::Checked);
        }
    }
    else {
        if(ui->filter->count()) {
            QString filterText = ui->filter->itemData(ui->filter->currentIndex()).toString();
            if(filterText.isEmpty())
                filterText = ui->filter->currentText();

            if(((needWord) && (ui->matches->text().isEmpty())) || (filterText.isEmpty())) {
                tagName         = "";
                tagNameCategory = "";
            }
            else {
                QStringList sortSplit = filterText.split("|");

                leftLength    = -1;
                asNumber      = asDate = asTimeline = false;
                sortAscending = true;
                QString _tagName = sortSplit.first().trimmed();
                if(_tagName.count()) {
                    QStringList tagNames = _tagName.split("->");
                    if(tagNames.count()) {
                        tagName       = tagNames.last();
                        if(tagNames.count() > 1)
                            tagNameCategory = tagNames.first();
                        if(tagName == "Time")   asTimeline = true;
                        else                    asTimeline = false;
                        if((tagName.toLower().contains("date")) || (tagName.toLower().contains("time")))    asDate = true;
                        else                                                                                asDate = false;
                    }
                }
                else
                    tagName = "";

                if(sortSplit.count() > 1) {
                    QString rightPart = sortSplit.at(1);
                    if(rightPart.endsWith("d")) {
                        rightPart.chop(1);
                        sortAscending = false;
                    }
                    QStringList lefts = rightPart.split(",");
                    left = toDouble(lefts.at(0));
                    if(lefts.count() > 1)
                        leftLength = toDouble(lefts.at(1));
                }
            }
        }
    }

    QString text2 = ui->matches->text();
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++)
        if((!ui->checks->topLevelItem(i)->isHidden()) && (ui->checks->topLevelItem(i)->checkState(1) == Qt::Unchecked))
            text2 = ui->filter->currentText();
    emit(actionned(ui->filter->currentText(), text2));
}
void Sorting::actionSelection() {
    QString suffix = "all";
    if(ui->checks->selectedItems().count() > 1)
        suffix = "selection";

    ui->checkAll  ->setText(tr("Check %1").arg(suffix));
    ui->uncheckAll->setText(tr("Uncheck %1").arg(suffix));
    ui->invertAll ->setText(tr("Invert %1").arg(suffix));
}



const QString Sorting::getCriteria(const QString &criteria) const {
    if(criteria.isEmpty())
        return QString();

    if(asTimeline)
        return 0;

    bool asNumberGuess = false;
    qreal criteriaReal = toDouble(criteria, &asNumberGuess);
    if((!asDate) && (asNumberGuess))
        return QString("%1").arg(criteriaReal, 25, 'f', 5, QChar('0')).trimmed();

    return criteria.toLower();
}
const QString Sorting::getCriteriaFormated(qreal criteria) const {
    if(asTimeline)
        return timeToString(criteria);
    else {
        if(asNumber)
            return QString("%1").arg((criteria / 60.) * (asNumberRange.second - asNumberRange.first) + asNumberRange.first).trimmed();
        else if(qFloor(criteria/5) < criteriaFormatedRealCacheFormated.count())
            return criteriaFormatedRealCacheFormated.at(qFloor(criteria/5));
    }

    return QString();
}
qreal Sorting::getCriteriaFormatedReal(const QString &criteria, qreal timeValue) const {
    if(asTimeline)
        return timeValue;

    qreal val;
    bool asNumberGuess = false;
    qreal criteriaReal = toDouble(criteria, &asNumberGuess);
    if((!asDate) && (asNumberGuess))
        val = (criteriaReal - asNumberRange.first) / (asNumberRange.second - asNumberRange.first) * 60.;
    else {
        val = (qreal)criteriaFormatedRealCacheRaw.indexOf(criteria);
        if(val < 0) val = criteriaFormatedRealCacheRaw.count();
        val *= 5;
    }

    return val;
}
const QString Sorting::getCriteriaFormated(const QString &_criteria) {
    if(_criteria.isEmpty())
        return _criteria;


    bool asNumberGuess = false;
    qreal criteriaReal = toDouble(_criteria, &asNumberGuess);
    if((!asDate) && (asNumberGuess))
        return QString("%1").arg(criteriaReal, 'f').trimmed();

    QString criteria = _criteria, suffix;
    qint16 index = criteria.indexOf("\n");
    if(index > 0) {
        criteria = criteria.left(index);
        suffix   = criteria.right(criteria.length() - index - 1);
    }
    if(asDate) {
        if(criteriaFormatedCache.contains(criteria))
            return criteriaFormatedCache.value(criteria);
        else {
            QString retour;
            if(     criteria.length() <= 3)   retour = criteria;
            else if(criteria.length() <= 4)   retour = tr("%1's").arg(criteria.left(4), -4, '0');
            else if(criteria.length() <= 8)   retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("MMMM yyyy");
            else if(criteria.length() <= 11)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy");
            else if(criteria.length() <= 13)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh") + "h";
            else if(criteria.length() <= 15)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh:mm");
            else                              retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh:mm:ss");
            criteriaFormatedCache.insert(criteria, retour);
            return retour + suffix;
        }
    }
    else if(leftLength > 0)
        return criteria + suffix + "...";
    return criteria + suffix;
}



bool Sorting::isAcceptable(bool strongCheck, const QString &_criteria) const {
    if(asTimeline)
        return true;

    if(_criteria.isEmpty())
        return false;

    /*
    if(ui->filter->currentText().contains("author")) {
        for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
            qDebug("%s - %s - %s", qPrintable(_criteria), qPrintable(ui->checks->topLevelItem(i)->text(0)), qPrintable(ui->checks->topLevelItem(i)->text(1)));
        }
    }
    */

    if(strongCheck) {
        for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
            //if((ui->checks->topLevelItem(i)->checkState(1) == Qt::Unchecked) && ((ui->checks->topLevelItem(i)->text(0) == _criteria) || (ui->checks->topLevelItem(i)->text(1) == _criteria)))
            if((ui->checks->topLevelItem(i)->checkState(1) == Qt::Unchecked) && (ui->checks->topLevelItem(i)->text(0) == _criteria))
                return false;
        }
    }
    if(!ui->matches->text().isEmpty()) {
        QStringList matches = ui->matches->text().toLower().split(",", QString::SkipEmptyParts);
        foreach(const QString &match, matches) {
            QRegExp regexp;
            regexp.setPatternSyntax(QRegExp::Wildcard);
            regexp.setPattern(match.trimmed());

            QStringList criterias = _criteria.toLower().split(",", QString::SkipEmptyParts);
            foreach(const QString &criteria, criterias)
                if(regexp.indexIn(criteria.trimmed()) >= 0)
                    return true;
        }
        return false;
    }
    return true;
}
const QString Sorting::getAcceptableWithFilters(const QString &_criteria) const {
    QString retour;
    if(!ui->matches->text().isEmpty()) {
        QStringList matches = ui->matches->text().toLower().split(",", QString::SkipEmptyParts);
        foreach(const QString &match, matches) {
            QRegExp regexp;
            regexp.setPatternSyntax(QRegExp::Wildcard);
            regexp.setPattern(match.trimmed());

            QStringList criterias = _criteria.toLower().split(",", QString::SkipEmptyParts);
            foreach(const QString &criteria, criterias)
                if(regexp.indexIn(criteria.trimmed()) >= 0)
                    return criteria.trimmed();
        }
        return retour;
    }
    return _criteria;
}

const QString Sorting::getMatchName() const {
    return ui->matches->text();
}

Sorting::~Sorting() {
    delete ui;
}

void Sorting::addCheckStart() {
    isUpdating = true;
    criteriaFormatedRealCacheRaw.clear();
    criteriaFormatedRealCacheFormated.clear();
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if(!ui->checks->topLevelItem(i)->isHidden())
            ui->checks->topLevelItem(i)->setHidden(true);
    }
    ui->checks->setColumnHidden(0, true);
    ui->sorting->setVisible(false);
    ui->checks->setColumnHidden(2, true);
    isUpdating = false;
}
void Sorting::addCheck(const QString &sorting, const QString &sortingFormated, const QString &_complement) {
    if(sorting.isEmpty())
        return;

    QString complement;
    if(_complement != sortingFormated)
        complement = _complement;

    isUpdating = true;
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if(ui->checks->topLevelItem(i)->text(0) == sorting) {
            if(ui->checks->topLevelItem(i)->isHidden())                    ui->checks->topLevelItem(i)->setHidden(false);
            if((!sortingFormated.isEmpty()) && (ui->checks->topLevelItem(i)->text(1) != sortingFormated))
                ui->checks->topLevelItem(i)->setText(1, sortingFormated);
            if(ui->checks->topLevelItem(i)->text(2) != complement)         ui->checks->topLevelItem(i)->setText(2, complement);
            if(!complement.isEmpty()) {
                ui->checks->setColumnHidden(2, false);
                ui->sorting->setVisible(true);
                ui->checks->setColumnWidth(0, 150);
            }
            isUpdating = false;
            return;
        }
    }

    QTreeWidgetItem *checkItem = new QTreeWidgetItem(ui->checks, QStringList() << sorting << sortingFormated << complement);
    checkItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    checkItem->setCheckState(1, Qt::Checked);
    if(!complement.isEmpty()) {
        ui->checks->setColumnHidden(2, false);
        ui->sorting->setVisible(true);
        ui->checks->setColumnWidth(0, 150);
    }
    isUpdating = false;
}
void Sorting::addCheckEnd() {
    asNumber      = (!asDate);
    asNumberRange = qMakePair(9999999., -9999999.);
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if((!ui->checks->topLevelItem(i)->isHidden()) && (ui->checks->topLevelItem(i)->checkState(1) == Qt::Checked)) {
            criteriaFormatedRealCacheRaw << ui->checks->topLevelItem(i)->text(0);
            if(ui->checks->topLevelItem(i)->text(1).isEmpty())  criteriaFormatedRealCacheFormated << ui->checks->topLevelItem(i)->text(0);
            else                                                criteriaFormatedRealCacheFormated << ui->checks->topLevelItem(i)->text(1);
            if(!asDate) {
                bool testIfNumber = false;
                qreal val = toDouble(ui->checks->topLevelItem(i)->text(0), &testIfNumber);
                if(testIfNumber) {
                    asNumberRange.first  = qMin(asNumberRange.first,  val);
                    asNumberRange.second = qMax(asNumberRange.second, val);
                }
                asNumber &= testIfNumber;
            }
        }
    }
}


QDomElement Sorting::serialize(QDomDocument &xmlDoc) const {
    QDomElement xmlData = xmlDoc.createElement("sorting");
    xmlData.setAttribute("asNumber",        asNumber);
    xmlData.setAttribute("asDate",          asDate);
    xmlData.setAttribute("asTimeline",      asTimeline);
    xmlData.setAttribute("sortAscending",   sortAscending);
    xmlData.setAttribute("tagNameCategory", tagNameCategory);
    xmlData.setAttribute("tagName",         tagName);
    xmlData.setAttribute("left",            left);
    xmlData.setAttribute("leftLength",      leftLength);
    return xmlData;
}
void Sorting::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}



const QString Sorting::timeToString(qreal time) {
    QString timeStr = "";

    quint16 min = time / 60;
    if(min < 10)        timeStr += "0";
    else if(min < 100)  timeStr += "";
    timeStr += QString::number(min) + ":";

    quint8 sec = qFloor(time) % 60;
    if(sec < 10) timeStr += "0";
    timeStr += QString::number(sec);

    return timeStr;
}
qreal Sorting::stringToTime(const QString &timeStr) {
    qreal time = 0;
    if(timeStr.count()) {
        QStringList timeParts = timeStr.split(":");
        if(timeParts.count() > 1)
            time = timeParts.last().toDouble() + timeParts.at(timeParts.count() - 2).toDouble() * 60;
    }
    return time;
}

qreal Sorting::toDouble(const MetadataElement &elmt, bool *ok) {
    return toDouble(elmt.toString(), ok);
}
qreal Sorting::toDouble(const QString &str, bool *ok) {
    qint16 indexOfSpace = str.indexOf(" ");
    if(indexOfSpace > 0) {
        qreal val = str.left(indexOfSpace).toDouble(ok), factor = 1;
        QString unit = str.mid(indexOfSpace).trimmed();
        if(unit.length() > 1) {
            if((unit.startsWith("n")) || (unit.startsWith("N")))
                factor = 0.0000000001;
            else if(unit.startsWith("µ"))
                factor = 0.000001;
            else if(unit.startsWith("m"))
                factor = 0.001;
            else if((unit.startsWith("c")) || (unit.startsWith("C")))
                factor = 0.01;
            else if((unit.startsWith("k")) || (unit.startsWith("K")))
                factor = 1000;
            else if(unit.startsWith("M"))
                factor = 1000000;
            else if((unit.startsWith("t")) || (unit.startsWith("T")))
                factor = 1000000000;
        }
        val *= factor;
        return val;
    }
    return str.toDouble(ok);
}

void Sorting::reset(const QString &filterText, QString matchText, QStringList checksOnly) {
    for(quint16 i = 0 ; i < ui->filter->count() ; i++)
        if(ui->filter->itemText(i) == filterText) {
            ui->filter->setCurrentIndex(i);
            break;
        }
    action();
    QApplication::processEvents();

    ui->matches->setText(matchText);
    action();
    QApplication::processEvents();

    if(checksOnly.count()) {
        for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
            if(!ui->checks->topLevelItem(i)->isHidden()) {
                if(checksOnly.contains(ui->checks->topLevelItem(i)->text(0)))
                    ui->checks->topLevelItem(i)->setCheckState(0, Qt::Checked);
                else
                    ui->checks->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
            }
        }
    }
    action();
    QApplication::processEvents();
}
