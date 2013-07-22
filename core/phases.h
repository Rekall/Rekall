#ifndef PHASES_H
#define PHASES_H

#include <QWidget>
#include "misc/options.h"
#include "qmath.h"

namespace Ui {
class Phases;
}

class Phases : public QWidget {
    Q_OBJECT
    
public:
    explicit Phases(QWidget *parent = 0);
    ~Phases();


public:
    QMap<QDateTime, QString> phasesByDate;


public:
    UiReal phasesByDaysTo;
    QMap<QString, MetadataElement> metaElements;
    bool needCalulation;
public:
    void addToMetaElements(const MetadataElement &value) { metaElements.insert(value.toString(), value); }
    void analyse();

public:
    const QString getPhaseFor(const MetadataElement &value);

public:
    QDomElement serialize(QDomDocument &xmlDoc);
    void deserialize(const QDomElement &xmlElement);

public slots:
    void action();
signals:
    void actionned();

private:
    Ui::Phases *ui;
};

#endif // PHASES_H
