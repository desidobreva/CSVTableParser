#pragma once
#include <iostream>

class Cell
{
public:
	virtual Cell* clone() const = 0;
	virtual std::string convertToCSV() const = 0;
	virtual ~Cell() noexcept = default;
};