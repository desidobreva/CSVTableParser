#pragma once
#include "Cell.h"

class CurrencyCell : public Cell
{
public:
	CurrencyCell(double amount, const char* type);

	double getAmount() const;
	const char* getCurrency() const;

	virtual Cell* clone() const override;
	virtual std::string convertToCSV() const override;
private:
	double amount;
	char type[4];
};