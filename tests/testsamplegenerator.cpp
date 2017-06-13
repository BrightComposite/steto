#include "testsamplegenerator.h"

TestSampleGenerator::TestSampleGenerator(QObject *parent) : QObject(parent) {
	_provider = new SampleProvider(this);

	const int samplesCount = 65536;
	const qreal rate = 1024;

	std::vector<float> buffer;
	std::vector<std::array<float, 3>> afp;

	for(int i = 0; i < 48; ++i) {
		int a = 1 << (qrand() % 8);
		int f = 1 << (qrand() % 16);
		int p = 1 << (qrand() % 16);

		afp.push_back(std::array<float, 3>{
			(a + (qrand() % a)),
			(f + (qrand() % f)) / 256.0f,
			(p + (qrand() % p)) * 2 * M_PI / 65536.0f
		});
	}

	buffer.resize(samplesCount, 0.0f);

	for(auto a : afp) {
		for(int i = 0; i < buffer.size(); i++) {
			buffer[i] += a[0] * std::sin(float(2 * M_PI * i * a[1] / rate + a[2]));
		}
	}

	_provider->setData(buffer, rate);
}

SampleProvider * TestSampleGenerator::provider() const {
	return _provider;
}
