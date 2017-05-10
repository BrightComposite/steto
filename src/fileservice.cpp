#include "fileservice.h"

#include <QStandardPaths>

FileService::FileService(QObject *parent) : QObject(parent) {
	_currentDir.setFilter(QDir::Files);
	_currentDir.setSorting(QDir::Name);
}

const QString & FileService::currentPath() const {
	return _currentPath;
}

const QString & FileService::currentFile() const {
	return _currentFile;
}

QString FileService::filePath() const {
	return _currentDir.filePath(_currentFile);
}

bool FileService::fileExists() const {
	return _currentDir.exists(_currentFile);
}

const QStringList & FileService::files() const {
	return _files;
}

void FileService::setCurrentPath(const QString & path) {
	_currentPath = path;
	update();
}

void FileService::setCurrentFile(const QString & filename) {
	_currentFile = filename;
	emit currentFileChanged();
}

void FileService::update() {
	_currentDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if(!_currentDir.exists(_currentPath)) {
		_currentDir.mkpath(_currentPath);
	}

	_currentDir.cd(_currentPath);
	_files = _currentDir.entryList();

	emit currentPathChanged();
	emit currentFileChanged();
}
