#include "person.h"

Person::Person(PersonCard _card, QTreeWidget *parent) :
    QTreeWidgetItem(parent) {
    updateCard(_card);
}
void Person::updateCard(PersonCard _card) {
    card = _card;
    setIcon(0, QIcon(card.getPhoto()));
    setText(1, card.getFirstname());
    setText(2, card.getLastname());
    setText(3, card.getRole());
}


QList<Person*> Person::fromString(QTreeWidget *parent, const QString &text) {
    QList<Person*> persons;

    QList<QString> cards = text.split("END:VCARD");
    foreach(QString card, cards) {
        if(card.trimmed().startsWith("BEGIN:VCARD")) {
            card.remove("BEGIN:VCARD");
            PersonCard personCard;
            QList<QString> cardInfos = card.remove("\n ").split("\n", QString::SkipEmptyParts);
            foreach(const QString &cardInfo, cardInfos) {
                qint16 i = cardInfo.indexOf(":");
                if(i >= 0) {
                    QPair<PersonCardHeader, PersonCardValues> personCardInfo;
                    personCardInfo.first  = PersonCardHeader::fromString(cardInfo.left(i) .split(";", QString::SkipEmptyParts));
                    personCardInfo.second = PersonCardValues::fromString(cardInfo.mid(i+1).split(";", QString::SkipEmptyParts));
                    if((personCardInfo.first.category == "photo") && (personCardInfo.second.count()))
                        personCardInfo.second.pixmap = QPixmap::fromImage(QImage::fromData(QByteArray::fromBase64(personCardInfo.second.first().toLatin1()), "jpeg"));
                    personCard.insert(personCardInfo.first.category, personCardInfo);
                }
            }
            personCard.debug();
            persons.append(new Person(personCard, parent));
        }
    }
    return persons;
}
QList<Person*> Person::fromFile(QTreeWidget *parent, const QString &file) {
    QFile f(file);
    QString text;
    if(f.open(QFile::ReadOnly)) {
        text = f.readAll();
        f.close();
    }
    return fromString(parent, text);
}
