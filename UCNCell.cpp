#include "UCNCell.h"
#include "utils.h"
#pragma warning (disable: 4996)

UCNCell::UCNCell(const char* str)
{
	if (!Validators::isUCN(str))
	{
		throw std::invalid_argument("Bad UCN!");
	}

	strcpy(num, str);
}

Cell* UCNCell::clone() const
{
	return new UCNCell(*this);
}