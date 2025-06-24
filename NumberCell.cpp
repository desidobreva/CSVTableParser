#include "NumberCell.h"
#include <string>
#include "utils.h"

NumberCell::NumberCell(double num) : num(num)
{

}

double NumberCell::getNumber() const
{
	return num;
}

Cell* NumberCell::clone() const
{
	return new NumberCell(*this);
}

std::string NumberCell::convertToCSV() const
{
	return HelperFunctions::trimTrailingZeros(std::to_string(num));
}