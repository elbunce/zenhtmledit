TEMPLATE = app
CONFIG += qt warn_on
QT += webkit network

mac {
    TARGET = ZenHtmlEdit
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
} else {
    TARGET = zenhtmledit
}

include(../zenhtmledit.pri)
include(../distfiles.pri)
include(src.pri)

