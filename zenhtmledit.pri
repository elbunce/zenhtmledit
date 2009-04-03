include($$PWD/pri/webkittrunk.pri)
include($$PWD/pri/git.pri)

isEmpty(OUTPUT_DIR) {
    CONFIG(release):OUTPUT_DIR=$$PWD/Release
    CONFIG(debug):OUTPUT_DIR=$$PWD/Debug
    webkit_trunk { OUTPUT_DIR=$${OUTPUT_DIR}_trunk }
}

OBJECTS_DIR = $$OUTPUT_DIR/obj
MOC_DIR = $$OUTPUT_DIR/moc
RCC_DIR = $$OUTPUT_DIR/rcc
DESTDIR = $$OUTPUT_DIR

mac { 
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
}

