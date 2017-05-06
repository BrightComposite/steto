#include <System.h>
#include <QQuickItem>

static void registerTypes() {
	qmlRegisterSingletonType<System>("ru.applabs", 1, 0, "System", System::instance);
}

Q_COREAPP_STARTUP_FUNCTION(registerTypes)
