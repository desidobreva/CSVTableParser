#pragma once
#include "Cell.h"

class NumberCell : public Cell
{
public:
	NumberCell(double num);

	double getNumber() const;
	virtual Cell* clone() const override;
	virtual std::string convertToCSV() const override;
private:
	double num;
};
