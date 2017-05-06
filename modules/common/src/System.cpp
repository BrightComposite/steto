#include <System.h>
#include <QDebug>

System::System(QObject * parent) : QObject(parent)
{
}

QString System::getPlatform() const
{
    #if defined(Q_OS_WIN)
        return "windows";
    #elif defined(Q_OS_ANDROID)
        return "android";
    #elif defined(Q_OS_LINUX)
		return "linux";
    #elif defined(Q_OS_IOS)
        return "ios";
    #elif defined(Q_OS_MAC)
        return "mac";
	#elif defined(Q_OS_UNIX)
		return "unix";
    #else
        return "unknown";
    #endif
}

QObject * System::instance(QQmlEngine * engine, QJSEngine * scriptEngine)
{
	Q_UNUSED(scriptEngine)
	return new System(engine);
}
