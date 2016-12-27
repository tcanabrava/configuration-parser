#pragma once

#include <functional>
#include <QObject>

#include <QString>

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)

public:
	void sync();
	void load();
	static Preferences* self();
	QString name() const;
	void setNameRule(std::function<bool(QString)> rule);
	int age() const;
	void setAgeRule(std::function<bool(int)> rule);

public slots:
	void setName(QString value);
	void setAge(int value);

signals:
	void nameChanged(QString value);
	void ageChanged(int value);

private:
	QString _name;
	std::function<bool(QString)> nameRule;
	int _age;
	std::function<bool(int)> ageRule;
	Preferences(QObject *parent = 0);
};

