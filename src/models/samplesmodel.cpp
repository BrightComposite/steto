#include "samplesmodel.h"
#include <QDebug>

SamplesModel::SamplesModel(QObject * parent) : QObject(parent) {

}

const QList<qreal> & SamplesModel::samples() const {
	return _samples;
}

uint SamplesModel::offset() const {
	return _offset;
}

uint SamplesModel::count() const {
	return _count;
}

uint SamplesModel::total() const {
	return uint(_data.size());
}

qreal SamplesModel::max() const {
	return _max;
}

qreal SamplesModel::period() const {
	return _period;
}

const std::vector<float> & SamplesModel::data() const {
	return _data;
}

void SamplesModel::setOffset(int value) {
	setRawOffset(value);
	updateSamples();
}

void SamplesModel::setRawOffset(int value) {
	if(value < 0) {
		value = 0;
	}

	_offset = uint(value);
	emit offsetChanged();
}

void SamplesModel::setCount(uint value) {
	_count = value;
	emit countChanged();

	updateSamples();
}

void SamplesModel::setPeriod(qreal value) {
	_period = std::min(qreal(1 << (_exponent - _viewExponent)), value);
	emit periodChanged();

	updateSamples();
}

void SamplesModel::setData(const std::vector<float> & data) {
	_data = data;

	_max = 0;
	uint step = std::max(uint(1), uint(_period));

	for(uint i = 0; i < _data.size(); i += step) {
		auto s = std::abs(_data[i]);

		if(s > _max) {
			_max = s;
		}
	}

	updateSamples();
}

void SamplesModel::setData(std::vector<float> && data) {
	_data = std::move(data);

	_max = 0;
	uint step = std::max(uint(1), uint(_period));

	for(uint i = 0; i < _data.size(); i += step) {
		auto s = std::abs(_data[i]);

		if(s > _max) {
			_max = s;
		}
	}

	updateSamples();
}

void SamplesModel::updateSamples() {
	uint start = _offset;

	if(start + _count * _period > _data.size()) {
		start = std::max(uint(0), uint(_data.size() - _count * _period));
	}

	uint step = std::max(uint(1), uint(_period));
	uint end  = std::min(uint(start + _count * _period), uint(_data.size()));

	_samples.clear();

	for(uint i = start; i < end; i += step) {
		auto s = sample(i, step, end);
		_samples.push_back(s);
	}

	emit samplesChanged();
}
