TEMPLATE = app
CONFIG += qt warn_on
QT += webkit network

mac {
    TARGET = ZenHtmlEdit
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
} else {
    TARGET = zenhtmledit
}

DESTDIR = ../

include(../pri/git.pri)
include(../pri/webkittrunk.pri)
include(../zenhtmledit.pri)
include(src.pri)

