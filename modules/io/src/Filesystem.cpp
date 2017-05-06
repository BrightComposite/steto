#include <Filesystem.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

Filesystem::Filesystem(QObject *parent) : QObject(parent)
{

}

QString Filesystem::path(const QString & p) const {
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + p;
}

bool Filesystem::copy(const QString & to, const QString & from) const {
	QFile src(from.contains("file://") ? from.mid(7) : from);
	QFile dst(path(to));
	QFileInfo info(dst.fileName());

	auto dir = info.dir();

	if(dir.exists()) {
		qDebug() << "Dir exists:" << dir.absolutePath();

		if(dst.exists()) {
			dst.remove();
		}
	} else {
		if(!dir.mkpath(".")) {
			qWarning() << "Can't make path" << dir.absolutePath();
			return false;
		}
	}

	if(!src.copy(dst.fileName())) {
		qWarning() << "Can't copy file" << src.fileName() << "to" << dst.fileName();
		return false;
	}

	return true;
}
