#include "QJsonArray.h"
#include "QJsonValueRef.h"
#include "QJsonValue.h"
#include <QtCore/QStringList>
#if QT_VERSION < 0x050000
QJsonArray::QJsonArray() {
}
QJsonArray::QJsonArray(const QJsonArray &other) : values_(other.values_) {
}
#if __cplusplus >= 201103L
QJsonArray::QJsonArray(std::initializer_list<QJsonValue> args) {
	for(const QJsonValue &arg : args) {
		values_.append(arg);
	}
}
#endif
QJsonArray::~QJsonArray() {
}
QJsonArray &QJsonArray::operator=(const QJsonArray &other) {
	QJsonArray(other).swap(*this);
	return *this;
}
QJsonArray &QJsonArray::operator+=(const QJsonValue &value) {
	values_.append(value);
	return *this;
}
QJsonArray &QJsonArray::operator<<(const QJsonValue &value) {
	values_.append(value);
	return *this;
}
QJsonArray QJsonArray::operator+(const QJsonValue &value) const {
	QJsonArray arr(*this);
	arr.append(value);
	return arr;
}
bool QJsonArray::operator!=(const QJsonArray &other) const {
	return values_ != other.values_;
}
bool QJsonArray::operator==(const QJsonArray &other) const {
	return values_ == other.values_;
}
QJsonArray::const_iterator QJsonArray::begin() const {
	return values_.begin();
}
QJsonArray::const_iterator QJsonArray::end() const {
	return values_.end();
}
QJsonArray::iterator QJsonArray::begin() {
	return values_.begin();
}
QJsonArray::iterator QJsonArray::end() {
	return values_.end();
}
QJsonArray::const_iterator QJsonArray::constBegin() const {
	return begin();
}
QJsonArray::const_iterator QJsonArray::constEnd() const {
	return end();
}
QJsonValue QJsonArray::first() const {
	Q_ASSERT(!empty());
	return values_.first();
}
QJsonValue QJsonArray::last() const {
	Q_ASSERT(!empty());
	return values_.last();
}
QJsonValueRef QJsonArray::operator[](int i) {
	return QJsonValueRef(this, i);
}
QJsonValue QJsonArray::operator[](int i) const {
	return values_[i];
}
QJsonValue QJsonArray::at(int i) const {
	return values_.at(i);
}
int QJsonArray::size() const {
	return values_.size();
}
int QJsonArray::count() const {
	return size();
}
bool QJsonArray::empty() const {
	return values_.empty();
}
bool QJsonArray::isEmpty() const {
	return empty();
}
void QJsonArray::pop_back() {
	values_.pop_back();
}
void QJsonArray::pop_front() {
	values_.pop_front();
}
void QJsonArray::push_back(const QJsonValue &value) {
	values_.push_back(value);
}
void QJsonArray::push_front(const QJsonValue &value) {
	values_.push_front(value);
}
void QJsonArray::append(const QJsonValue &value) {
	values_.append(value);
}
bool QJsonArray::contains(const QJsonValue &value) const {
	return values_.contains(value);
}
QJsonArray::iterator QJsonArray::erase(iterator it) {
	return values_.erase(it);
}
void QJsonArray::insert(int i, const QJsonValue &value) {
	values_.insert(i, value);
}
QJsonArray::iterator QJsonArray::insert(iterator before, const QJsonValue &value) {
	return values_.insert(before, value);
}
void QJsonArray::prepend(const QJsonValue &value) {
	values_.prepend(value);
}
void QJsonArray::removeAt(int i) {
	values_.removeAt(i);
}
void QJsonArray::removeFirst() {
	values_.removeFirst();
}
void QJsonArray::removeLast() {
	values_.removeLast();
}
void QJsonArray::replace(int i, const QJsonValue &value) {
	values_.replace(i, value);
}
QJsonValue QJsonArray::takeAt(int i) {
	return values_.takeAt(i);
}
QVariantList QJsonArray::toVariantList() const {
	QVariantList a;
	Q_FOREACH(const QJsonValue &v, *this) {
		a.push_back(v.toVariant());
	}
	return a;
}
QJsonArray QJsonArray::fromStringList(const QStringList &list) {
	QJsonArray a;
	Q_FOREACH(const QString &s, list) {
		a.push_back(QJsonValue(s));
	}
	return a;
}
QJsonArray QJsonArray::fromVariantList(const QVariantList &list) {
	QJsonArray a;
	Q_FOREACH(const QVariant &v, list) {
		a.push_back(QJsonValue::fromVariant(v));
	}
	return a;
}
QJsonRoot *QJsonArray::clone() const {
	return new QJsonArray(*this);
}
const QJsonObject *QJsonArray::toObject() const {
	return 0;
}
QJsonObject *QJsonArray::toObject() {
	return 0;
}
QJsonArray *QJsonArray::toArray() {
	return this;
}
const QJsonArray *QJsonArray::toArray() const {
	return this;
}
void QJsonArray::swap(QJsonArray &other) {
	qSwap(values_, other.values_);
}
#endif