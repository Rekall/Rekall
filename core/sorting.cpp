#include "sorting.h"
#include "ui_sorting.h"

Sorting::Sorting(bool _isFilter, const QString &title, quint16 index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sorting) {
    isFilter = _isFilter;
    regexp.setPatternSyntax(QRegExp::Wildcard);
    ui->setupUi(this);
    ui->title->setText(title);
    action();
    if(isFilter)    ui->stackedWidget->setCurrentIndex(1);
    else            ui->stackedWidget->setCurrentIndex(0);

    ui->filter->addItem("", "");
    ui->filter->addItem("documents date (year)",  "Document Date/Time | 0,4");
    ui->filter->addItem("documents date (month)", "Document Date/Time | 0,7");
    ui->filter->addItem("documents date (day)",   "Document Date/Time | 0,10");  // 1234:67:90 23:56:89
    ui->filter->addItem("documents time (hours)", "Document Date/Time | 11,2");  // 9876:43:10 87:54:21
    ui->filter->addItem("documents type",         "Document Category");
    ui->filter->addItem("documents keywords",     "Document Keywords");
    ui->filter->addItem("documents fullname",     "Document Name");
    ui->filter->addItem("documents first letter", "Document Name | 1");
    ui->filter->addItem("users names",            "Document Author");
    ui->filter->setCurrentIndex(index);
}

void Sorting::setTagname(const QString &_tagName) {
    tagName       = _tagName;
    left          = 0;
    leftLength    = -1;
    asNumber      = false;
    displayLinked = false;
    sortAscending = true;
    if((tagName.toLower().contains("date")) || (tagName.toLower().contains("time")))    asDate = true;
    else                                                                                asDate = false;
}

void Sorting::action() {
    if(!isFilter) {
        QString filterText = ui->filter->itemData(ui->filter->currentIndex()).toString();
        if(filterText.isEmpty())
            filterText = ui->filter->currentText();

        QStringList sortSplit = filterText.split("|");
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
                QStringList lefts = leftPart.split(",");
                left = lefts.at(0).toDouble();
                if(lefts.count() > 1)
                    leftLength = lefts.at(1).toDouble();
            }
        }
    }
    emit(actionned());
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
    xmlData.setAttribute("leftLength",    leftLength);
    return xmlData;
}
void Sorting::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
