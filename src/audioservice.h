#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <QObject>
#include <QtMultimedia/QAudioOutput>

#include <SampleProvider>

class AudioService : public QObject
{
	Q_OBJECT
public:
	explicit AudioService(QObject *parent = 0);

signals:

public slots:
	void start(SampleProvider * dataService);

private:
	QAudioOutput * output = nullptr;
};

#endif // AUDIOSERVICE_H
