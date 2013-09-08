#ifndef PERSON_H
#define PERSON_H

#include <QTreeWidgetItem>
#include <QObject>
#include <QImage>
#include <QLabel>
#include <QMap>
#include <QFile>
#include <QDateTime>
#include "core/metadata.h"

class Person : public Metadata, public PersonCard, public QTreeWidgetItem {
    Q_OBJECT

public:
    explicit Person(const PersonCard &card, QObject *parent = 0);

private:
    bool needGUIupdate;

public:
    void updateGUI();

public:
    static QList<Person*> fromString(const QString &text, QObject *parent = 0);
    static QList<Person*> fromFile  (const QString &file, QObject *parent = 0);

};

#endif // PERSON_H
