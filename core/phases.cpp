#include "phases.h"
#include "ui_phases.h"

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
    phasesByDate.clear();

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
            phasesByDate.insert(currentDate, QString("Phase %1 : %2 to %3").arg(index).arg(startingDate.toString("yyyy-MM-dd hh:mm:ss")).arg(currentDate.toString("yyyy-MM-dd hh:mm:ss")));
            index++;
            startingDate = currentDate;
        }
        oldDate = currentDate;
        firstElement = false;
    }
    currentDate = currentDate.addSecs(1);
    phasesByDate.insert(currentDate, QString("Phase %1 : after %2").arg(index).arg(currentDate.toString("yyyy-MM-dd hh:mm:ss")));
}

const QString Phases::getPhaseFor(const MetadataElement &value) {
    QString phaseForValue;
    if(value.isDate()) {
        QMapIterator<QDateTime, QString> phaseIterator(phasesByDate);
        while(phaseIterator.hasNext()) {
            phaseIterator.next();
            phaseForValue = phaseIterator.value();
            if(value.toDateTime() < phaseIterator.key())
                return phaseForValue;
        }
    }
    return phaseForValue;
}


void Phases::action() {
    emit(actionned());
}



QDomElement Phases::serialize(QDomDocument &xmlDoc) {
    QDomElement xmlData = xmlDoc.createElement("phases");
    QMapIterator<QDateTime, QString> phaseIterator(phasesByDate);
    while(phaseIterator.hasNext()) {
        phaseIterator.next();
        QDomElement phaseXml = xmlDoc.createElement("phase");
        phaseXml.setAttribute("date" , phaseIterator.key().toString("yyyy-MM-dd hh:mm:ss"));
        phaseXml.setAttribute("name" , phaseIterator.value());
    }
    return xmlData;
}
void Phases::deserialize(const QDomElement &xmlElement) {
    QString a = xmlElement.attribute("attribut");
}
