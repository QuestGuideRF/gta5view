#ifndef QJSON_OBJECT_H_
#define QJSON_OBJECT_H_
#include <QtCore/QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtCore/QJsonObject>
#else
#include "QJsonRoot.h"
#include "QJsonValueRef.h"
#include "QJsonValue.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QMap>
class QJsonObject : public QJsonRoot {
	friend class QJsonDocument;
	friend class QJsonValue;
	friend class QJsonValueRef;
	friend class QJsonParser;
public:
	typedef QMap<QString, QJsonValue>::const_iterator const_iterator;
	typedef QMap<QString, QJsonValue>::iterator       iterator;
	typedef const_iterator                            ConstIterator;
	typedef iterator                                  Iterator;
	typedef QMap<QString, QJsonValue>::key_type       key_type;
	typedef QMap<QString, QJsonValue>::mapped_type    mapped_type;
	typedef QMap<QString, QJsonValue>::size_type      size_type;
public:
	QJsonObject();
#if __cplusplus >= 201103L
	QJsonObject(std::initializer_list<QPair<QString, QJsonValue> > args);
#endif
	QJsonObject(const QJsonObject &other);
	~QJsonObject();
	QJsonObject &operator=(const QJsonObject &other);
public:
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	const_iterator constBegin() const;
	const_iterator constEnd() const;
public:
	int count() const;
	int length() const;
	int size() const;
	bool empty() const;
	bool isEmpty() const;
public:
	const_iterator constFind(const QString &key) const;
	bool contains(const QString &key) const;
	iterator find(const QString &key);
	const_iterator find(const QString &key) const;
public:
	iterator erase(iterator it);
	iterator insert(const QString &key, const QJsonValue &value);
	QStringList keys() const;
	void remove(const QString &key);
	QJsonValue take(const QString &key);
	QJsonValue value(const QString &key) const;
	bool operator!=(const QJsonObject &other) const;
	bool operator==(const QJsonObject &other) const;
	QJsonValue operator[](const QString &key) const;
	QJsonValueRef operator[](const QString &key);
public:
	QVariantMap toVariantMap() const;
	QVariantHash toVariantHash() const;
public:
	static QJsonObject fromVariantMap(const QVariantMap &map);
	static QJsonObject fromVariantHash(const QVariantHash &hash);
private:
	virtual QJsonRoot *clone() const;
	virtual QJsonArray *toArray();
	virtual QJsonObject *toObject();
	virtual const QJsonArray *toArray() const;
	virtual const QJsonObject *toObject() const;
private:
	void swap(QJsonObject &other);
private:
	QMap<QString, QJsonValue> values_;
};
#endif
#endif