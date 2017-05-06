#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

class System : public QObject
{
    Q_OBJECT
	Q_PROPERTY (QString platform READ getPlatform CONSTANT)

public:
    System(QObject * parent = nullptr);

	static QObject * instance(QQmlEngine * engine, QJSEngine * scriptEngine);

    QString getPlatform() const;
};

#endif // SYSTEM_H
