#include "QJsonValueRef.h"
#if QT_VERSION < 0x050000
#include "QJsonArray.h"
#include "QJsonObject.h"
QJsonValueRef::QJsonValueRef(QJsonArray *array, int idx) : p_(array), index_(idx) {
}
QJsonValueRef::QJsonValueRef(QJsonObject *object, const QString &key) : p_(object), index_(0), key_(key) {
}
QJsonValueRef::operator QJsonValue() const {
	return toValue();
}
QJsonValueRef &QJsonValueRef::operator=(const QJsonValue &val) {
	if(QJsonObject *const o = p_->toObject()) {
		o->values_[key_] = val;
	} else if(QJsonArray *const a = p_->toArray()) {
		a->values_[index_] = val;
	}
	return *this;
}
QJsonValueRef &QJsonValueRef::operator=(const QJsonValueRef &ref) {
	if(QJsonObject *const o = p_->toObject()) {
		o->values_[key_] = ref;
	} else if(QJsonArray *const a = p_->toArray()) {
		a->values_[index_] = ref;
	}
	return *this;
}
QJsonValue::Type QJsonValueRef::type() const {
	return toValue().type();
}
bool QJsonValueRef::isNull() const {
	return toValue().isNull();
}
bool QJsonValueRef::isBool() const {
	return toValue().isBool();
}
bool QJsonValueRef::isDouble() const {
	return toValue().isDouble();
}
bool QJsonValueRef::isString() const {
	return toValue().isString();
}
bool QJsonValueRef::isArray() const {
	return toValue().isArray();
}
bool QJsonValueRef::isObject() const {
	return toValue().isObject();
}
bool QJsonValueRef::isUndefined() const {
	return toValue().isUndefined();
}
bool QJsonValueRef::toBool() const {
	return toValue().toBool();
}
double QJsonValueRef::toDouble() const {
	return toValue().toDouble();
}
int QJsonValueRef::toInt(int defaultValue) const {
	return toValue().toInt(defaultValue);
}
QString QJsonValueRef::toString() const {
	return toValue().toString();
}
QJsonArray QJsonValueRef::toArray() const {
	return toValue().toArray();
}
QJsonObject QJsonValueRef::toObject() const {
	return toValue().toObject();
}
bool QJsonValueRef::operator==(const QJsonValue &other) const {
	return toValue() == other;
}
bool QJsonValueRef::operator!=(const QJsonValue &other) const {
	return toValue() != other;
}
QJsonValue QJsonValueRef::toValue() const {
	if(QJsonObject *const o = p_->toObject()) {
		return o->values_[key_];
	} else if(QJsonArray *const a = p_->toArray()) {
		return a->values_[index_];
	}
	return QJsonValue();
}
void QJsonValueRef::swap(QJsonValueRef &other) {
	qSwap(p_, other.p_);
	qSwap(key_, other.key_);
	qSwap(index_, other.index_);
}
#endif