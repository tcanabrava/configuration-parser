#pragma once

#include <functional>
#include <QObject>

#include <test1>

class Preferences : public QObject {
Q_OBJECT

public:
	void sync();
	void load();
	static Preferences* self();

private:
	Preferences(QObject *parent = 0);
};

