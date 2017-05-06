#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

class Filesystem : public QObject
{
	Q_OBJECT
public:
	explicit Filesystem(QObject *parent = 0);

public Q_SLOTS:
	QString path(const QString & p) const;
	bool copy(const QString & to, const QString & from) const;
};

#endif // FILESYSTEM_H
