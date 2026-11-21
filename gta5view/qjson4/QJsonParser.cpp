#include "QJsonParser.h"
#include "QJsonArray.h"
#include "QJsonObject.h"
#include "QJsonValue.h"
#if QT_VERSION < 0x050000
#include <cctype>
#include <QScopedPointer>
#include <QVector>
namespace {
unsigned int to_hex(int ch) {
	static const int hexval[256] = {
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
	};
	if(static_cast<unsigned int>(ch) < 256) {
		return hexval[static_cast<unsigned int>(ch)];
	} else {
		return 0;
	}
}
}
QJsonParser::QJsonParser(const char *begin, const char *end) : begin_(begin), end_(end), p_(begin) {
	state_.error  = QJsonParseError::NoError;
	state_.offset = 0;
}
QJsonRoot *QJsonParser::parse() {
	if(begin_ == end_) {
		return 0;
	}
	QJsonRoot *ret = 0;
	try {
		const char ch = peek();
		switch(ch) {
		case ArrayBegin:
			ret = getArray();
			break;
		case ObjectBegin:
			ret = getObject();
			break;
		default:
			state_.error  = QJsonParseError::IllegalValue;
			state_.offset = p_ - begin_;
			break;
		}
	} catch(const QJsonParseError &e) {
		state_ = e;
	}
	if(ret) {
		while(p_ != end_ && std::isspace(*p_)) {
			++p_;
		}
		if(p_ != end_) {
			state_.error  = QJsonParseError::GarbageAtEnd;
			state_.offset = p_ - begin_;
		}
	}
	return ret;
}
char QJsonParser::peek() {
	while(p_ != end_ && std::isspace(*p_)) {
		++p_;
	}
	return *p_;
}
QJsonValue QJsonParser::getValue() {
	switch(peek()) {
	case ObjectBegin:
		{
			QScopedPointer<QJsonObject> obj(getObject());
			return QJsonValue(*obj);
		}
	case ArrayBegin:
		{
			QScopedPointer<QJsonArray> arr(getArray());
			return QJsonValue(*arr);
		}
	case Quote:
		return QJsonValue(getString());
	case 't':
		return getTrue();
	case 'f':
		return getFalse();
	case 'n':
		return getNull();
	default:
		return getNumber();
	}
	throwError(QJsonParseError::MissingObject);
	return QJsonValue();
}
QJsonObject *QJsonParser::getObject() {
	QScopedPointer<QJsonObject> obj(new QJsonObject);
	char tok = peek();
	if(tok != ObjectBegin) {
		throwError(QJsonParseError::IllegalValue);
	}
	++p_;
	tok = peek();
	if(peek() == ObjectEnd) {
		++p_;
	} else {
		do {
			QPair<QString, QJsonValue> p = getPair();
			obj->values_.insert(p.first, p.second);
			tok = peek();
			++p_;
		} while(tok == ValueSeparator);
	}
	if(tok != ObjectEnd) {
		throwError(QJsonParseError::UnterminatedObject);
	}
	return obj.take();
}
QJsonArray *QJsonParser::getArray() {
	QScopedPointer<QJsonArray> arr(new QJsonArray);
	char tok = peek();
	if(tok != ArrayBegin) {
		throwError(QJsonParseError::IllegalValue);
	}
	++p_;
	tok = peek();
	if(tok == ArrayEnd) {
		++p_;
	} else {
		do {
			arr->values_.push_back(getValue());
			tok = peek();
			++p_;
		} while(tok == ValueSeparator);
	}
	if(tok != ArrayEnd) {
		throwError(QJsonParseError::MissingValueSeparator);
	}
	return arr.take();
}
QPair<QString, QJsonValue> QJsonParser::getPair() {
	QString key = getString();
	if(peek() != NameSeparator) {
		throwError(QJsonParseError::MissingNameSeparator);
	}
	++p_;
	return qMakePair(key, getValue());
}
QString QJsonParser::getString() {
	if(peek() != Quote) {
		throwError(QJsonParseError::IllegalUTF8String);
	}
	++p_;
	QByteArray s;
	while(p_ != end_ && *p_ != Quote && *p_ != '\n') {
		if(*p_ == '\\') {
			++p_;
			if(p_ != end_) {
				switch(*p_) {
				case '"':  s.append('"'); break;
				case '\\': s.append('\\'); break;
				case '/':  s.append('/'); break;
				case 'b':  s.append('\b'); break;
				case 'f':  s.append('\f'); break;
				case 'n':  s.append('\n'); break;
				case 'r':  s.append('\r'); break;
				case 't':  s.append('\t'); break;
				case 'u':
					{
						QString hexChar;
						char hex[4];
						if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[0] = *++p_;
						if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[1] = *++p_;
						if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[2] = *++p_;
						if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[3] = *++p_;
						if(!std::isxdigit(hex[0])) throwError(QJsonParseError::IllegalUTF8String);
						if(!std::isxdigit(hex[1])) throwError(QJsonParseError::IllegalUTF8String);
						if(!std::isxdigit(hex[2])) throwError(QJsonParseError::IllegalUTF8String);
						if(!std::isxdigit(hex[3])) throwError(QJsonParseError::IllegalUTF8String);
						quint16 w1 = 0;
						quint16 w2 = 0;
						w1 |= (to_hex(hex[0]) << 12);
						w1 |= (to_hex(hex[1]) << 8);
						w1 |= (to_hex(hex[2]) << 4);
						w1 |= (to_hex(hex[3]));
						hexChar.append(QChar(w1));
						if((w1 & 0xfc00) == 0xdc00) {
							throwError(QJsonParseError::IllegalUTF8String);
						}
						if((w1 & 0xfc00) == 0xd800) {
							if(p_ == end_ || *++p_ != '\\') { throwError(QJsonParseError::IllegalEscapeSequence); }
							if(p_ == end_ || *++p_ != 'u')  { throwError(QJsonParseError::IllegalEscapeSequence); }
							if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[0] = *++p_;
							if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[1] = *++p_;
							if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[2] = *++p_;
							if(p_ == end_) { throwError(QJsonParseError::IllegalEscapeSequence); } hex[3] = *++p_;
							if(!std::isxdigit(hex[0])) throwError(QJsonParseError::IllegalUTF8String);
							if(!std::isxdigit(hex[1])) throwError(QJsonParseError::IllegalUTF8String);
							if(!std::isxdigit(hex[2])) throwError(QJsonParseError::IllegalUTF8String);
							if(!std::isxdigit(hex[3])) throwError(QJsonParseError::IllegalUTF8String);
							w2 |= (to_hex(hex[0]) << 12);
							w2 |= (to_hex(hex[1]) << 8);
							w2 |= (to_hex(hex[2]) << 4);
							w2 |= (to_hex(hex[3]));
							hexChar.append(QChar(w2));
						}
						s.append(hexChar.toUtf8());
					}
					break;
				default:
					s.append('\\');
					break;
				}
			}
		} else {
			s.append(*p_);
		}
		++p_;
	}
	if(*p_ != Quote || p_ == end_) {
		throwError(QJsonParseError::UnterminatedString);
	}
	++p_;
	return QString::fromUtf8(s, s.size());
}
QJsonValue QJsonParser::getNull() {
	if(p_ == end_ || *p_++ != 'n') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'u') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'l') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'l') { throwError(QJsonParseError::IllegalValue); }
	return QJsonValue();
}
QJsonValue QJsonParser::getTrue() {
	if(p_ == end_ || *p_++ != 't') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'r') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'u') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'e') { throwError(QJsonParseError::IllegalValue); }
	return QJsonValue(true);
}
QJsonValue QJsonParser::getFalse() {
	if(p_ == end_ || *p_++ != 'f') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'a') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'l') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 's') { throwError(QJsonParseError::IllegalValue); }
	if(p_ == end_ || *p_++ != 'e') { throwError(QJsonParseError::IllegalValue); }
	return QJsonValue(false);
}
QJsonValue QJsonParser::getNumber() {
	const char *const first = p_;
	if(p_ != end_ && *p_ == '-') {
		++p_;
	}
	if(p_ != end_) {
		if(*p_ >= '1' && *p_ <= '9') {
			while(p_ != end_ && std::isdigit(*p_)) {
				++p_;
			}
		} else if(*p_ == '0') {
			++p_;
		} else {
			throwError(QJsonParseError::IllegalNumber);
		}
	}
	if(p_ != end_ && *p_ == '.') {
		++p_;
		if(!std::isdigit(*p_)) {
			throwError(QJsonParseError::IllegalNumber);
		}
		while(p_ != end_ && std::isdigit(*p_)) {
			++p_;
		}
	}
	if(p_ != end_ && (*p_ == 'e' || *p_ == 'E')) {
		++p_;
		if(p_ != end_ && (*p_ == '+' || *p_ == '-')) {
			++p_;
		}
		if(!std::isdigit(*p_)) {
			throwError(QJsonParseError::IllegalNumber);
		}
		while(p_ != end_ && std::isdigit(*p_)) {
			++p_;
		}
	}
	if(p_ == end_) {
		throwError(QJsonParseError::TerminationByNumber);
	}
	return QJsonValue(QByteArray::fromRawData(first, p_ - first).toDouble());
}
QJsonParseError QJsonParser::state() const {
	return state_;
}
void QJsonParser::throwError(QJsonParseError::ParseError e) {
	QJsonParseError err;
	err.error  = e;
	err.offset = p_ - begin_;
	throw err;
}
#endif