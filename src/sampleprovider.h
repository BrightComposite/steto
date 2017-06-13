#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <QObject>
#include <QColor>
#include <QVariantList>

#include <fft.h>

class SamplesModel;

class SampleProvider : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariantList areas READ areas NOTIFY areasChanged)
	Q_PROPERTY(SamplesModel * signal READ signal CONSTANT)
	Q_PROPERTY(SamplesModel * spectre READ spectre CONSTANT)
	Q_PROPERTY(qreal frequencyStep READ frequencyStep NOTIFY frequencyStepChanged)

public:
	enum Sizes {
		MIN_FREQUENCY = 1,
		MAX_FREQUENCY = 2000
	};
	Q_ENUM(Sizes)

	explicit SampleProvider(QObject * parent = nullptr);

	SamplesModel * signal() const;
	SamplesModel * spectre() const;
	const QVariantList & areas() const;
	qreal frequencyStep() const;

	void setData(const std::vector<float> & data, qreal rate);
	void setData(std::vector<float> && data, qreal rate);

signals:
	void dataChanged();
	void spectreChanged();
	void areasChanged();
	void frequencyStepChanged();

public slots:
	void addArea(int start, int end, const QColor & color);
	void modifyArea(int index, int start, int end, const QColor & color);
	void setAreaStart(int index, int start);
	void setAreaEnd(int index, int end);
	void setAreaColor(int index, const QColor & color);
	void removeArea(int index);
	void updateAreas();

	bool serialize(const QString & filepath);
	bool serializeAreas(const QString & filepath);
	bool unserialize(const QString & filepath);

	bool readWav(const QString & filepath);
	bool writeWav(const QString & filepath) const;

private:
	SamplesModel * _signal;
	SamplesModel * _spectre;
	QVariantList _areas;
	int _windowSize;
	qreal _rate;
};

#endif // DATASERVICE_H
