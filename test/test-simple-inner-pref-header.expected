#pragma once

#include <functional>
#include <QObject>

class InnerStruct : public QObject {
Q_OBJECT

public:
	InnerStruct(QObject *parent = 0);
};

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(QObject* inner_struct MEMBER _innerStruct CONSTANT);

public:
	void sync();
	void load();
	static Preferences* self();
	InnerStruct *innerStruct();

private:
	InnerStruct *_innerStruct;
	Preferences(QObject *parent = 0);
};

