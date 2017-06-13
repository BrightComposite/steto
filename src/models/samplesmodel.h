#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <SampleProvider>

class SamplesModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QList<qreal> samples READ samples NOTIFY samplesChanged)
	Q_PROPERTY(uint offset READ offset WRITE setOffset NOTIFY offsetChanged)
	Q_PROPERTY(uint count READ count WRITE setCount NOTIFY countChanged)
	Q_PROPERTY(uint total READ total NOTIFY samplesChanged)
	Q_PROPERTY(qreal max READ max NOTIFY samplesChanged)
	Q_PROPERTY(qreal period READ period WRITE setPeriod NOTIFY periodChanged)
public:
	explicit SamplesModel(QObject * parent = nullptr);

	const QList<qreal> & samples() const;
	uint offset() const;
	uint count() const;
	uint total() const;
	qreal max() const;
	qreal period() const;
	const std::vector<float> & data() const;

	void setOffset(int value);
	void setRawOffset(int value);
	void setCount(uint value);
	void setPeriod(qreal value);
	void setData(const std::vector<float> & data);
	void setData(std::vector<float> && data);

signals:
	void samplesChanged();
	void offsetChanged();
	void countChanged();
	void periodChanged();

protected:
	void updateSamples();

	virtual qreal sample(uint index, uint/* step */, uint/* end */) {
		return _data[index];
	}

	QList<qreal> _samples;
	std::vector<float> _data;
	uint _viewExponent = 7;
	uint _exponent = 10;
	uint _offset = 0;
	uint _count = 128;
	qreal _max = 0;
	qreal _period = 1;
};

#endif // CHARTMODEL_H
