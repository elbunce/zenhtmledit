TEMPLATE = subdirs
SUBDIRS = src

unix {
    # this is an ugly work around to do .PHONY: doc
    doxygen.target = doc dox
    doxygen.commands = doxygen Doxyfile
    doxygen.depends = Doxyfile
    QMAKE_EXTRA_UNIX_TARGETS += doxygen
}

