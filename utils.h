#pragma once
#include <string>

namespace Validators 
{
    bool isUCN(const char* str);
    bool isFacultyNumber(const char* str);
    bool isCurrency(const char* str);
    bool isNumber(const char* str);
    bool isQuotedText(const char* str);
}

namespace HelperFunctions
{
    bool hasOnlyDigits(const char* str);
    bool isDigit(char cha);
    void parseToCurrency(const char* str, double& amount, std::string& currency);
    std::string addQuotes(const char* str);
    std::string removeQuotes(const char* str);
    std::string trimTrailingZeros(const std::string& str);
    bool isValidComparisonOperator(const char* op);
    bool hasCSVExtension(const std::string& filename);
}
