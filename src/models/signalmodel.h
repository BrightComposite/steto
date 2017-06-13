#ifndef SIGNALMODEL_H
#define SIGNALMODEL_H

#include <models/samplesmodel.h>

class SignalModel : public SamplesModel
{
	Q_OBJECT
public:
	explicit SignalModel(QObject * parent = nullptr);

signals:

public slots:

protected:
	virtual qreal sample(uint index, uint, uint) override {
		return _data[index];
	}

};

#endif // SIGNALMODEL_H
