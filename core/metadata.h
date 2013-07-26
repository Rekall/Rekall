#ifndef METADATA_H
#define METADATA_H

#include <QMutex>
#include "misc/global.h"

enum DocumentType     { DocumentTypeFile, DocumentTypeVideo, DocumentTypeAudio, DocumentTypeImage, DocumentTypeDoc, DocumentTypeMarker };
enum DocumentFunction { DocumentFunctionContextual, DocumentFunctionRender };
enum DocumentStatus   { DocumentStatusWaiting, DocumentStatusProcessing, DocumentStatusReady };


class MetadataWaveform : public QList< QPair<qreal,qreal> > {
public:
    explicit MetadataWaveform() { normalisation = 1; }
public:
    qreal normalisation;
};



class Metadata : public QObject {
    Q_OBJECT
    
public:
    explicit Metadata(QObject *parent = 0, bool createEmpty = false);
    ~Metadata();

public:
    DocumentFunction function;
    QFileInfo file;
    DocumentStatus status;
protected:
    QDir dirBase;
    QList<QMetaDictionnay> metadatas;


public:
    qreal mediaDuration;
    DocumentType type;
    Thumbnails thumbnails;
    MetadataWaveform waveform;
protected:
    QMutex metadataMutex;
public:
    bool updateFile(const QFileInfo &file, qint16 version = -1, quint16 falseInfoForTest = 0);
    bool updateImport(const QString &name, qint16 version = -1);
    void updateFeed();
    const QPair<QString, QPixmap> getThumbnail(qint16 version = -1);
    const QPair<QString,QString> getGps();
public:
    inline qint16 getMetadataIndexVersion(qint16 version = -1) const {
        if(version < 0) return getMetadataCountM();
        else            return qMin(version, getMetadataCountM());
    }
    inline const QMetaDictionnay& getMetadata(qint16 version = -1) const { return metadatas.at(getMetadataIndexVersion(version)); }
    inline qint16 getMetadataCount()                               const { return metadatas.count(); }
    inline qint16 getMetadataCountM()                              const { return metadatas.count()-1; }
public:
    const MetadataElement getMetadata(const QString &key, qint16 version = -1);
    const MetadataElement getMetadata(const QString &category,const QString &key, qint16 version = -1);
public:
    void setMetadata(const QString &category, const QString &key, const QString &value, qint16 version);
    void setMetadata(const QString &category, const QString &key, const QDateTime &value, qint16 version);
    void setMetadata(const QString &category, const QString &key, const MetadataElement &value, qint16 version);
    void setMetadata(const QString &category, const QString &key, qreal value, qint16 version);
    void setMetadata(const QMetaDictionnay &metaDictionnay);
public:
    const QString getCriteriaSort(qint16 version = -1);
    const QString getCriteriaCluster(qint16 version = -1);
    const MetadataElement getCriteriaSortRaw(qint16 version);
    const QString getCriteriaColor(qint16 version = -1);
    const QString getCriteriaSortFormated(qint16 version = -1);
    const QString getCriteriaColorFormated(qint16 version = -1);
    const QString getCriteriaClusterFormated(qint16 version = -1);
    bool isAcceptableWithSortFilters(qint16 version = -1);
    bool isAcceptableWithColorFilters(qint16 version = -1);
    bool isAcceptableWithClusterFilters(qint16 version = -1);
    const QString getAcceptableWithClusterFilters(qint16 version = -1);

public:
    void debug();
    QDomElement serializeMetadata(QDomDocument &xmlDoc);
    void deserializeMetadata(const QDomElement &xmlElement);

public:
    static QStringList suffixesTypeVideo, suffixesTypeDoc, suffixesTypeImage, suffixesTypeAudio, suffixesTypePatches;
};

#endif // METADATA_H
