#include "ColumnType.h"
#include "TextCell.h"
#include "NumberCell.h"
#include "CurrencyCell.h"
#include "FacultyNumberCell.h"
#include "UCNCell.h"
#include "utils.h"

ColumnType* TypeText::clone() const
{
	return new TypeText(*this);
}

bool TypeText::validate(const char* str) const
{
	return true;
}

Cell* TypeText::createCell(const char* str) const
{
	if (!validate(str))
	{
		throw std::invalid_argument("Mismatch type!");
	}

	return new TextCell(str);
}

int TypeText::compare(const Cell* a, const Cell* b) const
{
	auto aa = dynamic_cast<const TextCell*>(a);
	auto bb = dynamic_cast<const TextCell*>(b);
	if (!aa || !bb) throw std::runtime_error("Type mismatch in compare");

	return strcmp(aa->getData(), bb->getData());
}

bool TypeNumber::validate(const char* str) const
{
	return Validators::isNumber(str);
}

Cell* TypeNumber::createCell(const char* str) const
{
	if (!validate(str))
	{
		throw std::invalid_argument("Mismatch type!");
	}

	return new NumberCell(std::stod(str));
}

ColumnType* TypeNumber::clone() const
{
	return new TypeNumber(*this);
}

int TypeNumber::compare(const Cell* a, const Cell* b) const
{
	auto aa = dynamic_cast<const NumberCell*>(a);
	auto bb = dynamic_cast<const NumberCell*>(b);
	if (!aa || !bb) throw std::runtime_error("Type mismatch in compare");

	double x = aa->getNumber();
	double y = bb->getNumber();

	if (x < y) return -1;
	if (x > y) return 1;
	return 0;
}

ColumnType* TypeCurrency::clone() const
{
	return new TypeCurrency(*this);
}

bool TypeCurrency::validate(const char* str) const
{
	return Validators::isCurrency(str);
}

Cell* TypeCurrency::createCell(const char* str) const
{
	if (!validate(str))
	{
		throw std::invalid_argument("Mismatch type!");
	}

	double amount = 0.0;
	std::string currency;

	HelperFunctions::parseToCurrency(str, amount, currency);
	return new CurrencyCell(amount, currency.c_str());
}

int TypeCurrency::compare(const Cell* a, const Cell* b) const
{
	auto aa = dynamic_cast<const CurrencyCell*>(a);
	auto bb = dynamic_cast<const CurrencyCell*>(b);
	if (!aa || !bb) throw std::runtime_error("Type mismatch in compare");

	double x = aa->getAmount();
	double y = bb->getAmount();

	if (x < y) return -1;
	if (x > y) return 1;

	// Сравняваме по валута, ако сумата е еднаква
	return strcmp(aa->getCurrency(), bb->getCurrency());
}

ColumnType* TypeUCN::clone() const
{
	return new TypeUCN(*this);
}

bool TypeUCN::validate(const char* str) const
{
	return Validators::isUCN(str);
}

Cell* TypeUCN::createCell(const char* str) const
{
	if (!validate(str))
	{
		throw std::invalid_argument("Mismatch type!");
	}

	return new UCNCell(str);
}

int TypeUCN::compare(const Cell* a, const Cell* b) const
{
	auto aa = dynamic_cast<const UCNCell*>(a);
	auto bb = dynamic_cast<const UCNCell*>(b);
	if (!aa || !bb) throw std::runtime_error("Type mismatch in compare");

	return strcmp(aa->getData(), bb->getData());
}

ColumnType* TypeFacultyNumber::clone() const
{
	return new TypeFacultyNumber(*this);
}

bool TypeFacultyNumber::validate(const char* str) const
{
	return Validators::isFacultyNumber(str);
}

Cell* TypeFacultyNumber::createCell(const char* str) const
{
	if (!validate(str))
	{
		throw std::invalid_argument("Mismatch type!");
	}

	return new FacultyNumberCell(str);
}

int TypeFacultyNumber::compare(const Cell* a, const Cell* b) const
{
	auto aa = dynamic_cast<const FacultyNumberCell*>(a);
	auto bb = dynamic_cast<const FacultyNumberCell*>(b);
	if (!aa || !bb) throw std::runtime_error("Type mismatch in compare");

	const char* x = aa->getData();
	const char* y = bb->getData();

	bool isOldX = strlen(x) == 5;
	bool isOldY = strlen(y) == 5;

	int specX = isOldX ? x[0] - '0' : x[4] - '0';
	int specY = isOldY ? y[0] - '0' : y[4] - '0';

	if (specX != specY)
		return (specX < specY) ? -1 : 1;

	if (isOldX != isOldY)
		return isOldX ? -1 : 1;

	return strcmp(x, y);
}
