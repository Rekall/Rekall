#ifndef CLUSTER_H
#define CLUSTER_H

#include <QObject>
#include "tag.h"

class Cluster : public QObject {
    Q_OBJECT

public:
    explicit Cluster(QObject *parent = 0);

private:
    const Cluster* linkedCluster;
public:
    inline void setLinkedCluster(const Cluster* _linkedCluster) { linkedCluster = _linkedCluster; }

private:
    QList<Tag*> tags;
public:
    void clear();
    void add(Tag *tag);

private:
    QRectF timelineBoundingRect;
    qreal  animation, animationDest;
public:
    void paintTimeline();

};

#endif // CLUSTER_H
