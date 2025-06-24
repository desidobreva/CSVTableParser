#pragma once
#include "Cell.h"

class TextCell : public Cell
{
public:
	TextCell(const char* data);
	TextCell(const TextCell& other);
	TextCell(TextCell&& other) noexcept;
	TextCell& operator=(const TextCell& other);
	TextCell& operator=(TextCell&& other) noexcept;

	const char* getData() const;
	virtual std::string convertToCSV() const override;

	virtual Cell* clone() const override;
	virtual ~TextCell() noexcept override;
private:
	char* data;
};
