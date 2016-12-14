#pragma once

#include <functional>
#include <QObject>

#include <QPoint>

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(QPoint point READ point WRITE setPoint NOTIFY pointChanged)
Q_PROPERTY(std::string name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)

public:
	void sync();
	void load();
	static Preferences* self();
	QPoint point() const;
	void setPointRule(std::function<bool(QPoint)> rule);
	std::string name() const;
	void setNameRule(std::function<bool(std::string)> rule);
	int age() const;
	void setAgeRule(std::function<bool(int)> rule);

public slots:
	void setPoint(QPoint value);
	void setName(std::string value);
	void setAge(int value);

signals:
	void pointChanged(QPoint value);
	void nameChanged(std::string value);
	void ageChanged(int value);

private:
	QPoint _point;
	std::function<bool(QPoint)> pointRule;
	std::string _name;
	std::function<bool(std::string)> nameRule;
	int _age;
	std::function<bool(int)> ageRule;
	Preferences(QObject *parent = 0);
};

