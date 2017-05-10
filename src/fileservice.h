#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <QObject>
#include <QDir>
#include <QFile>

class FileService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
	Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
	Q_PROPERTY(QString filePath READ filePath NOTIFY currentFileChanged)
	Q_PROPERTY(bool fileExists READ fileExists NOTIFY currentFileChanged)
	Q_PROPERTY(QStringList files READ files NOTIFY currentPathChanged)

public:
	explicit FileService(QObject *parent = 0);

	const QString & currentPath() const;
	const QString & currentFile() const;
	QString filePath() const;
	bool fileExists() const;
	const QStringList & files() const;

	void setCurrentPath(const QString & path);
	void setCurrentFile(const QString & filename);

signals:
	void currentPathChanged();
	void currentFileChanged();

public slots:
	void update();

private:
	QString _currentFile;
	QString _currentPath;
	QDir _currentDir;
	QStringList _files;
};

#endif // FILESERVICE_H
