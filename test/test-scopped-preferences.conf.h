#pragma once

#include <functional>
#include <QObject>

#include <test1>

class InnerInnerPrefs1 : public QObject {
Q_OBJECT

public:
	InnerInnerPrefs1(QObject *parent = 0);
};

class InnerPrefs1 : public QObject {
Q_OBJECT
Q_PROPERTY(QObject* inner_inner_prefs1 MEMBER _innerInnerPrefs1 CONSTANT);

public:
	InnerPrefs1(QObject *parent = 0);
	InnerInnerPrefs1 *innerInnerPrefs1() const;

private:
	InnerInnerPrefs1 *_innerInnerPrefs1;
};

class InnerPrefs2 : public QObject {
Q_OBJECT

public:
	InnerPrefs2(QObject *parent = 0);
};

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(QObject* inner_prefs1 MEMBER _innerPrefs1 CONSTANT);
Q_PROPERTY(QObject* inner_prefs2 MEMBER _innerPrefs2 CONSTANT);

public:
	void sync();
	void load();
	static Preferences* self();
	InnerPrefs1 *innerPrefs1() const;
	InnerPrefs2 *innerPrefs2() const;

private:
	InnerPrefs1 *_innerPrefs1;
	InnerPrefs2 *_innerPrefs2;
	Preferences(QObject *parent = 0);
};

