#include "QJsonObject.h"
#if QT_VERSION < 0x050000
QJsonObject::QJsonObject() {
}
QJsonObject::QJsonObject(const QJsonObject &other) : values_(other.values_) {
}
#if __cplusplus >= 201103L
QJsonObject::QJsonObject(std::initializer_list<QPair<QString, QJsonValue> > args) {
	for(const QPair<QString, QJsonValue> &arg : args) {
		values_.insert(arg.first, arg.second);
	}
}
#endif
QJsonObject::~QJsonObject() {
}
QJsonObject &QJsonObject::operator=(const QJsonObject &other) {
	QJsonObject(other).swap(*this);
	return *this;
}
QJsonObject::iterator QJsonObject::begin() {
	return values_.begin();
}
QJsonObject::const_iterator QJsonObject::begin() const {
	return values_.begin();
}
QJsonObject::iterator QJsonObject::end() {
	return values_.end();
}
QJsonObject::const_iterator QJsonObject::end() const {
	return values_.end();
}
QJsonObject::const_iterator QJsonObject::constBegin() const {
	return begin();
}
QJsonObject::const_iterator QJsonObject::constEnd() const {
	return end();
}
int QJsonObject::count() const {
	return size();
}
int QJsonObject::length() const {
	return size();
}
int QJsonObject::size() const {
	return values_.size();
}
bool QJsonObject::empty() const {
	return values_.empty();
}
bool QJsonObject::isEmpty() const {
	return empty();
}
QJsonObject::const_iterator QJsonObject::constFind(const QString &key) const {
	return values_.find(key);
}
bool QJsonObject::contains(const QString &key) const {
	return values_.contains(key);
}
QJsonObject::iterator QJsonObject::find(const QString &key) {
	return values_.find(key);
}
QJsonObject::const_iterator QJsonObject::find(const QString &key) const {
	return values_.find(key);
}
QJsonObject::iterator QJsonObject::erase(iterator it) {
	return values_.erase(it);
}
QJsonObject::iterator QJsonObject::insert(const QString &key, const QJsonValue &value) {
	return values_.insert(key, value);
}
QStringList QJsonObject::keys() const {
	return values_.keys();
}
void QJsonObject::remove(const QString &key) {
	values_.remove(key);
}
QJsonValue QJsonObject::take(const QString &key) {
	return values_.take(key);
}
QJsonValue QJsonObject::value(const QString &key) const {
	return values_.value(key);
}
bool QJsonObject::operator!=(const QJsonObject &other) const {
	return values_ != other.values_;
}
bool QJsonObject::operator==(const QJsonObject &other) const {
	return values_ != other.values_;
}
QJsonValue QJsonObject::operator[](const QString &key) const {
	return values_[key];
}
QJsonValueRef QJsonObject::operator[](const QString &key) {
	return QJsonValueRef(this, key);
}
QVariantMap QJsonObject::toVariantMap() const {
	QVariantMap a;
	for(const_iterator it = begin(); it != end(); ++it) {
		a.insert(it.key(), it.value().toVariant());
	}
	return a;
}
QVariantHash QJsonObject::toVariantHash() const {
	QVariantHash a;
	for(const_iterator it = begin(); it != end(); ++it) {
		a.insert(it.key(), it.value().toVariant());
	}
	return a;
}
QJsonObject QJsonObject::fromVariantMap(const QVariantMap &map) {
	QJsonObject o;
	for(QVariantMap::const_iterator it = map.begin(); it != map.end(); ++it) {
		o.insert(it.key(), QJsonValue::fromVariant(it.value()));
	}
	return o;
}
QJsonObject QJsonObject::fromVariantHash(const QVariantHash &hash) {
	QJsonObject o;
	for(QVariantHash::const_iterator it = hash.begin(); it != hash.end(); ++it) {
		o.insert(it.key(), QJsonValue::fromVariant(it.value()));
	}
	return o;
}
QJsonRoot *QJsonObject::clone() const {
	return new QJsonObject(*this);
}
const QJsonObject *QJsonObject::toObject() const {
	return this;
}
QJsonObject *QJsonObject::toObject() {
	return this;
}
QJsonArray *QJsonObject::toArray() {
	return 0;
}
const QJsonArray *QJsonObject::toArray() const {
	return 0;
}
void QJsonObject::swap(QJsonObject &other) {
	qSwap(values_, other.values_);
}
#endif