#include "FacultyNumberCell.h"
#include "utils.h"
#pragma warning (disable: 4996)

FacultyNumberCell::FacultyNumberCell(const char* str)
{
	if (!Validators::isFacultyNumber(str))
	{
		throw std::invalid_argument("Bad faculty number!");
	}

	strcpy(num, str);
}

Cell* FacultyNumberCell::clone() const
{
	return new FacultyNumberCell(*this);
}