#include "UCNFacultyNumberCell.h"

const char* UCNFacultyNumberCell::getData() const
{
	return num;
}

std::string UCNFacultyNumberCell::convertToCSV() const
{
	return std::string(num);
}