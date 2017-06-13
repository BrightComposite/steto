QT += qml quick bluetooth multimedia

CONFIG += c++14

HEADERS += \
	src/btconnection.h \
	src/device.h \
	src/BtConnection \
	src/Device \
	src/DeviceService \
	src/audioservice.h \
	src/AudioService \
    src/recordingservice.h \
    src/models/signalmodel.h \
    src/models/spectremodel.h \
    src/fft.h \
    src/deviceservice.h \
	src/RecordingService \
    src/models/samplesmodel.h \
    src/models/SamplesModel \
    src/models/DirectoryModel \
    src/models/directorymodel.h \
    src/sampleprovider.h \
    src/SampleProvider \
    tests/testsamplegenerator.h

SOURCES += main.cpp \
    src/btconnection.cpp \
    src/deviceservice.cpp \
    src/device.cpp \
    src/audioservice.cpp \
    src/recordingservice.cpp \
    src/models/signalmodel.cpp \
    src/models/spectremodel.cpp \
    src/models/samplesmodel.cpp \
    src/models/directorymodel.cpp \
    src/sampleprovider.cpp \
    tests/testsamplegenerator.cpp

RESOURCES += qml/qml.qrc

INCLUDEPATH += $$PWD/src/
INCLUDEPATH += $$PWD/libs/libsndfile/src

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += \
	$$PWD/qml/ \
	$$PWD/qml/steto/ \
	$$PWD/qml/steto/view \
	$$PWD/qml/steto/model

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android: QT += androidextras

include(modules/common/common.pri)
include(modules/lodash/lodash.pri)
include(modules/storage/storage.pri)
include(modules/async/async.pri)
include(libs/fft-real/fft-real.pri)
include(vendor/vendor.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
	android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

android {
	LIBS += $$PWD/libs/libsndfile/src/.libs/libsndfile.so

	contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
		ANDROID_EXTRA_LIBS = \
			$$PWD/libs/libsndfile/src/.libs/libsndfile.so
	}
}
