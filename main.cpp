#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <DeviceService>
#include <AudioService>
#include <RecordingService>
#include <models/DirectoryModel>
#include <models/SamplesModel>

#include "tests/testsamplegenerator.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterType<Device>("steto", 1, 0, "Device");
	qmlRegisterType<DeviceService>("steto", 1, 0, "DeviceService");
	qmlRegisterType<DirectoryModel>("steto", 1, 0, "DirectoryModel");
	qmlRegisterType<AudioService>("steto", 1, 0, "AudioService");
	qmlRegisterType<RecordingService>("steto", 1, 0, "RecordingService");
	qmlRegisterType<SampleProvider>("steto", 1, 0, "SampleProvider");
	qmlRegisterType<SamplesModel>("steto", 1, 0, "SamplesModel");
	qmlRegisterType<TestSampleGenerator>("steto.test", 1, 0, "TestSampleGenerator");

	QQmlApplicationEngine engine;

	engine.addImportPath("qrc:/");
	engine.addImportPath("qrc:/ru/applabs");
	engine.load(QUrl(QLatin1String("qrc:/steto/main.qml")));

	return app.exec();
}
