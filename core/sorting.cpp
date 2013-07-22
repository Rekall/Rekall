#include "sorting.h"
#include "ui_sorting.h"

Sorting::Sorting(bool _isFilter, const QString &title, quint16 index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sorting) {
    isFilter = _isFilter;
    regexp.setPatternSyntax(QRegExp::Wildcard);
    ui->setupUi(this);
    ui->title->setText(title);
    ui->filter->setCurrentIndex(index);
    action();
    if(isFilter) {
        ui->stackedWidget->setCurrentIndex(1);
        displayLinked.setAction(ui->history);
    }
    else
        ui->stackedWidget->setCurrentIndex(0);
}

void Sorting::setTagname(const QString &_tagName) {
    tagName       = _tagName;
    left          = -1;
    asNumber      = false;
    displayLinked = false;
    sortAscending = true;
    if((tagName.toLower().contains("date")) || (tagName.toLower().contains("time")))    asDate = true;
    else                                                                                asDate = false;
}

void Sorting::action() {
    if(!isFilter) {
        QStringList sortSplit = ui->filter->currentText().split("|");
        if(sortSplit.count()) {
            setTagname(sortSplit.first().trimmed());
            if(sortSplit.count() > 1) {
                QString leftPart = sortSplit.at(1);
                if(leftPart.endsWith("d")) {
                    leftPart.chop(1);
                    sortAscending = false;
                }
                if(leftPart.endsWith("f")) {
                    leftPart.chop(1);
                    asNumber = true;
                }
                left = leftPart.toDouble();
            }
        }
    }
    emit(actionned());
}

const QString Sorting::getCriteriaFormated(const QString &criteria) {
    if(criteria.isEmpty())
        return criteria;
    else if(asDate) {
        if(criteriaFormatedCache.contains(criteria))
            return criteriaFormatedCache.value(criteria);
        else {
            QString retour;
            if(     criteria.length() <= 5)   retour = tr("%1's").arg(criteria.left(4), -4, '0');
            else if(criteria.length() <= 8)   retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("MMMM yyyy");
            else if(criteria.length() <= 11)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy");
            else if(criteria.length() <= 13)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh") + "h";
            else if(criteria.length() <= 15)  retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh:mm");
            else                              retour = QDateTime::fromString(criteria, QString("yyyy:MM:dd hh:mm:ss").left(criteria.length())).toString("dddd dd MMMM yyyy, hh:mm:ss");
            criteriaFormatedCache.insert(criteria, retour);
            return retour;
        }
    }
    else if(left >= 0)
        return criteria + "...";
    return criteria;
}
bool Sorting::isAcceptableWithFilters(const QString &_criteria) {
    if(!ui->matches->text().isEmpty()) {
        QStringList matches = ui->matches->text().toLower().split(",", QString::SkipEmptyParts);
        foreach(const QString &match, matches) {
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
const QString Sorting::getAcceptableWithFilters(const QString &_criteria) {
    QString retour;
    if(!ui->matches->text().isEmpty()) {
        QStringList matches = ui->matches->text().toLower().split(",", QString::SkipEmptyParts);
        foreach(const QString &match, matches) {
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



QDomElement Sorting::serialize(QDomDocument &xmlDoc) {
    QDomElement xmlData = xmlDoc.createElement("sorting");
    xmlData.setAttribute("isFilter",      isFilter);
    xmlData.setAttribute("asNumber",      asNumber);
    xmlData.setAttribute("asDate",        asDate);
    xmlData.setAttribute("sortAscending", sortAscending);
    xmlData.setAttribute("displayLinked", displayLinked);
    xmlData.setAttribute("tagName",       tagName);
    xmlData.setAttribute("left",          left);
    return xmlData;
}
void Sorting::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
