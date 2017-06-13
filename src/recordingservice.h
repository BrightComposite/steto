#ifndef RECORDINGSERVICE_H
#define RECORDINGSERVICE_H

#include <QObject>
#include <QTimer>
#include <BtConnection>
#include <SampleProvider>

class RecordingService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)
	Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY activeChanged)
	Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(SampleProvider * provider READ provider CONSTANT)
public:
	enum Sizes {
		SIZE_EXP = 16,
		SIZE = 1 << SIZE_EXP
	}; Q_ENUM(Sizes)

	explicit RecordingService(BtConnection * connection = nullptr);

	bool isActive() const;
	bool isCompleted() const;
	float progress() const;
	SampleProvider * provider() const;

signals:
	void activeChanged();
	void activated();
	void deactivated();
	void completed();
	void progressChanged();

public slots:
	void start();
	void stop();
	void reset();

private:
	QFuture<void> activate();
	QFuture<void> deactivate();

	std::vector<float> _collected;
	bool _active = false;
	BtConnection * _connection = nullptr;
	SampleProvider * _provider = nullptr;
	QTimer * _timer = nullptr;
};

#endif // RECORDINGSERVICE_H
