#ifndef UDP_H
#define UDP_H

#include <QWidget>
#include <QByteArray>
#include <QUdpSocket>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

namespace Ui {
class Udp;
}

class UdpDatagram;

class Udp : public QWidget {
    Q_OBJECT

private:
    quint16 port;
    QUdpSocket *socket;
    quint8  parsingBufferI[4096];
    quint16 parsingBufferISize;
    char    parsingAddressBuffer[255];
    quint8  parsingArgumentsBuffer[255];
    quint16 parsingIndexAddressBuffer, parsingIndexArgumentsBuffer;

public:
    explicit Udp(QWidget *parent, quint16 _port);
    ~Udp();

public:
    void open();
    void send(const QString &ip, quint16 port, const QString &destination, const QList<QVariant> &valeurs = QList<QVariant>());

signals:
    void readyUdp();

signals:
    void outgoingMessage(const QString &ip, quint16 port, const QString &destination, const QList<QVariant> &valeurs);

public slots:
    void incomingMessage(const QString &, quint16, const QString &, const QList<QVariant> &) {}
private slots:
    void parseOSC();
private:
    inline void pad(QByteArray & b) const {
        while (b.size() % 4 != 0)
            b += (char)0;
    }

private:
    Ui::Udp *ui;
};

#endif // UDP_H
