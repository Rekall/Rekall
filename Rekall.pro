#    This file is part of Rekall.
#    Copyright (C) 2013-2014
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
contains(QT_VERSION, "^4.*") {
    message("Rekall For QT4")
    DEFINES += QT4
    QT      += core gui opengl network script xml phonon webkit
} else {
    message("Rekall For QT5 (very experimental)")
    DEFINES += QT5
    QT      += widgets core gui opengl network script xml phonon webkit
}

TARGET    = Rekall
TEMPLATE  = app

SOURCES  += main.cpp
HEADERS  += tasks/taskslist.h   tasks/feedlist.h   tasks/taskprocess.h
SOURCES  += tasks/taskslist.cpp tasks/feedlist.cpp tasks/taskprocess.cpp
FORMS    += tasks/taskslist.ui  tasks/feedlist.ui

HEADERS  += core/watcherfeeling.h   core/watcher.h
SOURCES  += core/watcherfeeling.cpp core/watcher.cpp
FORMS    += core/watcherfeeling.ui

HEADERS  += rekall.h   gui/splash.h   misc/global.h   misc/options.h
SOURCES  += rekall.cpp gui/splash.cpp misc/global.cpp misc/options.cpp
FORMS    += rekall.ui  gui/splash.ui

HEADERS  += core/sorting.h   core/phases.h   core/metadata.h   core/project.h   core/document.h   core/tag.h   core/cluster.h
SOURCES  += core/sorting.cpp core/phases.cpp core/metadata.cpp core/project.cpp core/document.cpp core/tag.cpp core/cluster.cpp
FORMS    += core/sorting.ui  core/phases.ui

HEADERS  += gui/timeline.h   gui/previewer.h   gui/playervideo.h   gui/timelinecontrol.h   gui/timelinegl.h   gui/previewerlabel.h
SOURCES  += gui/timeline.cpp gui/previewer.cpp gui/playervideo.cpp gui/timelinecontrol.cpp gui/timelinegl.cpp gui/previewerlabel.cpp
FORMS    += gui/timeline.ui  gui/previewer.ui  gui/playervideo.ui  gui/timelinecontrol.ui

HEADERS  += gui/player.h   gui/viewer.h   gui/viewergl.h
SOURCES  += gui/player.cpp gui/viewer.cpp gui/viewergl.cpp
FORMS    += gui/player.ui  gui/viewer.ui

HEADERS  += items/uitreeview.h   items/uitreeviewwidget.h   items/uitreedelegate.h   items/uifileitem.h
SOURCES  += items/uitreeview.cpp items/uitreeviewwidget.cpp items/uitreedelegate.cpp items/uifileitem.cpp
FORMS    += items/uitreeview.ui

HEADERS  += interfaces/udp.h   interfaces/fileuploadcontroller.h
SOURCES  += interfaces/udp.cpp interfaces/fileuploadcontroller.cpp
FORMS    += interfaces/udp.ui
HEADERS  += interfaces/http/httplistener.h interfaces/http/httpconnectionhandler.h interfaces/http/httpconnectionhandlerpool.h interfaces/http/httprequest.h interfaces/http/httpresponse.h interfaces/http/httpcookie.h interfaces/http/httprequesthandler.h
HEADERS  += interfaces/http/httpsession.h interfaces/http/httpsessionstore.h
HEADERS  += interfaces/http/staticfilecontroller.h
SOURCES  += interfaces/http/httplistener.cpp interfaces/http/httpconnectionhandler.cpp interfaces/http/httpconnectionhandlerpool.cpp interfaces/http/httprequest.cpp interfaces/http/httpresponse.cpp interfaces/http/httpcookie.cpp interfaces/http/httprequesthandler.cpp
SOURCES  += interfaces/http/httpsession.cpp interfaces/http/httpsessionstore.cpp
SOURCES  += interfaces/http/staticfilecontroller.cpp

HEADERS  += core/person.h
SOURCES  += core/person.cpp

#Location
HEADERS += interfaces/userinfos.h
SOURCES += interfaces/userinfos.cpp
macx {
    DEFINES           += LOCATION_INSTALLED
    OBJECTIVE_SOURCES += interfaces/userinfos_mac.mm
    LIBS              += -framework Cocoa -framework CoreLocation
}

#TRANSLATIONS          = Tools/Translation_fr_FR.ts
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
