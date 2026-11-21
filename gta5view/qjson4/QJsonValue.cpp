#include "QJsonValue.h"
#include "QJsonArray.h"
#include "QJsonObject.h"
#if QT_VERSION < 0x050000
#include <QtCore/QtAlgorithms>
#include <QtCore/qmath.h>
QJsonValue::QJsonValue(Type type) : type_(type) {
}
QJsonValue::QJsonValue(bool b) : type_(Bool) {
	value_.b = b;
}
QJsonValue::QJsonValue(double n) : type_(Double) {
	value_.n = n;
}
QJsonValue::QJsonValue(const QString &s) : type_(String) {
	value_.s = new QString(s);
}
QJsonValue::QJsonValue(QLatin1String s) : type_(String) {
	value_.s = new QString(s);
}
#ifndef QT_NO_CAST_FROM_ASCII
QJsonValue::QJsonValue(const char *s) : type_(String) {
	value_.s = new QString(QString::fromUtf8(s));
}
#endif
QJsonValue::QJsonValue(const QJsonArray &a) : type_(Array) {
	value_.r = a.clone();
}
QJsonValue::QJsonValue(const QJsonObject &o) : type_(Object) {
	value_.r = o.clone();
}
QJsonValue::QJsonValue(int n) : type_(Double) {
	value_.n = n;
}
QJsonValue::QJsonValue(qint64 n) : type_(Double) {
	value_.n = n;
}
QJsonValue::QJsonValue(const QJsonValue &other) : type_(other.type_) {
	switch(other.type_) {
	case Bool:
		value_.b = other.value_.b;
		break;
	case Double:
		value_.n = other.value_.n;
		break;
	case String:
		value_.s = new QString(*other.value_.s);
		break;
	case Array:
	case Object:
		value_.r = other.value_.r->clone();
		break;
	case Undefined:
	case Null:
		value_ = other.value_;
		break;
	}
}
QJsonValue::~QJsonValue() {
	switch(type_) {
	case Null:
	case Bool:
	case Double:
	case Undefined:
		break;
	case String:
		delete value_.s;
		break;
	case Object:
	case Array:
		delete value_.r;
		break;
	}
}
QJsonValue &QJsonValue::operator=(const QJsonValue &other) {
	QJsonValue(other).swap(*this);
	return *this;
}
bool QJsonValue::operator!=(const QJsonValue &other) const {
	return !(*this == other);
}
bool QJsonValue::operator==(const QJsonValue &other) const {
	if(type_ == other.type_) {
		switch(type_) {
		case Null:
			return true;
		case Bool:
			return value_.b == other.value_.b;
		case Double:
			return value_.n == other.value_.n;
		case Undefined:
			return true;
		case String:
			return *value_.s == *other.value_.s;
		case Array:
			return *(value_.r->toArray()) == *(other.value_.r->toArray());
		case Object:
			return *(value_.r->toObject()) == *(other.value_.r->toObject());
		}
	}
	return false;
}
bool QJsonValue::isArray() const {
	return type_ == Array;
}
bool QJsonValue::isBool() const {
	return type_ == Bool;
}
bool QJsonValue::isDouble() const {
	return type_ == Double;
}
bool QJsonValue::isNull() const {
	return type_ == Null;
}
bool QJsonValue::isObject() const {
	return type_ == Object;
}
bool QJsonValue::isString() const {
	return type_ == String;
}
bool QJsonValue::isUndefined() const {
	return type_ == Undefined;
}
QJsonValue::Type QJsonValue::type() const {
	return type_;
}
QJsonArray QJsonValue::toArray(const QJsonArray &defaultValue) const {
	if(isArray()) {
		return *(value_.r->toArray());
	}
	return defaultValue;
}
QJsonArray QJsonValue::toArray() const {
	return toArray(QJsonArray());
}
bool QJsonValue::toBool(bool defaultValue) const {
	if(isBool()) {
		return value_.b;
	}
	return defaultValue;
}
double QJsonValue::toDouble(double defaultValue) const {
	if(isDouble()) {
		return value_.n;
	}
	return defaultValue;
}
int QJsonValue::toInt(int defaultValue) const {
	if(isDouble() && qFloor(value_.n) == value_.n) {
		return value_.n;
	}
	return defaultValue;
}
QJsonObject QJsonValue::toObject(const QJsonObject &defaultValue) const {
	if(isObject()) {
		return *(value_.r->toObject());
	}
	return defaultValue;
}
QJsonObject QJsonValue::toObject() const {
	return toObject(QJsonObject());
}
QString QJsonValue::toString(const QString &defaultValue) const {
	if(isString()) {
		return *value_.s;
	}
	return defaultValue;
}
QVariant QJsonValue::toVariant() const {
	switch(type_) {
	case Null:
		return QVariant();
	case Bool:
		return QVariant::fromValue(value_.b);
	case Double:
		return QVariant::fromValue(value_.n);
	case String:
		return QVariant::fromValue(*value_.s);
	case Array:
		return value_.r->toArray()->toVariantList();
	case Object:
		return value_.r->toObject()->toVariantMap();
	case Undefined:
		return QVariant();
	}
	return QVariant();
}
QJsonValue QJsonValue::fromVariant(const QVariant &variant) {
	if(variant.isNull()) {
		return QJsonValue(Null);
	}
	switch(variant.type()) {
	case QVariant::Bool:
		return QJsonValue(variant.toBool());
	case QVariant::Int:
		return QJsonValue(variant.toInt());
	case QVariant::Double:
	case QVariant::LongLong:
	case QVariant::ULongLong:
	case QVariant::UInt:
		return QJsonValue(variant.toDouble());
	case QVariant::String:
		return QJsonValue(variant.toString());
	case QVariant::List:
		return QJsonArray::fromVariantList(variant.toList());
	case QVariant::StringList:
		return QJsonArray::fromStringList(variant.toStringList());
	case QVariant::Map:
		return QJsonObject::fromVariantMap(variant.toMap());
	default:
		const QString s = variant.toString();
		if(!s.isEmpty()) {
			return QJsonValue(s);
		}
		break;
	}
	return QJsonValue(Null);
}
void QJsonValue::swap(QJsonValue &other) {
	qSwap(type_, other.type_);
	qSwap(value_, other.value_);
}
#endif