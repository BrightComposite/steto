#include "audioservice.h"
#include <QDebug>

#include <models/SamplesModel>

AudioService::AudioService(QObject *parent) : QObject(parent)
{

}

void AudioService::start(SampleProvider * dataService) {
	if(output) {
		output->stop();
		delete output;
		output = nullptr;
	}

	QAudioFormat format;
	// Set up the format, eg.
	format.setSampleRate(8000);
	format.setChannelCount(1);
	format.setSampleSize(8);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format)) {
		qWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return;
	}

	output = new QAudioOutput(format, this);
	connect(output, &QAudioOutput::stateChanged, [this](QAudio::State newState) {
		switch (newState) {
			case QAudio::IdleState:
				output->stop();
				delete output;
				output = nullptr;
				break;

			case QAudio::StoppedState:
				if (output->error() != QAudio::NoError) {
					qWarning() << output->error();
				}

				break;

			default:
				break;
		}
	});

	QIODevice * device = output->start();

	for(float b : dataService->signal()->data()) {
		device->putChar(char(b));
	}
}
