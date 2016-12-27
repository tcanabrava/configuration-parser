#pragma once

#include <functional>
#include <QObject>

#include <QString>

class Status : public QObject {
Q_OBJECT
Q_PROPERTY(int strength READ strength WRITE setStrength NOTIFY strengthChanged)
Q_PROPERTY(int wisdom READ wisdom WRITE setWisdom NOTIFY wisdomChanged)
Q_PROPERTY(int agility READ agility WRITE setAgility NOTIFY agilityChanged)
Q_PROPERTY(int inteligence READ inteligence WRITE setInteligence NOTIFY inteligenceChanged)

public:
	Status(QObject *parent = 0);
	int strength() const;
	void setStrengthRule(std::function<bool(int)> rule);
	int wisdom() const;
	void setWisdomRule(std::function<bool(int)> rule);
	int agility() const;
	void setAgilityRule(std::function<bool(int)> rule);
	int inteligence() const;
	void setInteligenceRule(std::function<bool(int)> rule);

public slots:
	void setStrength(int value);
	void setWisdom(int value);
	void setAgility(int value);
	void setInteligence(int value);

signals:
	void strengthChanged(int value);
	void wisdomChanged(int value);
	void agilityChanged(int value);
	void inteligenceChanged(int value);

private:
	int _strength;
	std::function<bool(int)> strengthRule;
	int _wisdom;
	std::function<bool(int)> wisdomRule;
	int _agility;
	std::function<bool(int)> agilityRule;
	int _inteligence;
	std::function<bool(int)> inteligenceRule;
};

class Equipment : public QObject {
Q_OBJECT

public:
	Equipment(QObject *parent = 0);
};

class Character : public QObject {
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)
Q_PROPERTY(int gold READ gold WRITE setGold NOTIFY goldChanged)
Q_PROPERTY(race player_race READ playerRace WRITE setPlayerRace NOTIFY playerRaceChanged)
Q_PROPERTY(QObject* status MEMBER _status CONSTANT);
Q_PROPERTY(QObject* equipment MEMBER _equipment CONSTANT);

public:
	Character(QObject *parent = 0);
	Status *status() const;
	Equipment *equipment() const;
	QString name() const;
	void setNameRule(std::function<bool(QString)> rule);
	int age() const;
	void setAgeRule(std::function<bool(int)> rule);
	int gold() const;
	void setGoldRule(std::function<bool(int)> rule);
	race playerRace() const;
	void setPlayerRaceRule(std::function<bool(race)> rule);

public slots:
	void setName(QString value);
	void setAge(int value);
	void setGold(int value);
	void setPlayerRace(race value);

signals:
	void nameChanged(QString value);
	void ageChanged(int value);
	void goldChanged(int value);
	void playerRaceChanged(race value);

private:
	QString _name;
	std::function<bool(QString)> nameRule;
	int _age;
	std::function<bool(int)> ageRule;
	int _gold;
	std::function<bool(int)> goldRule;
	race _playerRace;
	std::function<bool(race)> playerRaceRule;
	Status *_status;
	Equipment *_equipment;
};

class Preferences : public QObject {
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)
Q_PROPERTY(QObject* character MEMBER _character CONSTANT);

public:
	void sync();
	void load();
	static Preferences* self();
	Character *character() const;
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
	Character *_character;
	Preferences(QObject *parent = 0);
};

