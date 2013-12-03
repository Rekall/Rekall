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
        ui->filter->addItem("Time", "Time");
    ui->filter->addItem("Date (year)",  "Rekall->Date/Time | 0,4");
    ui->filter->addItem("Date (month)", "Rekall->Date/Time | 0,7");
    ui->filter->addItem("Date (day)",   "Rekall->Date/Time | 0,10");  // 1234:67:90 23:56:89
    ui->filter->addItem("Time (hours)", "Rekall->Date/Time | 11,2f");  // 0123:56:89012:45:67
    ui->filter->addItem("Type",         "Rekall->Type");
    ui->filter->addItem("Authors",      "Rekall->Author");
    ui->filter->addItem("All Keywords", "Rekall->All");
    ui->filter->addItem("Fullname",     "Rekall->Name");
    ui->filter->addItem("First letter (name)", "Rekall->Name | 0,1");
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



void Sorting::setTagname(const QString &_tagName) {
    if(_tagName.count()) {
        QStringList tagNames = _tagName.split("->");
        if(tagNames.count()) {
            tagName       = tagNames.last();
            if(tagNames.count() > 1)
                tagNameCategory = tagNames.first();
            if(tagName == "Time")   asTimeline = true;
            else                    asTimeline = false;
            leftLength    = -1;
            asNumber      = false;
            sortAscending = true;
            if((tagName.toLower().contains("date")) || (tagName.toLower().contains("time")))    asDate = true;
            else                                                                                asDate = false;
        }
    }
    else
        tagName = "";
}

void Sorting::action() {
    if(needWord) {
        bool needWordValue = !(ui->matches->text().isEmpty());
        ui->checks    ->setVisible(needWordValue);
        ui->checkAll  ->setVisible(needWordValue);
        ui->invertAll ->setVisible(needWordValue);
        ui->uncheckAll->setVisible(needWordValue);
    }
    if(isUpdating)
        return;

    if((sender() == ui->checkAll) || (sender() == ui->uncheckAll) || (sender() == ui->invertAll)) {
        QList<QTreeWidgetItem*> items;
        if(ui->checks->selectedItems().count() > 1)
            items = ui->checks->selectedItems();
        else {
            for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++)
                if(!ui->checks->topLevelItem(i)->isHidden())
                    items << ui->checks->topLevelItem(i);
        }
        foreach(QTreeWidgetItem *item, items) {
            if(     (sender() == ui->checkAll)   && (item->checkState(0) == Qt::Unchecked))
                item->setCheckState(0, Qt::Checked);
            else if((sender() == ui->uncheckAll) && (item->checkState(0) == Qt::Checked))
                item->setCheckState(0, Qt::Unchecked);
            else if((sender() == ui->invertAll)  && (item->checkState(0) == Qt::Checked))
                item->setCheckState(0, Qt::Unchecked);
            else if((sender() == ui->invertAll)  && (item->checkState(0) == Qt::Unchecked))
                item->setCheckState(0, Qt::Checked);
        }
    }
    else {
        if(ui->filter->count()) {
            QString filterText = ui->filter->itemData(ui->filter->currentIndex()).toString();
            if(filterText.isEmpty())
                filterText = ui->filter->currentText();

            if((needWord) && (ui->matches->text().isEmpty())) {
                tagName         = "";
                tagNameCategory = "";
            }
            else if(filterText.count()) {
                QStringList sortSplit = filterText.split("|");
                setTagname(sortSplit.first().trimmed());
                if(sortSplit.count() > 1) {
                    QString rightPart = sortSplit.at(1);
                    if(rightPart.endsWith("d")) {
                        rightPart.chop(1);
                        sortAscending = false;
                    }
                    if(rightPart.endsWith("f")) {
                        rightPart.chop(1);
                        asNumber = true;
                    }
                    QStringList lefts = rightPart.split(",");
                    left = lefts.at(0).toDouble();
                    if(lefts.count() > 1)
                        leftLength = lefts.at(1).toDouble();
                }
            }
        }
    }

    QString text2 = ui->matches->text();
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++)
        if((!ui->checks->topLevelItem(i)->isHidden()) && (ui->checks->topLevelItem(i)->checkState(0) == Qt::Unchecked))
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


const QString Sorting::getCriteriaFormated(const QString &_criteria) {
    if(_criteria.isEmpty())
        return _criteria;

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
const QString Sorting::getCriteriaFormated(qreal criteria) const {
    if(asTimeline)
        return timeToString(criteria);
    else if(qFloor(criteria/5) < criteriaFormatedRealCache.count())
        return criteriaFormatedRealCache.at(qFloor(criteria/5));
    return QString("");
}
qreal Sorting::getCriteriaFormatedReal(const QString &_criteria) const {
    qreal val = (qreal)criteriaFormatedRealCache.indexOf(_criteria);
    if(val < 0) val = criteriaFormatedRealCache.count();
    return val;
}
bool Sorting::isAcceptable(bool strongCheck, const QString &_criteria) const {
    if(asTimeline)
        return true;

    if(strongCheck) {
        for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
            if((ui->checks->topLevelItem(i)->checkState(0) == Qt::Unchecked) && (ui->checks->topLevelItem(i)->text(0) == _criteria))
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
    qDebug("%s", qPrintable(this->windowTitle()));
    criteriaFormatedRealCache.clear();
    isUpdating = true;
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if(!ui->checks->topLevelItem(i)->isHidden())
            ui->checks->topLevelItem(i)->setHidden(true);
    }
    ui->checks->setColumnHidden(1, true);
    isUpdating = false;
}
void Sorting::addCheck(const QString &check, const QString &value) {
    if(check.isEmpty())
        return;

    isUpdating = true;
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if(ui->checks->topLevelItem(i)->text(0) == check) {
            if(ui->checks->topLevelItem(i)->isHidden())         ui->checks->topLevelItem(i)->setHidden(false);
            if(ui->checks->topLevelItem(i)->text(1) != value)   ui->checks->topLevelItem(i)->setText(1, value);
            if(!value.isEmpty()) {
                ui->checks->setColumnHidden(1, false);
                ui->checks->setColumnWidth(0, 150);
            }
            isUpdating = false;
            return;
        }
    }

    QTreeWidgetItem *checkItem = new QTreeWidgetItem(ui->checks, QStringList() << check << value);
    checkItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    checkItem->setCheckState(0, Qt::Checked);
    if(!value.isEmpty()) {
        ui->checks->setColumnHidden(1, false);
        ui->checks->setColumnWidth(0, 150);
    }
    isUpdating = false;
}
void Sorting::addCheckEnd() {
    for(quint16 i = 0 ; i < ui->checks->topLevelItemCount() ; i++) {
        if((!ui->checks->topLevelItem(i)->isHidden()) && (ui->checks->topLevelItem(i)->checkState(0) == Qt::Checked))
            criteriaFormatedRealCache << ui->checks->topLevelItem(i)->text(0);
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
