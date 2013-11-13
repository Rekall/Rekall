#include "phases.h"
#include "ui_phases.h"

Phase::Phase(QTreeWidget *parent, const QDateTime &date, const QString &name)
    : QTreeWidgetItem(parent) {
    setText(0, name);
    setText(1, date.toString("yyyy/MM/dd hh:mm:ss"));
    setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

Phases::Phases(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Phases) {
    ui->setupUi(this);

    needCalulation = true;
    phasesByDaysTo.setAction(ui->daysTo);
}

Phases::~Phases() {
    delete ui;
}

void Phases::analyse() {
    ui->names->clear();
    QDateTime oldDate, startingDate, currentDate;
    quint16 index = 0;
    bool firstElement = true;
    QMapIterator<QString, MetadataElement> metaElementIterator(metaElements);
    while(metaElementIterator.hasNext()) {
        metaElementIterator.next();
        currentDate = metaElementIterator.value().toDateTime();
        if(firstElement)
            oldDate = startingDate = currentDate;

        if(oldDate.daysTo(currentDate) > phasesByDaysTo) {
            new Phase(ui->names, currentDate, QString("#%1").arg(index+1));
            index++;
            startingDate = currentDate;
        }
        oldDate = currentDate;
        firstElement = false;
    }
    currentDate = currentDate.addSecs(1);

    new Phase(ui->names, currentDate, QString("#%1").arg(index+1));
    needCalulation = false;
    action();
}

const QString Phases::getPhaseFor(const MetadataElement &value) {
    QString phaseForValue;
    if(value.isDate()) {
        for(quint16 i = 0 ; i < ui->names->topLevelItemCount() ; i++) {
            Phase *phase = (Phase*)ui->names->topLevelItem(i);
            if(value.toDateTime() < phase->getDate())
                return QString("%1").arg(i, 4, 10, QChar('0'));
        }
    }
    return phaseForValue;
}
const QString Phases::getVerbosePhaseFor(const QString &_phaseId) {
    quint16 phaseId = _phaseId.toUInt();
    if(phaseId < ui->names->topLevelItemCount())
        return ((Phase*)ui->names->topLevelItem(phaseId))->getName();
    return QString();
}


void Phases::actionNames() {
    ui->names->sortByColumn(1, Qt::AscendingOrder);
}
void Phases::action() {
    ui->names->sortByColumn(1, Qt::AscendingOrder);
    emit(actionned());
    needCalulation = true;
}


QDomElement Phases::serialize(QDomDocument &xmlDoc) {
    QDomElement xmlData = xmlDoc.createElement("phases");
    for(quint16 i = 0 ; i < ui->names->topLevelItemCount() ; i++) {
        Phase *phase = (Phase*)ui->names->topLevelItem(i);
        QDomElement phaseXml = xmlDoc.createElement("phase");
        phaseXml.setAttribute("date" , phase->getDateStr());
        phaseXml.setAttribute("name" , phase->getName());
    }
    return xmlData;
}
void Phases::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
