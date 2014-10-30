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

#include "filecontroller.h"

FileController::FileController(QSettings* settings, const QByteArray &_docroot, ProjectInterface *_project, QObject* parent)
    :HttpRequestHandler(parent) {
    maxAge   = settings->value("maxAge",   "60000").toInt();
    encoding = settings->value("encoding", "UTF-8").toString();
    project  = _project;
    docroot  = _docroot;//settings->value("path",     ".").toString();
    // Convert relative path to absolute, based on the directory of the config file.
#ifdef Q_OS_WIN32
    if (QDir::isRelativePath(docroot) && settings->format() != QSettings::NativeFormat)
#else
    if (QDir::isRelativePath(docroot))
#endif
    {
        QFileInfo configFile(settings->fileName());
        docroot = QFileInfo(configFile.absolutePath(), docroot).absoluteFilePath().toUtf8();
    }
    qDebug("FileController: docroot=%s, encoding=%s, maxAge=%i", qPrintable(docroot), qPrintable(encoding),maxAge);
    maxCachedFileSize=settings->value("maxCachedFileSize","65536").toInt();
    cache    .setMaxCost(settings->value("cacheSize","1000000").toInt());
    cacheTimeout=settings->value("cacheTime","60000").toInt();
    qDebug("FileController: cache timeout=%i, size=%i",cacheTimeout,cache.maxCost());
}


void FileController::service(HttpRequest& request, HttpResponse& response, const QByteArray &contentType) {
    QByteArray path = request.getPath();

    /*
    qDebug("\n\n\nHEADERS");
    QMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
    while (i.hasNext()) {
        i.next();
        qDebug("%s = %s", i.key().data(), i.value().data());
    }

    qDebug("\nPARAM");
    i=QMapIterator<QByteArray,QByteArray>(request.getParameterMap());
    while (i.hasNext()) {
        i.next();
        qDebug("%s = %s", i.key().data(), i.value().data());
    }

    qDebug("\nCOOKIES");
    i=QMapIterator<QByteArray,QByteArray>(request.getCookieMap());
    while (i.hasNext()) {
        i.next();
        qDebug("%s = %s", i.key().data(), i.value().data());
    }
    qDebug("\n\n");
    */

    //Header
    response.setHeader("Accept-Ranges", "bytes");

    // Check if we have the file in cache
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    mutex.lock();
    CacheEntry* entry = cache.object(path);
    if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout)) {
        QByteArray document=entry->document; //copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
        QByteArray filename=entry->filename;
        mutex.unlock();
        qDebug("FileController: Cache hit for %s",path.data());
        setContentType(filename, response, contentType);
        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
        response.write(document);
    }
    else {
        mutex.unlock();
        // The file is not in cache.
        qDebug("FileController: Cache miss for %s",path.data());
        // Forbid access to files outside the docroot directory
        if (path.contains("/..")) {
            qWarning("FileController: detected forbidden characters in path %s",path.data());
            response.setStatus(403,"forbidden");
            response.write("403 forbidden",true);
            return;
        }
        // If the filename is a directory, append index.html.
        if (QFileInfo(docroot+path).isDir()) {
            path += "/index.html";
        }
        // Try to open the file
        path = docroot + path;
        QFile file(path);
        qDebug("FileController: Open file %s", qPrintable(file.fileName()));
        if (file.open(QIODevice::ReadOnly)) {
            setContentType(path, response, contentType);
            response.setHeader("Cache-Control", "max-age="+QByteArray::number(maxAge/1000));

            QPair<qint64, qint64> bytesRange;
            bytesRange.first = bytesRange.second = -1;
            if(request.getHeader("Range").length() > 0) {
                QStringList bytes = QString(request.getHeader("Range")).replace("bytes=", "").split("-");
                if(bytes.length() == 2)
                    bytesRange = qMakePair(bytes.at(0).toLongLong(), bytes.at(1).toLongLong());
            }
            if ((file.size() <= maxCachedFileSize) && (bytesRange.first == -1) && (bytesRange.second == -1)) {
                // Return the file content and store it also in the cache
                entry = new CacheEntry();
                while (!file.atEnd() && !file.error()) {
                    QByteArray buffer=file.read(65536);
                    response.write(buffer);
                    entry->document.append(buffer);
                }
                entry->created=now;
                entry->filename=path;
                mutex.lock();
                cache.insert(request.getPath(),entry,entry->document.size());
                mutex.unlock();
            }
            else {
                // Return the file content, do not store in cache
                if((bytesRange.first == -1) && (bytesRange.second == -1)) {
                    while (!file.atEnd() && !file.error())
                        response.write(file.read(65536));
                }
                else {
                    response.setStatus(206, "partial content");
                    if((!file.atEnd()) && (!file.error()) && (bytesRange.first < file.size()) && (bytesRange.second < file.size())) {
                        file.seek(bytesRange.first);
                        response.write(file.read(bytesRange.second - bytesRange.first));
                    }
                    else
                        qWarning("Byte range error %d %d", bytesRange.first, bytesRange.second);
                }
            }
            file.close();
            qWarning("FileController: File %s sent", qPrintable(file.fileName()));
        }
        else {
            if (file.exists()) {
                qWarning("FileController: Cannot open existing file %s for reading",qPrintable(file.fileName()));
                response.setStatus(403,"forbidden");
                response.write("403 forbidden",true);
            }
            else {
                response.setStatus(404,"not found");
                response.write("404 not found",true);
            }
        }
    }
}

void FileController::setContentType(QString filename, HttpResponse& response, const QByteArray &contentType) {
    if(!contentType.isEmpty()) {
        response.setHeader("Content-Type", contentType);
    }
    else {
        if (filename.endsWith(".png"))
            response.setHeader("Content-Type", "image/png");
        else if ((filename.endsWith(".jpg")) || (filename.endsWith(".jpeg")))
            response.setHeader("Content-Type", "image/jpeg");
        else if (filename.endsWith(".gif"))
            response.setHeader("Content-Type", "image/gif");
        else if (filename.endsWith(".pdf"))
            response.setHeader("Content-Type", "application/pdf");
        else if (filename.endsWith(".txt"))
            response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
        else if (filename.endsWith(".html") || filename.endsWith(".htm"))
            response.setHeader("Content-Type", qPrintable("text/html; charset="+encoding));
        else if (filename.endsWith(".css"))
            response.setHeader("Content-Type", "text/css");
        else if (filename.endsWith(".js"))
            response.setHeader("Content-Type", "text/javascript");

        else {
            MimeCacheEntry entry;
            if (mimeCache.contains(filename)) {
                qDebug("FileController: Metadatas from cache");
                entry = mimeCache.value(filename);
            }
            else {
                entry.mimeType = "";
                entry.duration = 0;
                Metadatas metadatas;
                QFileInfo file(filename);
                if((project) && (project->sync->folders.contains(file.absolutePath())) && (project->sync->folders[file.absolutePath()].contains(file.absoluteFilePath())))
                    metadatas = project->sync->folders[file.absolutePath()][file.absoluteFilePath()]->metadatas;
                if(metadatas.contains("File->MIME Type")) {
                    entry.mimeType = metadatas["File->MIME Type"];
                    entry.mimeType = entry.mimeType.replace("video/quicktime", "video/mp4");
                }
                if(metadatas.contains("Rekall->Media Duration (s.)")) {
                    entry.duration = metadatas.value("Rekall->Media Duration (s.)").toDouble();
                }
                mimeCache.insert(filename, entry);
            }
            if(!entry.mimeType.isEmpty()) {
                response.setHeader("Content-Type", entry.mimeType.toLatin1());
                qDebug("FileController: Content-Type = %s for %s", qPrintable(entry.mimeType), qPrintable(filename));
            }
            if(entry.duration > 0) {
                response.setHeader("X-Content-Duration", entry.duration);
                qDebug("FileController: X-Content-Duration = %d for %s", entry.duration, qPrintable(filename));
            }
        }
    }
}
