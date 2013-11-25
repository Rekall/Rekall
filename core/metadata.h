#ifndef METADATA_H
#define METADATA_H

#include <QMutex>
#include "misc/global.h"

enum DocumentType     { DocumentTypeFile, DocumentTypeVideo, DocumentTypeAudio, DocumentTypeImage, DocumentTypeDoc, DocumentTypeMarker, DocumentTypePeople };
enum DocumentFunction { DocumentFunctionContextual, DocumentFunctionRender };
enum DocumentStatus   { DocumentStatusWaiting, DocumentStatusProcessing, DocumentStatusReady };


class MetadataWaveform : public QList< QPair<qreal,qreal> > {
public:
    explicit MetadataWaveform() { normalisation = 1; }
public:
    qreal normalisation;
};

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
    QImage photo;
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

class PersonCard : public QList< QPair<PersonCardHeader, PersonCardValues> > {
public:
    explicit PersonCard() : QList< QPair<PersonCardHeader, PersonCardValues> > ()                      {                    }
    explicit PersonCard(const PersonCard &card) : QList< QPair<PersonCardHeader, PersonCardValues> >() {    *this = card;   }
    inline bool check(const QString &category, quint16 count = 0)         const { return ((containsCategory(category)) && (valueCategory(category).second.count() > count));  }
    inline const PersonCardValues getRow(const QString &category)         const { return valueCategory(category).second; }
    inline const QString& get(const QString &category, quint16 index = 0) const { return valueCategory(category).second.at(index); }

public:
    bool containsCategory(const QString &category) const {
        for(quint16 i = 0 ; i < count() ; i++)
            if(at(i).first.category == category)
                return true;
        return false;
    }
    QPair<PersonCardHeader, PersonCardValues> valueCategory(const QString &category) const {
        for(quint16 i = 0 ; i < count() ; i++)
            if(at(i).first.category == category)
                return at(i);
        return QPair<PersonCardHeader, PersonCardValues>();
    }

    inline const QString getFullname() const {
        if(check("fn", 1))      return get("fn");
        else if((check("n", 1)) && (check("n")))  return get("n", 1) + " " + get("n");
        else if(check("n", 1))  return get("n", 1);
        else if(check("n"))     return get("n");
        return "";
    }
    inline const QString getFirstname() const {
        if(check("n", 1))       return get("n", 1);
        else if(check("fn"))    return get("fn");
        return "";
    }
    inline const QString getLastname() const  {
        if(check("n"))          return get("n");
        return "";
    }
    inline const QImage getPhoto() const {
        if(check("photo"))      return getRow("photo").photo;
        return QImage();
    }
};



class Metadata : public QObject {
    Q_OBJECT
    
public:
    explicit Metadata(QObject *parent = 0, bool createEmpty = false);
    ~Metadata();

public:
    DocumentFunction function;
    QFileInfo        file;
    QImage           photo;
    DocumentStatus   status;


protected:
    QDir dirBase;
    QList<QMetaDictionnay> metadatas;
    QMutex metadataMutex;
public:
    qreal mediaDuration;
    DocumentType type;
    Thumbnails thumbnails;
    MetadataWaveform waveform;

public:
    bool updateFile(const QFileInfo &file, qint16 version = -1, quint16 falseInfoForTest = 0);
    bool updateImport(const QString &name, qint16 version = -1);
    bool updateCard(const PersonCard &card, qint16 version = -1);
    void updateFeed();
    const QPair<QString, QPixmap> getThumbnail(qint16 version = -1);
    const QList< QPair<QString,QString> > getGps();
    inline const QColor getColor() {
        if(Global::colorForMeta.contains(getCriteriaColorFormated()))
            return Global::colorForMeta.value(getCriteriaColorFormated()).first;
        else if(function == DocumentFunctionRender)
            return QColor(226, 226, 226);
        else
            return QColor(226, 226, 226, 46);
    }


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
    const QString getCriteriaSortFormated(qint16 version = -1);
    const MetadataElement getCriteriaSortRaw(qint16 version);
    const QString getCriteriaCluster(qint16 version = -1);
    const QString getCriteriaClusterFormated(qint16 version = -1);
    const QString getCriteriaColor(qint16 version = -1);
    const QString getCriteriaColorFormated(qint16 version = -1);
    const QString getCriteriaFilter(qint16 version = -1);
    bool isAcceptableWithSortFilters(qint16 version = -1);
    bool isAcceptableWithColorFilters(qint16 version = -1);
    bool isAcceptableWithClusterFilters(qint16 version = -1);
    const QString getAcceptableWithClusterFilters(qint16 version = -1);

public:
    void debug();
    QDomElement serializeMetadata(QDomDocument &xmlDoc);
    void deserializeMetadata(const QDomElement &xmlElement);

public:
    static QStringList suffixesTypeVideo, suffixesTypeDoc, suffixesTypeImage, suffixesTypeAudio, suffixesTypePatches, suffixesTypePeople;
};

#endif // METADATA_H
