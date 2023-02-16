CONFIG      += plugin debug_and_release c++14
TARGET      = $$qtLibraryTarget(debuginfoplugin)
TEMPLATE    = lib

DESTDIR     = /opt/Qt/Tools/QtCreator/lib/Qt/plugins/designer

INCLUDEPATH += ..
HEADERS     += \
    debuginfoplugin.h \
    app_config.h
SOURCES     = debuginfoplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L. 
DEFINES     += QT_USE_QSTRINGBUILDER

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

include(debuginfo.pri)
