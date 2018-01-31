#-------------------------------------------------
#
# Project created by QtCreator 2018-01-29T13:31:32
#
#-------------------------------------------------
QT += core widgets

TARGET = dpa-ext-gnomekeyring
TEMPLATE = lib

DEFINES += DPAEXTGNOMEKEYRING_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += plugin c++11 link_pkgconfig
PKGCONFIG += gnome-keyring-1

SOURCES += \
        gnomekeyringextention.cpp

HEADERS += \
        gnomekeyringextention.h \
    ../dde-polkit-agent/agent-extension-proxy.h \
    ../dde-polkit-agent/agent-extension.h

unix {
    target.path = /usr/lib/polkit-1-dde/plugins/
    INSTALLS += target
}

DISTFILES += \
    dpa-ext-gnomekeyring.json
