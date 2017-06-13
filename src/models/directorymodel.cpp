#include "directorymodel.h"

#include <QStandardPaths>

DirectoryModel::DirectoryModel(QObject *parent) : QObject(parent) {
	_currentDir.setFilter(QDir::Files);
	_currentDir.setSorting(QDir::Name);
}

const QString & DirectoryModel::currentPath() const {
	return _currentPath;
}

const QString & DirectoryModel::currentFile() const {
	return _currentFile;
}

QString DirectoryModel::filePath() const {
	return _currentDir.filePath(_currentFile);
}

bool DirectoryModel::fileExists() const {
	return !_currentFile.isEmpty() && _currentDir.exists(_nameOnly ? _currentFile + "." + _extension : _currentFile);
}

const QString & DirectoryModel::extension() const {
	return _extension;
}

const QStringList & DirectoryModel::files() const {
	return _files;
}

bool DirectoryModel::nameOnly() const {
	return _nameOnly;
}

void DirectoryModel::setCurrentPath(const QString & path) {
	_currentPath = path;
	update();
}

void DirectoryModel::setCurrentFile(const QString & filename) {
	_currentFile = filename;
	emit currentFileChanged();
}

void DirectoryModel::setExtension(const QString & extension) {
	_extension = extension;
	QStringList filters;

	if(!extension.isEmpty()) {
		filters << "*." + extension;
	}

	_currentDir.setNameFilters(filters);
	emit extensionChanged();
	update();
}

void DirectoryModel::setNameOnly(bool value) {
	_nameOnly = value;
	emit nameOnlyChanged();
	update();
}

void DirectoryModel::update() {
	_currentDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

	if(!_currentDir.exists("stetoscope")) {
		_currentDir.mkdir("stetoscope");
	}

	_currentDir.cd("stetoscope");

	if(_currentPath.isEmpty()) {
		return;
	}

	if(!_currentDir.exists(_currentPath)) {
		_currentDir.mkpath(_currentPath);
	}

	_currentDir.cd(_currentPath);
	_files.clear();

	if(_nameOnly) {
		for(QString & f : _currentDir.entryList()) {
			_files.append(f.remove(QRegExp("\\." + _extension + "$")));
		}
	} else {
		_files = _currentDir.entryList();
	}

	emit currentPathChanged();
	emit currentFileChanged();
}
