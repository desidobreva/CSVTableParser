#pragma once
#include "UCNFacultyNumberCell.h"

class UCNCell : public UCNFacultyNumberCell
{
public:
	UCNCell(const char* str);

	virtual Cell* clone() const override;
};