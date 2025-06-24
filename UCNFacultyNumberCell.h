#pragma once
#include "Cell.h"

class UCNFacultyNumberCell : public Cell
{
public:
	virtual std::string convertToCSV() const override;
	const char* getData() const;
protected:
	char num[11];
};