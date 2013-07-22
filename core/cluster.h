#ifndef CLUSTER_H
#define CLUSTER_H

#include <QObject>
#include "tag.h"

class Cluster : public QObject {
    Q_OBJECT

public:
    explicit Cluster(QObject *parent = 0);

public:
    QRectF timelineBoundingRect;
    const Cluster* linkedCluster;
    qreal animation, animationDest;
    QList<Tag*> tags;
public:
    void clear();
    void add(Tag *tag);
    void paintTimeline();
    
};

#endif // CLUSTER_H
