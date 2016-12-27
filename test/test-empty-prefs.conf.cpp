#include "test-empty-prefs.h"
#include <QSettings>

Preferences::Preferences(QObject *parent) : QObject(parent){
}

void Preferences::sync()
{
}

void Preferences::load()
{
}

Preferences* Preferences::self()
{
	static Preferences s;
	return &s;
}
