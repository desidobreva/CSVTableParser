#include "TextCell.h"
#include "utils.h"
#pragma warning (disable: 4996)

TextCell::TextCell(const char* data) : data(nullptr)
{
	if (!data)
	{
		throw std::invalid_argument("Bad string!");
	}

	if (Validators::isQuotedText(data))
	{
		std::string noQuotes = HelperFunctions::removeQuotes(data);
		this->data = new char[noQuotes.length() + 1];
		strcpy(this->data, noQuotes.c_str());
	}
	else
	{
		this->data = new char[strlen(data) + 1];
		strcpy(this->data, data);
	}
}

TextCell::TextCell(const TextCell& other) : data(nullptr)
{
	data = new char[strlen(other.data) + 1];
	strcpy(data, other.data);
}

TextCell::TextCell(TextCell&& other) noexcept : data(other.data)
{
	other.data = nullptr;
}

TextCell& TextCell::operator=(const TextCell& other)
{
	if (this != &other)
	{
		char* newData = new char[strlen(other.data) + 1];
		strcpy(newData, other.data);

		delete[] data;
		data = newData;
	}

	return *this;
}

TextCell& TextCell::operator=(TextCell&& other) noexcept
{
	std::swap(data, other.data);

	return *this;
}

const char* TextCell::getData() const
{
	return data;
}

Cell* TextCell::clone() const
{
	return new TextCell(*this);
}

TextCell::~TextCell() noexcept
{
	delete[] data;
}

std::string TextCell::convertToCSV() const
{
	return HelperFunctions::addQuotes(data);
}

