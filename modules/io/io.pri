QT += websockets

CONFIG += c++11 no_keywords

SOURCES += \
    $$PWD/src/Filesystem.cpp \
    $$PWD/src/SocketIO.cpp \
    $$PWD/src/Http.cpp \
    $$PWD/io_plugin.cpp

HEADERS += \
    $$PWD/include/Filesystem.h \
    $$PWD/include/SocketIO.h \
    $$PWD/include/Http.h

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include

QML_IMPORT_PATH += $$PWD

RESOURCES += \
    $$PWD/io.qrc
