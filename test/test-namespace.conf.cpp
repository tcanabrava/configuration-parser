#include "test-namespace.h"
#include <QSettings>

namespace Conf {

InnerInnerPrefs1::InnerInnerPrefs1(QObject *parent) : QObject(parent){
}

int InnerInnerPrefs1::blah() const
{
	return _blah;
}

void InnerInnerPrefs1::setBlah(int value)
{
	if(blahRule && !blahRule(value))
		return;
	 _blah = value;
	emit blahChanged(value);
}

void InnerInnerPrefs1::setBlahRule(std::function<bool(int)> rule)
{
	blahRule = rule;
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
	QSettings s;
	s.beginGroup("InnerPrefs1");
		s.beginGroup("InnerInnerPrefs1");
			s.setValue("blah",innerPrefs1()->innerInnerPrefs1()->blah());
		s.endGroup();
	s.endGroup();
	s.beginGroup("InnerPrefs2");
	s.endGroup();
}

void Preferences::load()
{
	QSettings s;
	s.beginGroup("InnerPrefs1");
		s.beginGroup("InnerInnerPrefs1");
			innerPrefs1()->innerInnerPrefs1()->setBlah(s.value("blah").value<int>());
		s.endGroup();
	s.endGroup();
	s.beginGroup("InnerPrefs2");
	s.endGroup();
}

Preferences* Preferences::self()
{
	static Preferences s;
	return &s;
}
}
