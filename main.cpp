#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <DeviceService>

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterType<Device>("steto", 1, 0, "Device");
	qmlRegisterType<DeviceService>("steto", 1, 0, "DeviceService");
	qmlRegisterType<DataService>("steto", 1, 0, "DataService");

	QQmlApplicationEngine engine;

	engine.addImportPath("qrc:/");
	engine.addImportPath("qrc:/ru/applabs");
	engine.load(QUrl(QLatin1String("qrc:/steto/main.qml")));

	return app.exec();
}
