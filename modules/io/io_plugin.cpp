#include <Filesystem.h>
#include <Http.h>
#include <SocketIO.h>
#include <QQuickItem>

static void registerTypes() {
	qmlRegisterType<Filesystem>("ru.applabs.io", 1, 0, "Filesystem");
	qmlRegisterType<Http>("ru.applabs.io.private", 1, 0, "Http");
	qmlRegisterType<SocketIO>("ru.applabs.io", 1, 0, "SocketIO");
}

Q_COREAPP_STARTUP_FUNCTION(registerTypes)
