#ifndef PERSON_H
#define PERSON_H

#include <QTreeWidgetItem>
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QMap>
#include <QFile>
#include <QDateTime>

class PersonCardHeader : public QMap<QString, QString> {
public:
    QString category;
public:
    static PersonCardHeader fromString(const QStringList &vals) {
        PersonCardHeader header;
        if(vals.count()) {
            header.category = vals.first().trimmed().toLower();
            for(quint16 i = 1 ; i < vals.count() ; i++) {
                qint16 j = vals.at(i).trimmed().indexOf("=");
                if(j >= 0)
                    header.insert(vals.at(i).left(j).trimmed().toLower(), vals.at(i).mid(j+1).trimmed());
            }
        }
        return header;
    }
    const QString toString() const {
        if(count()) {
            QString retour = category + " (";
            QMapIterator<QString,QString> typeIterator(*this);
            while(typeIterator.hasNext()) {
                typeIterator.next();
                retour += typeIterator.key() + "=" + typeIterator.value() + ", ";
            }
            retour.chop(2);
            retour += ")";
            return retour;
        }
        return category;
    }
};
class PersonCardValues : public QStringList {
public:
    QPixmap pixmap;
    QDateTime dateTime;
public:
    static PersonCardValues fromString(const QStringList &vals) {
        PersonCardValues values;
        foreach(const QString &val, vals)
            values << val.trimmed();
        return values;
    }
    const QString toString() const {
        if(count()) {
            QString retour;
            foreach(const QString &valeur, *this)
                retour += valeur + "; ";
            retour.chop(2);
            return retour;
        }
        return "";
    }
};

class PersonCard : public QMap<QString, QPair<PersonCardHeader, PersonCardValues> > {
public:
    inline bool check(const QString &category, quint16 count = 0)         const { return ((contains(category)) && (value(category).second.count() > count));  }
    inline const PersonCardValues& getRow(const QString &category)        const { return value(category).second; }
    inline const QString& get(const QString &category, quint16 index = 0) const { return value(category).second.at(index); }

    inline const QString getFirstname() const {
        if(check("n", 1))       return get("n", 1);
        else if(check("fn"))    return get("fn");
        return "";
    }
    inline const QString getLastname() const  {
        if(check("n"))          return get("n");
        return "";
    }
    inline const QString getRole() const  {
        if(check("title"))      return get("title");
        return "";
    }
    inline const QPixmap getPhoto() const {
        if(check("photo"))      return getRow("photo").pixmap;
        return QPixmap();
    }

public:
    void debug() const  {
        QMapIterator<QString, QPair<PersonCardHeader, PersonCardValues> > personCardInfoIterator(*this);
        qDebug("%s", qPrintable(QString("Card of %1 %2 (%3)").arg(getFirstname()).arg(getLastname()).arg(getRole())));
        while(personCardInfoIterator.hasNext()) {
            personCardInfoIterator.next();
            qDebug("%s", qPrintable(personCardInfoIterator.value().first.toString()));
            foreach(const QString &value, personCardInfoIterator.value().second)
                qDebug("\t%s", qPrintable(value));
        }
    }
};




class Person : public QObject, public QTreeWidgetItem {
    Q_OBJECT

public:
    explicit Person(PersonCard _card, QTreeWidget *parent = 0);

public:
    PersonCard card;

public:
    void updateCard(PersonCard _card);

public:
    static QList<Person*> fromString(QTreeWidget *parent, const QString &text);
    static QList<Person*> fromFile  (QTreeWidget *parent, const QString &file);

};

#endif // PERSON_H
