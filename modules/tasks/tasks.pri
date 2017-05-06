SOURCES += \
    $$PWD/src/task.cpp \
    $$PWD/src/schedule.cpp \
    $$PWD/src/taskpool.cpp

HEADERS += \
	$$PWD/include/Task \
    $$PWD/include/task.h \
    $$PWD/include/taskpool.h \
    $$PWD/include/schedule.h

DEFINES += APPLABS_TASKS

INCLUDEPATH += $$PWD/include
