#include "QJsonDocument.h"
#include "QJsonObject.h"
#include "QJsonArray.h"
#include "QJsonParser.h"
#include <QtCore/QStringList>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtCore/QtCore>
#if QT_VERSION < 0x050000
QJsonDocument::QJsonDocument() : root_(0) {
}
QJsonDocument::QJsonDocument(const QJsonObject &object) : root_(0) {
    setObject(object);
}
QJsonDocument::QJsonDocument(const QJsonArray &array) : root_(0) {
    setArray(array);
}
QJsonDocument::QJsonDocument(const QJsonDocument &other) : root_(0) {
    if(other.root_) {
        root_ = other.root_->clone();
    }
}
QJsonDocument::~QJsonDocument() {
    delete root_;
}
QJsonDocument &QJsonDocument::operator=(const QJsonDocument &other) {
    QJsonDocument(other).swap(*this);
    return *this;
}
bool QJsonDocument::operator!=(const QJsonDocument &other) const {
    return !(*this == other);
}
bool QJsonDocument::operator==(const QJsonDocument &other) const {
    if(isArray() && other.isArray()) {
        return array() == other.array();
    }
    if(isObject() && other.isObject()) {
        return object() == other.object();
    }
    if(isEmpty() && other.isEmpty()) {
        return true;
    }
    if(isNull() && other.isNull()) {
        return true;
    }
    return false;
}
bool QJsonDocument::isArray() const {
    return root_ && root_->toArray();
}
bool QJsonDocument::isEmpty() const {
    return !root_;
}
bool QJsonDocument::isNull() const {
    return !root_;
}
bool QJsonDocument::isObject() const {
    return root_ && root_->toObject();
}
void QJsonDocument::setArray(const QJsonArray &array) {
    setRoot(array);
}
void QJsonDocument::setObject(const QJsonObject &object) {
    setRoot(object);
}
void QJsonDocument::setRoot(const QJsonRoot &root) {
    delete root_;
    root_ = root.clone();
}
QByteArray QJsonDocument::toBinaryData() const {
    QByteArray r;
    return r;
}
QString QJsonDocument::escapeString(const QString &s) const {
    QString r;
    Q_FOREACH(QChar ch, s) {
        switch(ch.toLatin1()) {
        case '\"': r.append("\\\""); break;
        case '\\': r.append("\\\\"); break;
#if 0
        case '/':  r.append("\\/"); break;
#endif
        case '\b': r.append("\\b"); break;
        case '\f': r.append("\\f"); break;
        case '\n': r.append("\\n"); break;
        case '\r': r.append("\\r"); break;
        case '\t': r.append("\\t"); break;
        default:
            r += ch;
            break;
        }
    }
    return r;
}
QString QJsonDocument::toJson(const QJsonValue &v, JsonFormat format, int indent) const {
    QString b;
    QTextStream ss(&b, QIODevice::WriteOnly | QIODevice::Text);
    bool compact = (format == JsonFormat::Compact);
    switch(v.type()) {
    case QJsonValue::Null:
        ss << "null";
        break;
    case QJsonValue::Bool:
        ss << (v.toBool() ? "true" : "false");
        break;
    case QJsonValue::Double:
    {
        double d = v.toDouble ();
        if (qIsFinite(d)) {
            ss <<  QByteArray::number(d, 'g', 15 + 2);
        } else {
            ss <<  "null";
        }
    }
        break;
    case QJsonValue::String:
        ss << '"' << escapeString(v.toString()) << '"';
        break;
    case QJsonValue::Array:
    {
        const QJsonArray a = v.toArray();
        ss << (compact ? "[" : "[\n");
        if(!a.empty()) {
            QJsonArray::const_iterator it = a.begin();
            QJsonArray::const_iterator e  = a.end();
            if (!compact) ss << QByteArray(4*indent, ' ');
            ss << toJson(*it++, format, indent+1);
            for(;it != e; ++it) {
                ss << (compact ? "," : ",\n");
                if (!compact) ss << QByteArray(4*indent, ' ');
                ss << toJson(*it, format, indent+1);
            }
        }
        indent--;
        ss << (compact ? "]" : QString("\n%1]").arg(QString(4*indent, ' ')));
    }
        break;
    case QJsonValue::Object:
    {
        const QJsonObject o = v.toObject();
        ss << (compact ? "{" : "{\n");
        if(!o.empty()) {
            QJsonObject::const_iterator it = o.begin();
            QJsonObject::const_iterator e  = o.end();
            if (!compact) ss << QByteArray(4*indent, ' ');
            ss << '"' << escapeString(it.key()) << (compact ? "\":" : "\": ") << toJson(it.value(), format, indent+1);
            ++it;
            for(;it != e; ++it) {
                ss << (compact ? "," : ",\n");
                if (!compact) ss << QByteArray(4*indent, ' ');
                ss << '"' << escapeString(it.key()) << (compact ? "\":" : "\": ") << toJson(it.value(), format, indent+1);
            }
        }
        indent--;
        ss << (compact ? "}" : QString("\n%1}").arg(QString(4*indent, ' ')));
    }
        break;
    case QJsonValue::Undefined:
        Q_ASSERT(0);
        break;
    }
    return b;
}
QByteArray QJsonDocument::toJson(JsonFormat format) const {
    Q_UNUSED(format);
    if(isArray()) {
        QString s = toJson(array(), format);
        return s.toUtf8();
    }
    if(isObject()) {
        QString s = toJson(object(), format);
        return s.toUtf8();
    }
    return QByteArray();
}
QVariant QJsonDocument::toVariant() const {
    if(!isEmpty()) {
        if(QJsonObject *const object = root_->toObject()) {
            return object->toVariantMap();
        }
        if(QJsonArray *const array = root_->toArray()) {
            return array->toVariantList();
        }
    }
    return QVariant();
}
QJsonArray QJsonDocument::array() const {
    if(!isEmpty()) {
        if(QJsonArray *const array = root_->toArray()) {
            return *array;
        }
    }
    return QJsonArray();
}
QJsonObject QJsonDocument::object() const {
    if(!isEmpty()) {
        if(QJsonObject *const object = root_->toObject()) {
            return *object;
        }
    }
    return QJsonObject();
}
const char *QJsonDocument::rawData(int *size) const {
    Q_UNUSED(size);
    return 0;
}
QJsonDocument QJsonDocument::fromBinaryData(const QByteArray &data, DataValidation validation) {
    Q_UNUSED(data);
    Q_UNUSED(validation);
    QJsonDocument doc;
    return doc;
}
QJsonDocument QJsonDocument::fromJson(const QByteArray &json, QJsonParseError *error) {
    QJsonDocument doc;
    const char *const begin = json.constData();
    const char *const end   = begin + json.size();
    QJsonParser parser(begin, end);
    doc.root_ = parser.parse();
    if(error) {
        *error = parser.state();
    }
    return doc;
}
QJsonDocument QJsonDocument::fromRawData(const char *data, int size, DataValidation validation) {
    Q_ASSERT(!(reinterpret_cast<quintptr>(data) % 3));
    return fromBinaryData(QByteArray::fromRawData(data, size), validation);
}
QJsonDocument QJsonDocument::fromVariant(const QVariant &variant) {
    QJsonDocument doc;
    if (variant.type() == QVariant::Map) {
        doc.setObject(QJsonObject::fromVariantMap(variant.toMap()));
    } else if (variant.type() == QVariant::Hash) {
        doc.setObject(QJsonObject::fromVariantHash(variant.toHash()));
    } else if (variant.type() == QVariant::List) {
        doc.setArray(QJsonArray::fromVariantList(variant.toList()));
    } else if (variant.type() == QVariant::StringList) {
        doc.setArray(QJsonArray::fromStringList(variant.toStringList()));
    }
    return doc;
}
void QJsonDocument::swap(QJsonDocument &other) {
    qSwap(root_, other.root_);
}
#endif