#ifndef QJSON_VALUEREF_H_
#define QJSON_VALUEREF_H_
#include <QtCore/QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtCore/QJsonValueRef>
#else
#include "QJsonValue.h"
class QJsonRoot;
class QJsonValueRef {
public:
	QJsonValueRef(QJsonArray *array, int idx);
	QJsonValueRef(QJsonObject *object, const QString &key);
public:
	operator QJsonValue() const;
public:
	QJsonValueRef &operator=(const QJsonValue &val);
	QJsonValueRef &operator=(const QJsonValueRef &val);
public:
	QJsonValue::Type type() const;
	bool isNull() const;
	bool isBool() const;
	bool isDouble() const;
	bool isString() const;
	bool isArray() const;
	bool isObject() const;
	bool isUndefined() const;
public:
	bool toBool() const;
	double toDouble() const;
	QString toString() const;
	QJsonArray toArray() const;
	QJsonObject toObject() const;
	int toInt(int defaultValue = 0) const;
public:
	bool operator==(const QJsonValue &other) const;
	bool operator!=(const QJsonValue &other) const;
private:
	QJsonValue toValue() const;
	void swap(QJsonValueRef &other);
private:
	QJsonRoot *p_;
	int        index_;
	QString    key_;
};
#endif
#endif