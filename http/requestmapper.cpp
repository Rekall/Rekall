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

#include "requestmapper.h"

RequestMapper::RequestMapper(QObject *parent)
    : HttpRequestHandler(parent) {
    clockReset = QDateTime::currentMSecsSinceEpoch();
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QString path = QString(request.getPath()).normalized(QString::NormalizationForm_D);
    request.path = qPrintable(path);
    //qDebug("RequestMapper: path=%s", qPrintable(path));
    foreach(ProjectInterface *project, Global::projects) {
        if(path.startsWith("/" + project->name)) {
            if(path.startsWith("/" + project->name + "/file")) {
                request.path = request.path.replace("/" + project->name + "/file/", "/");
                project->fileController->service(request, response);
                return;
            }
            else if(path.startsWith("/" + project->name + "/download")) {
                request.path = request.path.replace("/" + project->name + "/download/", "/");
                project->fileController->service(request, response, "application/force-download");
                return;
            }
            else if(path.startsWith("/" + project->name + "/finder")) {
                QFileInfo filename(project->path.absoluteFilePath() + path.remove("/" + project->name + "/finder"));
                qDebug("RequestMapper: reveal in finder %s", qPrintable(filename.absoluteFilePath()));
                Global::revealInFinder(filename);
                if(filename.exists())   response.setStatus(200, "OK");
                else                    response.setStatus(404, "not found");
                return;
            }
            else if(path.startsWith("/" + project->name + "/open")) {
                QFileInfo filename(project->path.absoluteFilePath() + path.remove("/" + project->name + "/open"));
                Global::openFile(filename);
                qDebug("RequestMapper: open %s", qPrintable(filename.absoluteFilePath()));
                if(filename.exists())   response.setStatus(200, "OK");
                else                    response.setStatus(404, "not found");
                return;
            }
            else if (path.startsWith("/" + project->name + "/project")) {
                QString json;
                json += "{";
                json += "\"name\": \"" + project->name + "\",";
                json += "\"friendlyName\": \"" + project->friendlyName + "\",";
                json += "\"path\": \"" + project->path.absoluteFilePath() + "\",";
                json += "\"events\": [ ";
                for(quint16 eventIndex = 0 ; eventIndex < qMin(project->events.count(), 5) ; eventIndex++) {
                    SyncEntryEvent *event = project->events.at(eventIndex);
                    json += "{\"author\": \"" + event->author + "\",";
                    if(event->file.exists())
                        json += "\"file\": \"" + event->file.absoluteFilePath() + "\",";
                    json += "\"name\": \"" + event->file.baseName() + "\",";
                    json += "\"action\": \"" + QString::number(event->action) + "\",";
                    json += "\"date\": \""   + event->dateTime.toString("yyyy:MM:dd hh:mm:ss") + "\",";
                    json += "\"locationName\": \"" + event->locationName + "\",";
                    json += "\"locationGPS\": \""  + event->locationGPS + "\"},";
                }
                json.chop(1);
                json += "]";
                json += "}";
                response.setHeader("Content-Type", "application/json");
                response.write(json.toUtf8(), true);
                return;
            }
            else if(path.startsWith("/" + project->name + "/xml")) {
                if(request.getParameterMap().contains("change")) {
                    project->projectChanged(request.getParameter("change"));
                }
                else {
                    //request.path = "/rekall_cache/project.xml";
                    //project->fileController->service(request, response);
                    response.setHeader("Content-Type", "application/xml");
                    response.write(project->xmlDoc.toString().toUtf8(), true);
                }
                return;
            }
            else if(path.startsWith("/" + project->name + "/remove")) {
                Global::rekall->removeProject(project);
                return;
            }
            else if(path.startsWith("/" + project->name + "/rename")) {
                if(!request.getParameter("friendlyName").isEmpty())
                    project->setFriendlyName(request.getParameter("friendlyName"));
                return;
            }
            else {
                if(request.path == ("/" + project->name))
                    request.path = request.path + "/";
                request.path = request.path.replace("/" + project->name, "/www");
                qDebug("RequestMapper: routing to %s", request.path.data());
            }
        }
    }

    //Refresh path
    path = request.getPath();

    //Go
    /*
    if (path.startsWith("/dump"))
        DumpController().service(request, response);
    else if (path.startsWith("/template"))
        TemplateController().service(request, response);
    else if (path.startsWith("/form"))
        FormController().service(request, response);
    else if (path.startsWith("/file"))
        FileUploadController().service(request, response);
    else if (path.startsWith("/session"))
        SessionController().service(request, response);
    else*/
    if (path.startsWith("/projects")) {
        QString json;
        bool chop = false;
        json += "{";
        json += "  \"owner\": \"" + Global::userInfos->getAuthor() + "\",";

        json += "  \"ip\": [";    chop = false;
        foreach(const HttpHost &host, Global::http->getRemoteHosts()) {
            if(host.isOk) {
                json += "{";
                json += "\"type\": \"" + host.type + "\",";
                json += "\"host\": \"" + host.ip   + "\",";
                json += "\"name\": \"" + host.name + "\",";
                json += "\"reachable\": \"" + QString((host.isReachable)?("true"):("false")) + "\",";
                json += "\"port\": \"" +      QString::number(Global::http->getPort()) + "\",";
                json += "\"url\": \""  +      QString("http://%1:%2").arg(host.ip).arg(Global::http->getPort()) + "\"";
                json += "},";
                chop = true;
            }
        }
        if(chop)
            json.chop(1);
        json += "],";

        json += "  \"projects\": [";    chop = false;
        foreach(ProjectInterface *project, Global::projects) {
            json += "{";
            json += "\"name\": \"" + project->name + "\",";
            json += "\"friendlyName\": \"" + project->friendlyName + "\",";
            json += "\"path\": \"" + project->path.absoluteFilePath() + "\",";
            json += "\"events\": [ ";
            for(quint16 eventIndex = 0 ; eventIndex < qMin(project->events.count(), 5) ; eventIndex++) {
                SyncEntryEvent *event = project->events.at(eventIndex);
                json += "{\"author\": \"" + event->author + "\",";
                if(event->file.exists())
                    json += "\"file\": \"" + event->file.absoluteFilePath() + "\",";
                json += "\"name\": \"" + event->file.baseName() + "\",";
                json += "\"action\": \"" + QString::number(event->action) + "\",";
                json += "\"date\": \""   + event->dateTime.toString("yyyy:MM:dd hh:mm:ss") + "\",";
                json += "\"locationName\": \"" + event->locationName + "\",";
                json += "\"locationGPS\": \""  + event->locationGPS + "\"},";
            }
            json.chop(1);
            json += "]";
            json += "},";
            chop = true;
        }
        if(chop)
            json.chop(1);
        json += "]";

        json += "}";
        response.setHeader("Content-Type", "application/json");
        response.write(json.toUtf8(), true);
        return;
    }
    else if (path.startsWith("/async")) {
        bool chop = false;
        QString json;
        json += "{";
        json += "  \"firstTimeOpened\": \""    + QString("%1").arg(Global::rekall->firstTimeOpened)    + "\",";
        json += "  \"newVersionOfRekall\": \"" + QString("%1").arg(Global::rekall->newVersionOfRekall) + "\",";
        json += "  \"projects\": [";    chop = false;
        foreach(ProjectInterface *project, Global::projects) {
            json += "{";
            json += "\"name\": \""  + project->name                   + "\",";
            json += "\"state\": \"" + QString::number(project->state) + "\"";
            json += "},";
            chop = true;
        }
        if(chop)
            json.chop(1);
        json += "]";
        json += "}";
        response.setHeader("Content-Type", "application/json");
        response.write(json.toUtf8(), true);
        return;
    }
    else if (path.startsWith("/osc")) {
        response.setHeader("Content-Type", "text/plain");
        request.path = request.path.replace("/osc", "");
        QString ip = "127.0.0.1";
        quint16 port = 57120;
        QString destination = request.path;
        if(destination.isEmpty())
            destination = "/rekall";
        QList<QVariant> valeurs;
        QMapIterator<QByteArray,QByteArray> parametersIterator(request.getParameterMap());
        while (parametersIterator.hasNext()) {
            parametersIterator.next();
            if(parametersIterator.key().toLower() == "ip")
                ip = QHostAddress(QString(parametersIterator.value())).toString();
            else if(parametersIterator.key().toLower() == "port")
                port = parametersIterator.value().toInt();
            else {
                bool ok = false;
                qreal valeur = parametersIterator.value().toDouble(&ok);
                if(ok)  valeurs << valeur;
                else    valeurs << QString(parametersIterator.value());
            }
        }
        QString verbosePath = QString("%1:%2%3 with %4 arguments: ").arg(ip).arg(port).arg(destination).arg(valeurs.count());
        foreach(const QVariant &valeur, valeurs)
            verbosePath += " " + valeur.toString();
        if((!ip.isEmpty()) && (port > 0) && (!destination.isEmpty())) {
            Global::udp->send(ip, port, destination, valeurs);
            response.write("Sent\n" + verbosePath.toUtf8(), true);
            response.setStatus(200, "OK");
        }
        else {
            response.setStatus(403,"forbidden");
            response.write("Not Sent\n" + verbosePath.toUtf8(), true);
        }
        return;
    }


    else if (path.startsWith("/clock_data")) {
        QDateTime value = QDateTime::currentDateTime();
        if(!request.getParameter("reset").isEmpty())
            clockReset = value.toMSecsSinceEpoch() - request.getParameter("reset").toLongLong();
        if(!request.getParameter("text").isEmpty())
            clockText = request.getParameter("text");
        QString json;
        json += "{";
        json += "\"clock\": "       + QString::number(value.toMSecsSinceEpoch()) + ",";
        json += "\"clockReset\": "  + QString::number(clockReset) + ",";
        json += "\"text\": \""  + clockText + "\"";
        json += "}";
        response.setHeader("Content-Type", "application/json");
        response.write(json.toUtf8(), true);
        return;
    }
    else if (path.startsWith("/clock")) {
        request.path = "/apps/clock/index.html";
        Static::fileController->service(request, response);
        return;
    }



    else if (path.startsWith("/add")) {
        Global::rekall->askAddProject = 1;
        while(Global::rekall->askAddProject != 0) {
            QCoreApplication::processEvents();
            Global::qSleep(40);
        }
        return;
    }
    else if (path.startsWith("/capture")) {
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        Global::rekall->askScreenshot = 1;
        while(Global::rekall->askScreenshot != 0) {
            QCoreApplication::processEvents();
            Global::qSleep(40);
        }
        Global::rekall->screenshot.save(&buffer, "PNG");
        buffer.close();
        buffer.open(QIODevice::ReadOnly);
        response.setHeader("Content-Type", "image/png");
        while (!buffer.atEnd())
            response.write(buffer.read(65536));

        response.write(bytes, true);
        return;
    }


    else if (path.startsWith("/returns_ip")) {
        response.setHeader("Content-Type", "text/plain");
        response.write("OK", true);
        return;
    }


    else if (path == "/") {
        request.path = "/menu.html";
        Static::fileController->service(request, response);
        return;
    }
    else {
        request.path = request.path.replace("/www", "");
        Static::fileController->service(request, response);
        return;
    }
    qDebug("RequestMapper: finished request");
}

void RequestMapper::incomingMessage(const QString &, quint16, const QString &destination, const QList<QVariant> &valeurs) {
    if(destination.startsWith("/rekall/clock")) {
        if(destination.startsWith("/rekall/clock/reset")) {
            clockReset = QDateTime::currentMSecsSinceEpoch();
            if(valeurs.count() > 0)
                clockReset -= valeurs.at(0).toLongLong();
        }
        else if(destination.startsWith("/rekall/clock/text"))
            clockText = valeurs.at(0).toString();
    }
}

