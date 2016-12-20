#include "test-scopped-preferences.h"
#include <QSettings>

InnerInnerPrefs1::InnerInnerPrefs1(QObject *parent) : QObject(parent){
}

InnerPrefs1::InnerPrefs1(QObject *parent) : QObject(parent),
	_innerInnerPrefs1(new InnerInnerPrefs1(this)){
}

InnerInnerPrefs1* InnerPrefs1::innerInnerPrefs1() const
{
	return _innerInnerPrefs1;
}

InnerPrefs2::InnerPrefs2(QObject *parent) : QObject(parent){
}

Preferences::Preferences(QObject *parent) : QObject(parent),
	_innerPrefs1(new InnerPrefs1(this)),
	_innerPrefs2(new InnerPrefs2(this)){
}

InnerPrefs1* Preferences::innerPrefs1() const
{
	return _innerPrefs1;
}

InnerPrefs2* Preferences::innerPrefs2() const
{
	return _innerPrefs2;
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
