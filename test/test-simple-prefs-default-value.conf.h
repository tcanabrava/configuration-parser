#pragma once

#include <functional>
#include <QObject>

#include <string>

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(std::string name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)

public:
	void sync();
	void load();
	static Preferences* self();
	std::string name() const;
	void setNameRule(std::function<bool(std::string)> rule);
	int age() const;
	void setAgeRule(std::function<bool(int)> rule);

public slots:
	void setName(std::string value);
	void setAge(int value);

signals:
	void nameChanged(std::string value);
	void ageChanged(int value);

private:
	std::string _name;
	std::function<bool(std::string)> nameRule;
	int _age;
	std::function<bool(int)> ageRule;
	Preferences(QObject *parent = 0);
};

