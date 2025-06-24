#include "ColumnTypeFactory.h"

ColumnType* ColumnTypeFactory::detectType(const char* str)
{
	if (!str)
	{
		throw std::invalid_argument("Bad cell in table!");
	}

    if (Validators::isQuotedText(str)) {
        return new TypeText();
    }

    if (Validators::isUCN(str)) {
        return new TypeUCN();
    }

    if (Validators::isFacultyNumber(str)) {
        return new TypeFacultyNumber();
    }

    if (Validators::isCurrency(str)) {
        return new TypeCurrency();
    }

    if (Validators::isNumber(str)) {
        return new TypeNumber();
    }

    return new TypeText();
}