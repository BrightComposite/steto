CONFIG += c++11 no_keywords

SOURCES += \
    $$PWD/src/System.cpp \
    $$PWD/common_plugin.cpp

HEADERS += \
    $$PWD/include/System.h

INCLUDEPATH += $$PWD/include

QML_IMPORT_PATH += $$PWD

RESOURCES += \
    $$PWD/common.qrc
