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
