#include "sampleprovider.h"
#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QtEndian>

#include <models/signalmodel.h>
#include <models/spectremodel.h>

#ifdef Q_OS_ANDROID
	#include <sndfile.hh>
#endif

SampleProvider::SampleProvider(QObject * parent) : QObject(parent) {
	_signal = new SignalModel(this);
	_spectre = new SpectreModel(this);
}

SamplesModel * SampleProvider::signal() const {
	return _signal;
}

SamplesModel * SampleProvider::spectre() const {
	return _spectre;
}

const QVariantList & SampleProvider::areas() const {
	return _areas;
}

qreal SampleProvider::frequencyStep() const {
	return _rate / qreal(_windowSize);
}

void SampleProvider::setData(const std::vector<float> & data, qreal rate) {
	std::vector<float> s;
	std::vector<float> v(data.begin(), data.begin() + std::min(int(data.size()), 512));
	int exp = FFT::transform(s, v, MIN_FREQUENCY, MAX_FREQUENCY, rate);
	_rate = rate;
	_windowSize = 1 << exp;

	_spectre->setData(std::move(s));
	_signal->setData(data);

	emit frequencyStepChanged();
}

void SampleProvider::setData(std::vector<float> && data, qreal rate) {
	std::vector<float> s;
	std::vector<float> v(data.begin(), data.begin() + std::min(int(data.size()), 512));
	int exp = FFT::transform(s, v, MIN_FREQUENCY, MAX_FREQUENCY, rate);
	_rate = rate;
	_windowSize = 1 << exp;

	_spectre->setData(std::move(s));
	_signal->setData(std::forward<std::vector<float>>(data));

	emit frequencyStepChanged();
}

void SampleProvider::addArea(int start, int end, const QColor & color) {
	QVariantMap area;
	area["start"] = start;
	area["end"] = end;
	area["color"] = QVariant::fromValue(color);
	_areas.append(QVariant::fromValue(area));
	emit areasChanged();
}

void SampleProvider::setAreaStart(int index, int start) {
	QVariantMap area = qvariant_cast<QVariantMap>(_areas[index]);
	area["start"] = start;
	_areas[index] = QVariant::fromValue(area);
	emit areasChanged();
}

void SampleProvider::setAreaEnd(int index, int end) {
	QVariantMap area = qvariant_cast<QVariantMap>(_areas[index]);
	area["end"] = end;
	_areas[index] = QVariant::fromValue(area);
	emit areasChanged();
}

void SampleProvider::setAreaColor(int index, const QColor & color) {
	QVariantMap area = qvariant_cast<QVariantMap>(_areas[index]);
	area["color"] = QVariant::fromValue(color);
	_areas[index] = QVariant::fromValue(area);
	emit areasChanged();
}

void SampleProvider::modifyArea(int index, int start, int end, const QColor & color) {
	QVariantMap area;
	area["start"] = start;
	area["end"] = end;
	area["color"] = QVariant::fromValue(color);
	_areas[index] = QVariant::fromValue(area);
	emit areasChanged();
}

void SampleProvider::removeArea(int index) {
	_areas.removeAt(index);
	emit areasChanged();
}

void SampleProvider::updateAreas() {
	emit areasChanged();
}

bool SampleProvider::serialize(const QString & filepath) {
	auto & data = _signal->data();

	if(data.size() == 0) {
		qWarning() << "There is not valid data to save to file" << filepath + ".txt";
		return false;
	}

	QFile file(filepath + ".txt");

	if(!file.open(QFile::WriteOnly)) {
		qWarning() << "Can't open file" << filepath + ".txt" << "for write";
		return false;
	}

	QByteArray bytes;

	for(auto val : data) {
		bytes.append(QByteArray::number(val) + "\n");
	}

	file.write(bytes);
	file.close();

	serializeAreas(filepath);
	return true;
}

bool SampleProvider::serializeAreas(const QString & filepath) {
	QFile areas(filepath + ".areas");

	if(_areas.isEmpty()) {
		if(areas.exists()) {
			areas.remove();
		}

		return true;
	}


	if(!areas.open(QFile::WriteOnly)){
		qWarning() << "Can't open file" << areas.fileName() << "for write";
		return false;
	}

	QByteArray bytes;

	for(auto & area : _areas) {
		QVariantMap map = qvariant_cast<QVariantMap>(area);
		bytes.append(map["start"].toString() + " " + map["end"].toString() + " " + map["color"].toString() + "\n");
	}

	areas.write(bytes);
	areas.close();

	return true;
}

bool SampleProvider::unserialize(const QString & filepath) {
	QFile file(filepath + ".txt");

	if(!file.open(QFile::ReadOnly)) {
		qWarning() << "Can't open file" << filepath + ".txt" << "for read";
		return false;
	}

	std::vector<float> signal;

	{
		auto bytes = file.readAll();
		file.close();

		for(auto & v : bytes.split('\n')) {
			if(v.size() > 0) {
				signal.push_back(v.toFloat());
			}
		}
	}

	QFile areas(filepath + ".areas");

	_areas.clear();

	if(areas.exists()) {
		if(areas.open(QFile::ReadOnly)) {
			auto bytes = areas.readAll();
			areas.close();

			QVariantMap map;

			for(auto & v : bytes.split('\n')) {
				if(v.isEmpty()) {
					continue;
				}

				QList<QByteArray> a = v.split(' ');

				if(a.size() < 2) {
					qWarning() << "Invalid area record in" << areas.fileName();
					continue;
				}

				map["start"] = a[0].toInt();
				map["end"] = a[1].toInt();
				map["color"] = QColor(QLatin1String(a[2]));
				_areas.append(QVariant::fromValue(map));
			}
		} else {
			qWarning() << "Can't open file" << areas.fileName() << "for read";
		}
	}

	qDebug() << "Read" << signal.size() << "values";

	setData(std::move(signal), 964);
	return true;
}

bool SampleProvider::readWav(const QString & filepath) {
#ifdef Q_OS_ANDROID
	qDebug() << "Opening WAV file at: " << filepath;

	SndfileHandle wav(filepath.toUtf8());

	qDebug() << "Channels:" << wav.channels();
	qDebug() << "Sample rate:" << wav.samplerate();

	std::vector<float> v, out;
	v.resize(wav.frames() * wav.channels());

	wav.read(v.data(), v.size());

	for(int i = 0; i < v.size(); i += wav.channels()) {
		out.push_back(v[i]);
	}

	setData(std::move(out), wav.samplerate());
	return true;
#else
	qWarning() << "Wav file support wasn't implemented for this platform :(";
	return false;
#endif
}

bool SampleProvider::writeWav(const QString & filepath) const {
	return false;
}

