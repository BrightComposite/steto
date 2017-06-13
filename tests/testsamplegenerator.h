#ifndef TESTSAMPLEGENERATOR_H
#define TESTSAMPLEGENERATOR_H

#include <QObject>
#include <SampleProvider>

class TestSampleGenerator : public QObject
{
	Q_OBJECT
	Q_PROPERTY(SampleProvider * provider READ provider CONSTANT)
public:
	explicit TestSampleGenerator(QObject *parent = 0);

	SampleProvider * provider() const;

signals:

public slots:

private:
	SampleProvider * _provider;
};

#endif // TESTSAMPLEGENERATOR_H
