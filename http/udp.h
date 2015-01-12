/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

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

#ifndef UDP_H
#define UDP_H

#include <QWidget>
#include <QByteArray>
#include <QUdpSocket>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class UdpDatagram;

class Udp : public QObject {
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
    explicit Udp(QObject *parent = 0, quint16 _port = 23411);
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
};

#endif // UDP_H
