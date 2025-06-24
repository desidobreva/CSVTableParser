#include <iostream>
#include <string>
#include "utils.h"
#pragma warning (disable: 4996)

bool Validators::isUCN(const char* str)
{
	if (!str || strlen(str) != 10)
	{
		return false;
	}

	if (!HelperFunctions::hasOnlyDigits(str))
	{
		return false;
	}

	int year = (str[0] - '0') * 10 + (str[1] - '0');
	int month = (str[2] - '0') * 10 + (str[3] - '0');
	int day = (str[4] - '0') * 10 + (str[5] - '0');

	if (month >= 1 && month <= 12)
		year += 1900;
	else if (month >= 21 && month <= 32) {
		year += 1800;
		month -= 20;
	}
	else if (month >= 41 && month <= 52) {
		year += 2000;
		month -= 40;
	}
	else
		return false; 

	if (month < 1 || month > 12 || day < 1)
		return false;

	int daysInMonth[] = { 31, 28, 31, 30, 31, 30,
						  31, 31, 30, 31, 30, 31 };

	bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
	if (month == 2 && isLeap)
		daysInMonth[1] = 29;

	if (day > daysInMonth[month - 1])
		return false;

	return true;
}

bool Validators::isFacultyNumber(const char* str)
{
	if (!str)
		return false;

	size_t len = strlen(str);

	if (len == 5)
	{
		if (!HelperFunctions::hasOnlyDigits(str))
			return false;

		// Първата цифра трябва да е 4, 6, 7 или 8
		char firstDigit = str[0];
		return firstDigit == '4' || firstDigit == '6' || firstDigit == '7' || firstDigit == '8';
	}

	if (len == 10)
	{
		// Формат: 0MI0400123

		if (str[1] != 'M' || str[2] != 'I') return false;

		// Контролна цифра
		if (str[4] != '4' && str[4] != '6' && str[4] != '7' && str[4] != '8') 
			return false;

		if (str[5] != '0' || str[6] != '0') 
			return false;

		for (int i = 7; i < 10; i++)
		{
			if (!HelperFunctions::isDigit(str[i])) 
				return false;
		}

		return true;
	}

	return false;
}

bool Validators::isCurrency(const char* str) {
	if (!str) return false;

	size_t len = strlen(str);
	if (len < 4) return false;

	int i = (int)len - 1;
	int letterCount = 0;

	while (i >= 0 && letterCount < 3) {
		if (isalpha(str[i])) {
			++letterCount;
			--i;
		}
		else {
			break;
		}
	}

	if (letterCount != 3) return false;

	char numberPart[64] = {};
	size_t numberLen = i + 1;

	while (numberLen > 0 && str[numberLen - 1] == ' ') {
		--numberLen;
	}

	if (numberLen == 0 || numberLen >= sizeof(numberPart)) return false;

	strncpy(numberPart, str, numberLen);
	numberPart[numberLen] = '\0';

	bool hasDigit = false;
	bool hasDot = false;
	for (size_t j = 0; numberPart[j]; ++j) {
		if (isdigit(numberPart[j])) {
			hasDigit = true;
		}
		else if (numberPart[j] == '.') {
			if (hasDot) return false;
			hasDot = true;
		}
		else if (j == 0 && (numberPart[j] == '-' || numberPart[j] == '+')) {
			continue;
		}
		else {
			return false;
		}
	}

	return hasDigit;
}

bool Validators::isNumber(const char* str) {
	if (!str || !*str) return false;

	size_t i = 0;
	if (str[i] == '+' || str[i] == '-') ++i;

	bool hasDigits = false;
	bool hasDot = false;

	for (; str[i]; ++i) {
		if (isdigit(str[i])) {
			hasDigits = true;
		}
		else if (str[i] == '.') {
			if (hasDot) return false;
			hasDot = true;
		}
		else {
			return false;
		}
	}

	return hasDigits;
}

bool Validators::isQuotedText(const char* str) {
	if (!str || strlen(str) < 2) return false;

	char first = str[0];
	char last = str[strlen(str) - 1];

	return ((first == '\'' || first == '\"') && first == last);
}

bool HelperFunctions::hasOnlyDigits(const char* str)
{
	if (!str)
	{
		return false;
	}

	while (*str)
	{
		char ch = *str;
		if (!isDigit(ch))
		{
			return false;
		}
		str++;
	}

	return true;
}

bool HelperFunctions::isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

void HelperFunctions::parseToCurrency(const char* str, double& amount, std::string& currency)
{
	std::string s(str);
	size_t i = 0;

	while (i < s.length() && (std::isdigit(s[i]) || s[i] == '.' || s[i] == '-')) {
		i++;
	}
	amount = std::stod(s.substr(0, i));

	while (i < s.length() && s[i] == ' ') ++i;
	currency = s.substr(i);
}

std::string HelperFunctions::addQuotes(const char* str)
{
	if (!str)
	{
		throw std::invalid_argument("Bad string!");
	}

	return std::string("\"") + str + "\"";
}

std::string HelperFunctions::removeQuotes(const char* str)
{
	if (!str)
	{
		throw std::invalid_argument("Bad string!");
	}

	size_t len = strlen(str);
	if (len >= 2 && (str[0] == '"' || str[0] == '\'') && str[len - 1] == str[0]) {
		return std::string(str + 1, len - 2);
	}
	return str;
}

std::string HelperFunctions::trimTrailingZeros(const std::string& str) {
	size_t dotPos = str.find('.');
	if (dotPos == std::string::npos) return str;

	size_t end = str.size() - 1;

	while (end > dotPos && str[end] == '0') {
		--end;
	}

	if (end == dotPos) --end;

	return str.substr(0, end + 1);
}

bool HelperFunctions::isValidComparisonOperator(const char* op)
{
	if (!op) return false;

	return strcmp(op, "==") == 0 ||
		strcmp(op, "!=") == 0 ||
		strcmp(op, "<") == 0 ||
		strcmp(op, "<=") == 0 ||
		strcmp(op, ">") == 0 ||
		strcmp(op, ">=") == 0;
}

bool HelperFunctions::hasCSVExtension(const std::string& filename)
{
	if (filename.length() < 4)
		return false;

	std::string ext = filename.substr(filename.length() - 4);

	return ext == ".csv" || ext == ".CSV";
}