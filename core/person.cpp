#include "person.h"

Person::Person(const PersonCard &card, QObject *parent) :
    Metadata(parent, true), PersonCard(card), QTreeWidgetItem() {
    updateCard(*this, getMetadataCountM());
    updateGUINeeded = true;
}
void Person::updateGUI() {
    if(updateGUINeeded) {
        updateGUINeeded = false;
        setIcon(0, QIcon(QPixmap::fromImage(getPhoto())));
        setText(1, getFullname());
    }
}


QList<Person*> Person::fromString(const QString &text, QObject *parent) {
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
                        personCardInfo.second.photo = QImage::fromData(QByteArray::fromBase64(personCardInfo.second.first().toLatin1()), "jpeg");
                    personCard.append(personCardInfo);
                }
            }
            //personCard.debug();
            persons.append(new Person(personCard, parent));
        }
    }
    return persons;
}
QList<Person*> Person::fromFile(const QString &file, QObject *parent) {
    QFile f(file);
    QString text;
    if(f.open(QFile::ReadOnly)) {
        text = f.readAll();
        f.close();
    }
    return fromString(text, parent);
}
