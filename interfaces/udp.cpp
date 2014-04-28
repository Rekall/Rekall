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

#include "udp.h"
#include "ui_udp.h"

Udp::Udp(QWidget *parent, quint16 _port) :
    QWidget(parent),
    ui(new Ui::Udp) {
    ui->setupUi(this);
    port = _port;

    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), SLOT(parseOSC()));

    //show();

    open();
}

void Udp::open() {
    if(socket->localPort() < 1024) {
        if(socket->bind(port))  ui->status->setStyleSheet("background-color: #B7FF00;");
        else                    ui->status->setStyleSheet("background-color: #FF2D00;");
    }
}

Udp::~Udp() {
    delete ui;
}

void Udp::parseOSC() {
    //Parse all UDP datagram
    while(socket->hasPendingDatagrams()) {
        //Extract host, port & UDP datagram
        QHostAddress receivedHost;
        quint16 receivedPort;
        parsingBufferISize = socket->readDatagram((char*)parsingBufferI, 4096, &receivedHost, &receivedPort);

        quint16 indexBuffer = 0;

        //Parse UDP content
        while(indexBuffer < parsingBufferISize) {
            parsingIndexAddressBuffer = 0;
            parsingIndexArgumentsBuffer = 0;

            //Looking for '/'
            while((indexBuffer < parsingBufferISize) && (parsingBufferI[indexBuffer] != '/'))
                indexBuffer++;

            //Parse header
            if((parsingBufferI[indexBuffer] == '/') && (parsingBufferISize%4 == 0)) {
                //OSC Adress
                while((indexBuffer < parsingBufferISize) && (parsingBufferI[indexBuffer] != 0))
                    parsingAddressBuffer[parsingIndexAddressBuffer++] = parsingBufferI[indexBuffer++];
                parsingAddressBuffer[parsingIndexAddressBuffer] = 0;

                //Looking for ','
                while((indexBuffer < parsingBufferISize) && (parsingBufferI[indexBuffer++] != ',')) {}

                //OSC arguments type
                indexBuffer--;
                while((indexBuffer < parsingBufferISize) && (parsingBufferI[++indexBuffer] != 0))
                    parsingArgumentsBuffer[parsingIndexArgumentsBuffer++] = parsingBufferI[indexBuffer];
                parsingArgumentsBuffer[parsingIndexArgumentsBuffer] = 0;
                indexBuffer++;

                //Index modulo 4
                while((indexBuffer < parsingBufferISize) && ((indexBuffer++)%4 != 0)) {}
                indexBuffer--;


                //Parse content
                QString commandDestination = QString(parsingAddressBuffer);
                QString command = commandDestination + " ";
                QList<QVariant> commandArguments;
                quint16 indexDataBuffer = 0;
                while((indexBuffer < parsingBufferISize) && (indexDataBuffer < parsingIndexArgumentsBuffer)) {
                    //Integer argument
                    if(parsingArgumentsBuffer[indexDataBuffer] == 'i') {
                        union { int i; char ch[4]; } u;
                        u.ch[3] = parsingBufferI[indexBuffer + 0];
                        u.ch[2] = parsingBufferI[indexBuffer + 1];
                        u.ch[1] = parsingBufferI[indexBuffer + 2];
                        u.ch[0] = parsingBufferI[indexBuffer + 3];
                        indexBuffer += 4;
                        QString commandValue = QString::number(u.i);
                        command += commandValue + " ";
                        commandArguments << u.i;
                    }
                    //Float argument
                    else if(parsingArgumentsBuffer[indexDataBuffer] == 'f') {
                        union { float f; char ch[4]; } u;
                        u.ch[3] = parsingBufferI[indexBuffer + 0];
                        u.ch[2] = parsingBufferI[indexBuffer + 1];
                        u.ch[1] = parsingBufferI[indexBuffer + 2];
                        u.ch[0] = parsingBufferI[indexBuffer + 3];
                        indexBuffer += 4;
                        QString commandValue = QString::number(u.f);
                        command += commandValue + " ";
                        commandArguments << u.f;
                    }
                    //String argument
                    else if(parsingArgumentsBuffer[indexDataBuffer] == 's') {
                        QString commandValue = "";
                        while((indexBuffer < parsingBufferISize) && (parsingBufferI[indexBuffer]) != 0)
                            commandValue += parsingBufferI[indexBuffer++];
                        indexBuffer++;
                        while(indexBuffer % 4 != 0)
                            indexBuffer++;
                        command += commandValue + " ";
                        commandArguments << commandValue;
                    }
                    else
                        indexBuffer += 4;
                    indexDataBuffer++;
                }

                //Fire events (log, message and script mapping)
                emit(outgoingMessage(receivedHost.toString(), receivedPort, commandDestination.toLower(), commandArguments));
            }
        }
    }
}

void Udp::send(const QString &ip, quint16 port, const QString &destination, const QList<QVariant> &valeurs) {
    QHostAddress host = QHostAddress(ip);
    if(!host.isNull()) {
        QByteArray arguments, typetag, address, buffer;
        QString verboseMessage = "";
        address += destination;
        address += (char)0;
        pad(address);
        typetag += ',';

        foreach(const QVariant &valeur, valeurs) {
            if(valeur.type() == QVariant::String) {
                verboseMessage = verboseMessage + " " + qPrintable(valeur.toString());
                arguments += valeur.toString();
                arguments += (char)0;
                pad(arguments);
                typetag += 's';
            }
            else {
                verboseMessage = verboseMessage + " " + QByteArray::number(valeur.toDouble());
                union { float f; char ch[4]; } u;
                u.f = valeur.toDouble();
                arguments += u.ch[3];
                arguments += u.ch[2];
                arguments += u.ch[1];
                arguments += u.ch[0];
                typetag += 'f';
            }
        }

        buffer += address;
        buffer += typetag;
        buffer += (char)0;
        pad(buffer);
        buffer += arguments;

        socket->writeDatagram(buffer, host, port);
    }
}
