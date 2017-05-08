#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <QObject>
#include <BtConnection>

#include <ffft/FFTRealFixLen.h>

class DataService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool active READ active NOTIFY activeChanged)
	Q_PROPERTY(int count READ count NOTIFY valuesFlushed)
	Q_PROPERTY(float progress READ progress NOTIFY valuesFlushed)
	Q_PROPERTY(QList<int> displayed READ displayed NOTIFY valuesFlushed)
	Q_PROPERTY(QList<qreal> spectre READ spectre NOTIFY spectreChanged)

public:
	enum Sizes {
		SIZE_EXPONENT = 16,
		SIZE = 1 << SIZE_EXPONENT,
		DISPLAYED_SIZE = 128
	};
	Q_ENUM(Sizes)

	explicit DataService(BtConnection * connection = nullptr);

	bool active() const;
	int count() const;
	float progress() const;
	const QList<int> & displayed() const;
	const QList<qreal> & spectre() const;

signals:
	void activeChanged();
	void valuesFlushed();
	void spectreChanged();
	void completed();

public slots:
	void start();
	void setDisplayRange(int offset, uint count, uint samplePeriod);

private:
	QFuture<void> activate();
	QFuture<void> deactivate();

	bool _active = false;
	QList<int> _displayed;
	QList<qreal> _spectre;
	std::vector<float> _collected;
	BtConnection * _connection = nullptr;
	QTimer * _timer = nullptr;
	ffft::FFTRealFixLen<16> _fft;
};

#endif // IOSERVICE_H
