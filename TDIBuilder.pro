CONFIG      += plugin debug_and_release c++14
TARGET      = $$qtLibraryTarget(tdicollectionplugin)
TEMPLATE    = lib

DESTDIR     = /opt/Qt/Tools/QtCreator/lib/Qt/plugins/designer

#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += LIB
INCLUDEPATH += LIB/DebugInfo
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtDesigner
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtDesigner/$$[QT_VERSION]/QtDesigner/private
INCLUDEPATH += Extensions

SOURCES = deviceroot/devicerootplugin.cpp \
    devicefolder/devicefolderplugin.cpp \
    devicedata/devicedataplugin.cpp \
    deviceproperty/devicepropertyplugin.cpp \
    tdicollection.cpp \
    LIB/securitygroups.cpp \
    Extensions/symboltreemap.cpp \
    Extensions/tdiextensionfactory.cpp \
    Extensions/tdimap.cpp \
    LIB/smodatatype_calc.c \
    LIB/tdiclasses.cpp

HEADERS = app_config.h \
    deviceroot/devicerootplugin.h \
    devicefolder/devicefolderplugin.h \
    devicedata/devicedataplugin.h \
    deviceproperty/devicepropertyplugin.h \
    tdicollection.h \
    LIB/securitygroups.h \
    LIB/tdiclasses.h \
    Extensions/symboltreemap.h \
    Extensions/tdiextensionfactory.h \
    Extensions/tdimap.h

#    LIB/tdi.h

RESOURCES   = icons.qrc
LIBS        += -L. 

DEFINES += QT_DEBUG_PLUGINS=1
DEFINES += QT_USE_QSTRINGBUILDER

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

#message(QT_INSTALL_HEADERS: $$[QT_INSTALL_HEADERS])
#message(QT_ver: $$[QT_VERSION])

include(deviceitem/deviceitem.pri)
include(devicefolder/devicefolder.pri)
include(deviceroot/deviceroot.pri)
include(devicedata/devicedata.pri)
include(deviceproperty/deviceproperty.pri)
include(LIB/elf_parser/elf_parser.pri)

FORMS += \
    form.ui
