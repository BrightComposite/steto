#ifndef SPECTREMODEL_H
#define SPECTREMODEL_H

#include <models/samplesmodel.h>

class SpectreModel : public SamplesModel
{
	Q_OBJECT
public:
	explicit SpectreModel(QObject * parents = nullptr);

signals:

public slots:

protected:
	virtual qreal sample(uint index, uint delta, uint end) override {
		qreal val = 0;

		for(uint i = 0; i < delta && index + i < end; ++i) {
			if(_data[index + i] > val) {
				val = _data[index + i];
			}
		}

		return val;
	}
};

#endif // SPECTREMODEL_H
