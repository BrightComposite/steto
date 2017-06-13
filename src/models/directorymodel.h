#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <QObject>
#include <QDir>
#include <QFile>

class DirectoryModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
	Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
	Q_PROPERTY(QString filePath READ filePath NOTIFY currentFileChanged)
	Q_PROPERTY(QString extension READ extension WRITE setExtension NOTIFY extensionChanged)
	Q_PROPERTY(bool nameOnly READ nameOnly WRITE setNameOnly NOTIFY nameOnlyChanged)
	Q_PROPERTY(bool fileExists READ fileExists NOTIFY currentFileChanged)
	Q_PROPERTY(QStringList files READ files NOTIFY currentPathChanged)

public:
	explicit DirectoryModel(QObject *parent = 0);

	const QString & currentPath() const;
	const QString & currentFile() const;
	QString filePath() const;
	bool fileExists() const;
	const QString & extension() const;
	bool nameOnly() const;
	const QStringList & files() const;

	void setCurrentPath(const QString & path);
	void setCurrentFile(const QString & filename);
	void setExtension(const QString & extension);
	void setNameOnly(bool value);

signals:
	void currentPathChanged();
	void currentFileChanged();
	void extensionChanged();
	void nameOnlyChanged();

public slots:
	void update();

private:
	QString _currentFile;
	QString _currentPath;
	QString _extension = "";
	bool _nameOnly = false;
	QDir _currentDir;
	QStringList _files;
};

#endif // FILESERVICE_H
