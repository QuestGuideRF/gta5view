#include "QJsonParseError.h"
#if QT_VERSION < 0x050000
QString QJsonParseError::errorString() const {
	switch(error) {
	case NoError:
		return "Ошибок не произошло";
	case UnterminatedObject:
		return "незавершенный объект";
	case MissingNameSeparator:
		return "отсутствует разделитель имени";
	case UnterminatedArray:
		return "незавершенный массив";
	case MissingValueSeparator:
		return "разделитель пропущенных значений";
	case IllegalValue:
		return "незаконная стоимость";
	case TerminationByNumber:
		return "недействительное завершение по номеру";
	case IllegalNumber:
		return "незаконный номер";
	case IllegalEscapeSequence:
		return "недопустимая escape-последовательность";
	case IllegalUTF8String:
		return "неверная строка UTF8";
	case UnterminatedString:
		return "незавершенная строка";
	case MissingObject:
		return "объект отсутствует после запятой";
	case DeepNesting:
		return "слишком глубоко вложенный документ";
	case DocumentTooLarge:
		return "слишком большой документ";
	case GarbageAtEnd:
		return "мусор в конце документа";
	}
	return QString();
}
#endif