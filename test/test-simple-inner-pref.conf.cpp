#include "test-simple-inner-pref.h"
#include <QSettings>

InnerStruct::InnerStruct(QObject *parent) : QObject(parent){
}

Preferences::Preferences(QObject *parent) : QObject(parent),
	_innerStruct(new InnerStruct(this)){
}

InnerStruct* Preferences::innerStruct() const
{
	return _innerStruct;
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
