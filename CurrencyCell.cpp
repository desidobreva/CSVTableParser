#include "CurrencyCell.h"
#include <string>
#include "utils.h"
#pragma warning (disable: 4996)

CurrencyCell::CurrencyCell(double amount, const char* type) : amount(amount)
{
	if (!type || strlen(type) != 3)
	{
		throw std::invalid_argument("Bad currency!");
	}

	strcpy(this->type, type);
}

double CurrencyCell::getAmount() const
{
	return amount;
}

const char* CurrencyCell::getCurrency() const
{
	return type;
}

Cell* CurrencyCell::clone() const
{
	return new CurrencyCell(*this);
}

std::string CurrencyCell::convertToCSV() const
{
	std::string csvValue = HelperFunctions::trimTrailingZeros(std::to_string(amount));
	csvValue += type;
	return csvValue;
}