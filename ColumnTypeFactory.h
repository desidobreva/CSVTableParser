#pragma once

#include "ColumnType.h"
#include "utils.h"
#include <iostream>

class ColumnTypeFactory
{
public:
	static ColumnType* detectType(const char* str);
};