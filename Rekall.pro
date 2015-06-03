#    This file is part of Rekall.
#    Copyright (C) 2013-2015
#
#    Project Manager: Clarisse Bardiot
#    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)
#
#    This file was written by Guillaume Jacquemin.
#
#    Rekall is a free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

QT_VERSION = $$[QT_VERSION]
message($$[QT_VERSION])
contains(QT_VERSION, "^5.*") {
    message("Rekall For QT5")
    DEFINES += QT5
    QT      += core gui network xml widgets webkitwidgets multimedia multimediawidgets
} else {
    message("Rekall For QT4 — not compatible, please upgrade")
}

TARGET   = Rekall
TEMPLATE = app

SOURCES  += main.cpp

SOURCES  += rekall.cpp global.cpp core/project.cpp  core/analyse.cpp
HEADERS  += rekall.h   global.h   core/project.h    core/analyse.h

SOURCES  += video/videoplayer.cpp webwrapper.cpp
HEADERS  += video/videoplayer.h   webwrapper.h
FORMS    += video/videoplayer.ui  webwrapper.ui


#Location
HEADERS  += core/userinfos.h
SOURCES  += core/userinfos.cpp
macx {
    DEFINES           += LOCATION_INSTALLED
    OBJECTIVE_SOURCES += core/userinfos_mac.mm
    LIBS              += -framework Cocoa -framework CoreLocation
}

#VLC
#false {
#    DEFINES  += VLC_INSTALLED
#    HEADERS  += video/vlc.h
#    SOURCES  += video/vlc.cpp
#    macx {
#        INCLUDEPATH       +=   /Applications/VLC.app/Contents/MacOS/include
#        LIBS              += -L/Applications/VLC.app/Contents/MacOS/lib -lvlc
#        BUNDLE_RES.files   =   /Applications/VLC.app/Contents/MacOS/plugins
#        BUNDLE_RES.path    = Contents/MacOS
#        QMAKE_BUNDLE_DATA += BUNDLE_RES
#        BUNDLE_RES2.files  =   /Applications/VLC.app/Contents/MacOS/lib
#        BUNDLE_RES2.path   = Contents/MacOS
#        QMAKE_BUNDLE_DATA += BUNDLE_RES2
#    }
#    win32 {
#        INCLUDEPATH += "C:/Program Files (x86)/VideoLAN/VLC/sdk/include"
#        LIBS        += -L"C:/Program Files (x86)/VideoLAN/VLC/sdk/lib" -llibvlc
#    }
#}

#Bundle Mac OS X
RESOURCES             += icons/Rekall.qrc
macx {
    LIBS              += -framework Carbon
    ICON               = icon.icns
    QMAKE_INFO_PLIST   = Info.plist

    BUNDLE_RES.files   = icon_file.icns
    BUNDLE_RES.path    = Contents/Resources
    QMAKE_BUNDLE_DATA += BUNDLE_RES
}
win32 {
    DEFINES           += __WINDOWS_MM__
    #LIBS             += -lwinmm -lsetupapi
    RC_FILE            = icon.rc
    DEFINES           += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
}


linux {
    HEADERS += watcher/folderwatcher_linux.h
    SOURCES += watcher/folderwatcher_linux.cpp
}
win32 {
    HEADERS += watcher/folderwatcher_win.h
    SOURCES += watcher/folderwatcher_win.cpp
}
macx {
    HEADERS += watcher/folderwatcher_mac.h
    SOURCES += watcher/folderwatcher_mac.cpp
}
HEADERS += core/watcherlocal.h   watcher/folderwatcher.h
SOURCES += core/watcherlocal.cpp watcher/folderwatcher.cpp


#WebApp
HEADERS += http/http.h   http/requestmapper.h   http/static.h   http/filecontroller.h   http/udp.h
SOURCES += http/http.cpp http/requestmapper.cpp http/static.cpp http/filecontroller.cpp http/udp.cpp
true {
    HEADERS += http/lib/dumpcontroller.h    http/lib/templatecontroller.h    http/lib/formcontroller.h    http/lib/fileuploadcontroller.h    http/lib/sessioncontroller.h
    SOURCES += http/lib/dumpcontroller.cpp  http/lib/templatecontroller.cpp  http/lib/formcontroller.cpp  http/lib/fileuploadcontroller.cpp  http/lib/sessioncontroller.cpp
    include(http/lib/qtservice/src/qtservice.pri)
    include(http/lib/bfLogging/src/bfLogging.pri)
    include(http/lib/bfHttpServer/src/bfHttpServer.pri)
    include(http/lib/bfTemplateEngine/src/bfTemplateEngine.pri)
}
