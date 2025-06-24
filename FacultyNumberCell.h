#pragma once
#include "UCNFacultyNumberCell.h"

class FacultyNumberCell : public UCNFacultyNumberCell
{
public:
	FacultyNumberCell(const char* str);

	virtual Cell* clone() const override;
};
