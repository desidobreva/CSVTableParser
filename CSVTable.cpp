#include "CSVTable.h"
#include "ColumnTypeFactory.h"
#include <string>
#include <cassert>
#pragma warning (disable: 4996)

size_t CSVTable::getCellsInRowCount(const char* row, char separator)
{
	if (!row)
	{
		return 0;
	}

	size_t count = 1;
	bool inQuotes = false;

	for (const char* ptr = row; *ptr; ptr++)
	{
		if (*ptr == '"' || *ptr == '\'')
		{
			inQuotes = !inQuotes;
		}
		else if (*ptr == separator && !inQuotes)
		{
			count++;
		}
	}

	return count;
}

//тази функция е взета от ChatGPT
size_t CSVTable::splitLine(const char* line, char separator, char** tokens, size_t maxTokens) 
{
	size_t count = 0;
	const char* ptr = line;

	while (*ptr && count < maxTokens) {
		std::string token;
		bool inQuotes = false;
		char quoteChar = '\0';

		while (*ptr == ' ') 
			++ptr;

		if (*ptr == '\'' || *ptr == '"') {
			inQuotes = true;
			quoteChar = *ptr;
			token += *ptr; 
			++ptr;

			while (*ptr) 
			{
				if (*ptr == quoteChar && *(ptr + 1) == quoteChar) 
				{
					token += quoteChar;
					token += quoteChar;
					ptr += 2;
				}
				else if (*ptr == quoteChar) 
				{
					token += *ptr; 
					++ptr;
					break;
				}
				else {
					token += *ptr;
					++ptr;
				}
			}
		}

		while (*ptr && (!inQuotes && *ptr != separator)) 
		{
			token += *ptr;
			++ptr;
		}

		if (*ptr == separator) 
			++ptr;

		char* result = new char[token.length() + 1];
		std::strcpy(result, token.c_str());
		tokens[count++] = result;
	}

	return count;
}

CSVTable::CSVTable() : columns(nullptr), columnsCount(0), rowsCount(0)
{

}

void CSVTable::free()
{
	for (size_t i = 0; i < columnsCount; i++)
	{
		delete columns[i];
	}

	delete[] columns;
	columns = nullptr;

	if (previousState)
	{
		delete previousState;
		previousState = nullptr;
	}
}

void CSVTable::nullify()
{
	for (int i = 0; i < columnsCount; i++)
	{
		columns[i] = nullptr;
	}
}

void CSVTable::clearColumns()
{
	for (size_t i = 0; i < columnsCount; ++i)
		delete columns[i];

	delete[] columns;
	columns = nullptr;
	columnsCount = 0;
	rowsCount = 0;
}

CSVTable::CSVTable(const CSVTable& other)
	: columns(nullptr), columnsCount(other.columnsCount), rowsCount(other.rowsCount), 
	separator(other.separator), hasTitleRow(other.hasTitleRow)
{
	columns = new Column * [columnsCount];

	for (int i = 0; i < columnsCount; i++)
	{
		columns[i] = other.columns[i]->clone();
	}
}

CSVTable::CSVTable(CSVTable&& other) noexcept
	: columns(nullptr), columnsCount(other.columnsCount), rowsCount(other.rowsCount),
	separator(other.separator), hasTitleRow(other.hasTitleRow)
{
	columns = other.columns;
	other.columns = nullptr;
}

CSVTable& CSVTable::operator=(const CSVTable& other)
{
	if (this != &other)
	{
		CSVTable copy(other);
		*this = std::move(copy);
	}

	return *this;
}

CSVTable& CSVTable::operator=(CSVTable&& other) noexcept
{
	swap(other);
	return *this;
}

bool CSVTable::isEmpty() const
{
	return columnsCount == 0 || rowsCount == 0;
}

char CSVTable::getSeparator() const
{
	return separator;
}

void CSVTable::swap(CSVTable& other)
{
	std::swap(columns, other.columns);
	std::swap(columnsCount, other.columnsCount);
	std::swap(rowsCount, other.rowsCount);
	std::swap(separator, other.separator);
	std::swap(hasTitleRow, other.hasTitleRow);
	std::swap(previousState, other.previousState);
}

CSVTable::~CSVTable() noexcept
{
	free();
}

CSVTable* CSVTable::clone() const
{
	return new CSVTable(*this);
}

void CSVTable::setSeparator(char ch)
{
	separator = ch;
}

size_t CSVTable::getColumnsCount() const
{
	return columnsCount;
}

size_t CSVTable::getRowsCount() const
{
	return rowsCount;
}

bool CSVTable::safeCloneToPreviousState()
{
	try {
		delete previousState;
		previousState = this->clone();
		return true;
	}
	catch (...) {
		return false;
	}
}

bool CSVTable::loadFromFile(std::ifstream& ifs, bool hasTitleRow) {
	if (!ifs.is_open()) return false;

	std::string line;
	if (!getline(ifs, line)) 
		return false;

	size_t columnsCount = getCellsInRowCount(line.c_str(), separator);
	char** tokens = new char* [columnsCount];
	for (size_t i = 0; i < columnsCount; ++i) 
		tokens[i] = nullptr;

	size_t actualCount = splitLine(line.c_str(), separator, tokens, columnsCount);
	if (actualCount != columnsCount) {
		for (size_t i = 0; i < columnsCount; ++i) 
			delete[] tokens[i];
		delete[] tokens;
		throw std::runtime_error("Malformed first row.");
	}

	Column** tempColumns = new (std::nothrow) Column * [columnsCount] {};
	if (!tempColumns)
	{
		for (size_t i = 0; i < columnsCount; ++i)
			delete[] tokens[i];
		delete[] tokens;
		throw std::bad_alloc();
	}

	for (size_t i = 0; i < columnsCount; ++i)
	{
		tempColumns[i] = nullptr;
	}

	try
	{
		if (hasTitleRow) {
			// Създаване на колони по заглавие
			for (size_t i = 0; i < columnsCount; ++i) {
				tempColumns[i] = new Column(tokens[i], nullptr);
				delete[] tokens[i];
				tokens[i] = nullptr;
			}
			delete[] tokens;
			tokens = nullptr;

			if (!std::getline(ifs, line)) {
				throw std::runtime_error("Missing data after title row.");
			}

			tokens = new char* [columnsCount] {};
			actualCount = splitLine(line.c_str(), separator, tokens, columnsCount);
			if (actualCount != columnsCount) {
				throw std::runtime_error("Malformed first data row.");
			}
		}
		else {
			// Няма заглавен ред — откриване на тип по първи ред
			for (size_t i = 0; i < columnsCount; ++i) {
				ColumnType* type = ColumnTypeFactory::detectType(tokens[i]);
				tempColumns[i] = new Column(nullptr, type);
			}
		}

		for (size_t i = 0; i < columnsCount; ++i) {
			ColumnType* type = tempColumns[i]->getType();
			if (!type)
			{
				type = ColumnTypeFactory::detectType(tokens[i]);
				tempColumns[i]->setType(type);
			}
			if (!type->validate(tokens[i]))
			{
				throw std::runtime_error("Invalid value for column type.");
			}

			Cell* cell = type->createCell(tokens[i]);
			tempColumns[i]->addCell(cell);

			delete[] tokens[i];
			tokens[i] = nullptr;
		}
		delete[] tokens;
		tokens = nullptr;
	}
	catch (...)
	{
		for (size_t i = 0; i < columnsCount; ++i) {
			delete tempColumns[i];
		}
		delete[] tempColumns;

		if (tokens) {
			for (size_t i = 0; i < columnsCount; ++i)
				delete[] tokens[i];
			delete[] tokens;
		}

		throw;
	}

	size_t rowsCount = 1;
	try {
		while (loadRowFromFile(ifs, columnsCount, tempColumns)) 
		{
			rowsCount++;
		}
	}
	catch (...) {
		for (size_t i = 0; i < columnsCount; ++i) {
			if (tempColumns[i]) {
				delete tempColumns[i];
			}
		}
		delete[] tempColumns;
		throw;
	}

	free();
	this->columns = tempColumns;
	this->columnsCount = columnsCount;
	this->rowsCount = rowsCount;
	this->hasTitleRow = hasTitleRow;

	return true;
}

bool CSVTable::loadRowFromFile(std::ifstream& ifs, size_t currColumnsCount, Column** cols) 
{
	std::string line;
	if (!std::getline(ifs, line)) return false;

	char** tokens = new char* [currColumnsCount];
	for (size_t i = 0; i < currColumnsCount; ++i) 
		tokens[i] = nullptr;

	size_t actualCount = splitLine(line.c_str(), separator, tokens, currColumnsCount);
	if (actualCount != currColumnsCount) {
		for (size_t i = 0; i < currColumnsCount; ++i) 
			delete[] tokens[i];
		delete[] tokens;
		throw std::runtime_error("Malformed row.");
	}

	Cell** cells = new Cell * [currColumnsCount];
	for (size_t i = 0; i < currColumnsCount; ++i) 
		cells[i] = nullptr;

	try {
		for (size_t i = 0; i < currColumnsCount; ++i) {
			ColumnType* type = cols[i]->getType();
			if (!type->validate(tokens[i])) {
				throw std::runtime_error("Type mismatch in row.");
			}
			cells[i] = type->createCell(tokens[i]);
		}

		for (size_t i = 0; i < currColumnsCount; ++i) {
			cols[i]->addCell(cells[i]);
		}
	}
	catch (...) {
		for (size_t i = 0; i < currColumnsCount; ++i) {
			delete cells[i]; 
		}
		delete[] cells;

		for (size_t i = 0; i < currColumnsCount; ++i) {
			delete[] tokens[i];
		}
		delete[] tokens;

		throw;
	}

	delete[] cells;

	for (size_t i = 0; i < currColumnsCount; ++i) {
		delete[] tokens[i];
	}
	delete[] tokens;

	return true;
}

bool CSVTable::print(std::ostream& out) const
{
	if (hasTitleRow)
	{
		for (int i = 0; i < columnsCount; i++)
		{
			out << columns[i]->getName();
			if (i != columnsCount - 1)
			{
				out << separator << ' ';
			}
			else
			{
				out << '\n';
			}
		}
	}

	for (int i = 0; i < rowsCount; i++)
	{
		for (int j = 0; j < columnsCount; j++)
		{
			std::string csvValue = columns[j]->getCell(i)->convertToCSV();
			out << csvValue;
			if (j != columnsCount - 1)
			{
				out << separator << ' ';
			}
			else
			{
				out << '\n';
			}
		}
	}

	return out.good();
}

bool CSVTable::sortByColumn(size_t columnIndex, bool ascending)
{
	if (columnIndex >= columnsCount)
		return false;

	Column* targetColumn = columns[columnIndex];
	ColumnType* type = targetColumn->getType();

	size_t* rowIndices = new (std::nothrow) size_t[rowsCount];
	if (!rowIndices)
	{
		return false;
	}

	for (size_t i = 0; i < rowsCount; ++i)
		rowIndices[i] = i;

	for (size_t i = 0; i < rowsCount - 1; ++i)
	{
		for (size_t j = i + 1; j < rowsCount; ++j)
		{
			Cell* a = targetColumn->getCell(rowIndices[i]);
			Cell* b = targetColumn->getCell(rowIndices[j]);
			int cmp = -1;

			try
			{
				cmp = type->compare(a, b);
			}
			catch (...)
			{
				delete[] rowIndices;
				return false;
			}

			bool shouldSwap = ascending ? (cmp > 0) : (cmp < 0);
			if (shouldSwap)
				std::swap(rowIndices[i], rowIndices[j]);
		}
	}

	if (!safeCloneToPreviousState())
	{
		delete[] rowIndices;
		return false;
	}

	// Разместваме редовете във ВСИЧКИ колони според новите индекси
	for (size_t col = 0; col < columnsCount; ++col)
	{
		Column* colPtr = columns[col];
		Cell** newCells = new (std::nothrow) Cell * [rowsCount];
		if(!newCells)
		{
			delete[] rowIndices;
			return false;
		}

		for (size_t i = 0; i < rowsCount; ++i)
		{
			newCells[i] = nullptr;
		}

		try
		{
			for (size_t i = 0; i < rowsCount; ++i)
			{
				newCells[i] = colPtr->getCell(rowIndices[i])->clone();
			}
		}
		catch (...)
		{
			for (size_t i = 0; i < rowsCount; ++i)
			{
				delete newCells[i];
			}
			delete[] newCells;
			delete[] rowIndices;

			return false;
		}

		for (size_t i = 0; i < rowsCount; ++i)
			colPtr->setCell(i, newCells[i]);

		delete[] newCells;
	}

	delete[] rowIndices;
	return true;
}

bool CSVTable::sortByColumn(const char* columnName, bool ascending)
{
	if (!hasTitleRow || !columnName || !columns || columnsCount == 0) 
		return false;

	for (size_t i = 0; i < columnsCount; ++i) {
		const char* name = columns[i]->getName();
		if (name && strcmp(name, columnName) == 0) {
			return sortByColumn(i, ascending);
		}
	}

	return false;
}

bool CSVTable::filterByColumn(size_t columnIndex, const char* op, const char* value)
{
	if (columnIndex >= columnsCount || !HelperFunctions::isValidComparisonOperator(op) || !value)
		return false;

	Column* col = columns[columnIndex];
	ColumnType* type = col->getType();

	if (!type->validate(value))
		return false;

	Cell* reference = nullptr;
	try
	{
		reference = type->createCell(value);
	}
	catch (...)
	{
		return false;
	}

	// Масив дали да запазим даден ред
	bool* keepRow = new (std::nothrow) bool[rowsCount];
	if (!keepRow) {
		delete reference;
		return false;
	}

	for (size_t i = 0; i < rowsCount; ++i) {
		Cell* cell = col->getCell(i);
		int cmp = -1;

		try
		{
			cmp = type->compare(cell, reference);
		}
		catch (...)
		{
			delete[] keepRow;
			delete reference;
			return false;
		}

		bool keep = false;
		if (strcmp(op, "==") == 0) keep = (cmp == 0);
		else if (strcmp(op, "!=") == 0) keep = (cmp != 0);
		else if (strcmp(op, "<") == 0)  keep = (cmp < 0);
		else if (strcmp(op, "<=") == 0) keep = (cmp <= 0);
		else if (strcmp(op, ">") == 0)  keep = (cmp > 0);
		else if (strcmp(op, ">=") == 0) keep = (cmp >= 0);

		keepRow[i] = keep;
	}

	if (!safeCloneToPreviousState())
	{
		delete[] keepRow;
		delete reference;
		return false;
	}

	for (size_t colIndex = 0; colIndex < columnsCount; ++colIndex)
	{
		Column* current = columns[colIndex];
		Cell** newCells = new (std::nothrow) Cell * [rowsCount];
		if (!newCells) {
			delete[] keepRow;
			delete reference;
			return false;
		}
		size_t newCount = 0;

		try 
		{
			for (size_t i = 0; i < rowsCount; ++i) {
				if (keepRow[i]) {
					newCells[newCount++] = current->getCell(i)->clone();
				}
			}
		}
		catch (...) 
		{
			for (size_t j = 0; j < newCount; ++j)
				delete newCells[j];
			delete[] newCells;
			delete[] keepRow;
			delete reference;
			return false;
		}

		current->clearCells();
		for (size_t i = 0; i < newCount; ++i)
			current->addCell(newCells[i]);

		delete[] newCells;
	}

	size_t newRows = 0;
	for (size_t i = 0; i < rowsCount; ++i)
		if (keepRow[i]) ++newRows;

	rowsCount = newRows;

	delete[] keepRow;
	delete reference;
	return true;
}

bool CSVTable::filterByColumn(const char* columnName, const char* op, const char* value)
{
	if (!hasTitleRow || !columnName || !HelperFunctions::isValidComparisonOperator(op) || !value)
		return false;

	for (size_t i = 0; i < columnsCount; ++i) {
		const char* name = columns[i]->getName();
		if (name && strcmp(name, columnName) == 0) {
			return filterByColumn(i, op, value);
		}
	}

	return false;
}

bool CSVTable::removeDuplicateRows()
{
	if (rowsCount == 0 || columnsCount == 0)
		return false;

	bool* isDuplicate = new (std::nothrow) bool[rowsCount] {};
	if (!isDuplicate)
		return false;

	bool hasDuplicates = false;

	for (size_t i = 0; i < rowsCount; ++i) {
		if (isDuplicate[i]) 
			continue;

		for (size_t j = i + 1; j < rowsCount; ++j) 
		{
			bool equal = true;
			for (size_t col = 0; col < columnsCount && equal; ++col) 
			{
				try
				{
					if (columns[col]->getType()->compare(columns[col]->getCell(i), columns[col]->getCell(j)) != 0)
						equal = false;
				}
				catch (...)
				{
					delete[] isDuplicate;
					return false;
				}
			}
			if (equal) {
				isDuplicate[j] = true;
				hasDuplicates = true;
			}
		}
	}

	if (!hasDuplicates) {
		delete[] isDuplicate;
		return true;
	}

	Cell*** newColumnCells = new (std::nothrow) Cell** [columnsCount];
	if (!newColumnCells) {
		delete[] isDuplicate;
		return false;
	}

	size_t newRowCount = 0;
	for (size_t i = 0; i < rowsCount; ++i)
		if (!isDuplicate[i]) ++newRowCount;

	try {
		for (size_t col = 0; col < columnsCount; ++col) {
			newColumnCells[col] = new Cell * [newRowCount];
			size_t idx = 0;

			for (size_t row = 0; row < rowsCount; ++row) {
				if (!isDuplicate[row]) {
					newColumnCells[col][idx++] = columns[col]->getCell(row)->clone();
				}
			}
		}
	}
	catch (...) {
		for (size_t col = 0; col < columnsCount; ++col) {
			if (newColumnCells[col]) {
				for (size_t i = 0; i < newRowCount; ++i)
					delete newColumnCells[col][i];
				delete[] newColumnCells[col];
			}
		}
		delete[] newColumnCells;
		delete[] isDuplicate;
		return false;
	}

	if (!safeCloneToPreviousState())
	{
		delete[] newColumnCells;
		delete[] isDuplicate;
		return false;
	}

	for (size_t col = 0; col < columnsCount; ++col) {
		columns[col]->clearCells();
		for (size_t i = 0; i < newRowCount; ++i) {
			columns[col]->addCell(newColumnCells[col][i]);
		}
		delete[] newColumnCells[col];
	}
	delete[] newColumnCells;

	rowsCount = newRowCount;
	delete[] isDuplicate;
	return true;
}

bool CSVTable::undo()
{
	if (!previousState)
		return false;

	Column** newColumns = new (std::nothrow) Column * [previousState->columnsCount];
	if (!newColumns)
	{
		return false;
	}

	for (size_t i = 0; i < previousState->columnsCount; ++i)
		newColumns[i] = nullptr;

	try
	{
		for (size_t i = 0; i < previousState->columnsCount; ++i)
			newColumns[i] = previousState->columns[i]->clone();
	}
	catch (...)
	{
		if (newColumns)
		{
			for (size_t i = 0; i < previousState->columnsCount; ++i)
				delete newColumns[i];
			delete[] newColumns;
		}
		return false;
	}

	clearColumns();

	columns = newColumns;
	columnsCount = previousState->columnsCount;
	rowsCount = previousState->rowsCount;
	separator = previousState->separator;
	hasTitleRow = previousState->hasTitleRow;

	delete previousState;
	previousState = nullptr;

	return true;
}

bool CSVTable::renameColumn(size_t index, const char* newName)
{
	if (index >= columnsCount || !newName)
		return false;

	delete previousState;
	previousState = this->clone();

	return columns[index]->setName(newName);
}

const char* CSVTable::getColumnName(size_t index) const
{
	if (index >= columnsCount)
		return nullptr;

	return columns[index]->getName();
}

int CSVTable::getColumnIndexByName(const char* name) const
{
	if (!name || !hasTitleRow) return -1;

	for (size_t i = 0; i < columnsCount; ++i) {
		if (strcmp(columns[i]->getName(), name) == 0)
			return static_cast<int>(i);
	}

	return -1; 
}

bool CSVTable::setColumnNames(const char** newNames, size_t count)
{
	if (!newNames || count != columnsCount)
		return false;

	if (!safeCloneToPreviousState())
	{
		return false;
	}

	for (size_t i = 0; i < columnsCount; ++i)
		columns[i]->setName(newNames[i]);

	hasTitleRow = true;
	return true;
}

bool CSVTable::clearColumnNames()
{
	if (columnsCount == 0 || !hasTitleRow)
		return false;

	if (!safeCloneToPreviousState())
	{
		return false;
	}

	for (size_t i = 0; i < columnsCount; ++i)
		columns[i]->setName("");

	hasTitleRow = false;
	return true;
}

bool CSVTable::clearColumnName(size_t index)
{
	if (index >= columnsCount || !hasTitleRow)
		return false;

	if (!safeCloneToPreviousState())
	{
		return false;
	}

	return columns[index]->setName("");
}

bool CSVTable::removeColumn(size_t columnIndex)
{
	if (columnIndex >= columnsCount)
		return false;

	if (!safeCloneToPreviousState())
		return false;

	delete columns[columnIndex];

	for (size_t i = columnIndex + 1; i < columnsCount; ++i)
		columns[i - 1] = columns[i];

	--columnsCount;

	return true;
}

bool CSVTable::removeColumn(const char* columnName)
{
	if (!columnName)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
	{
		const char* name = columns[i]->getName();
		if (name && strcmp(name, columnName) == 0)
		{
			return removeColumn(i); 
		}
	}
	return false;
}

bool CSVTable::addColumn(Column* column)
{
	if (!column)
		return false;

	if (columnsCount > 0 && column->getSize() != rowsCount)
		return false;

	if (columnsCount == 0)
		rowsCount = column->getSize();

	Column** newColumns = new (std::nothrow) Column * [columnsCount + 1];
	if (!newColumns)
	{
		return false;
	}

	if (!safeCloneToPreviousState())
	{
		delete[] newColumns;
		return false;
	}

	for (size_t i = 0; i < columnsCount; ++i)
		newColumns[i] = columns[i];

	newColumns[columnsCount] = column; 
		
	delete[] columns;
	columns = newColumns;
	++columnsCount;

	return true;
}

bool CSVTable::addColumn(const Column* column)
{
	if (!column)
		return false;

	if (columnsCount > 0 && column->getSize() != rowsCount)
		return false;

	if (columnsCount == 0)
		rowsCount = column->getSize();

	Column** newColumns = new (std::nothrow) Column * [columnsCount + 1];
	if (!newColumns)
	{
		return false;
	}

	try
	{
		newColumns[columnsCount] = column->clone(); 
	}
	catch (...)
	{
		delete[] newColumns;
		return false;
	}

	if (!safeCloneToPreviousState())
	{
		delete[] newColumns;
		return false;
	}

	for (size_t i = 0; i < columnsCount; ++i)
		newColumns[i] = columns[i];

	delete[] columns;
	columns = newColumns;
	++columnsCount;

	return true;
}

bool CSVTable::duplicateColumn(size_t columnIndex)
{
	if (columnIndex >= columnsCount)
		return false;

	if (!safeCloneToPreviousState())
		return false;

	Column* original = columns[columnIndex];
	Column* copy = original->clone();
	if (!copy)
		return false;

	const char* baseName = original->getName();
	if (!baseName || baseName[0] == '\0') 
		baseName = "Column";

	size_t suffix = 1;
	std::string newName;
	bool unique = false;

	while (!unique)
	{
		newName = std::string(baseName) + "_" + std::to_string(suffix++);
		unique = true;
		for (size_t i = 0; i < columnsCount; ++i)
		{
			if (columns[i]->getName() && strcmp(columns[i]->getName(), newName.c_str()) == 0)
			{
				unique = false;
				break;
			}
		}
	}

	if (!copy->setName(newName.c_str()))
	{
		delete copy;
		return false;
	}

	return addColumn(copy);
}

bool CSVTable::duplicateColumn(const char* name)
{
	if (!hasTitleRow || !name)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
	{
		if (strcmp(columns[i]->getName(), name) == 0)
			return duplicateColumn(i);
	}
	return false; 
}

bool CSVTable::reorderColumns(const size_t* permutation, size_t size)
{
	if (!permutation || size != columnsCount)
		return false;

	bool* seen = new (std::nothrow) bool[columnsCount] {};
	if (!seen)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
	{
		seen[i] = false;
	}

	for (size_t i = 0; i < columnsCount; ++i)
	{
		if (permutation[i] >= columnsCount || seen[permutation[i]])
		{
			delete[] seen;
			return false; 
		}
		seen[permutation[i]] = true;
	}
	delete[] seen;

	if (!safeCloneToPreviousState())
		return false;

	Column** newColumns = new (std::nothrow) Column * [columnsCount];
	if (!newColumns)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
		newColumns[i] = columns[permutation[i]];

	delete[] columns;
	columns = newColumns;

	return true;
}

bool CSVTable::editCell(size_t colIndex, size_t rowIndex, const char* newValue)
{
	if (colIndex >= columnsCount || rowIndex >= rowsCount || !newValue)
		return false;

	Column* col = columns[colIndex];
	ColumnType* type = col->getType();

	if (!type->validate(newValue))
		return false;

	Cell* newCell = nullptr;
	try 
	{
		newCell = type->createCell(newValue);
	}
	catch (...) {
		return false;
	}

	if (!safeCloneToPreviousState()) {
		delete newCell;
		return false;
	}

	Cell* oldCell = col->getCell(rowIndex);
	col->setCell(rowIndex, newCell);

	return true;
}

bool CSVTable::editCell(const char* colName, size_t rowIndex, const char* newValue)
{
	if (!hasTitleRow || !colName)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
	{
		if (strcmp(columns[i]->getName(), colName) == 0)
			return editCell(i, rowIndex, newValue);
	}
	return false;
}

bool CSVTable::addRow(Cell** newRow, size_t size)
{
	if (!newRow || size != columnsCount || columnsCount == 0)
		return false;

	for (size_t col = 0; col < columnsCount; ++col)
	{
		if (!newRow[col])
			return false;
	}

	if (!safeCloneToPreviousState())
		return false;

	for (size_t col = 0; col < columnsCount; ++col)
	{
		columns[col]->addCell(newRow[col]);
	}

	++rowsCount;
	return true;
}

bool CSVTable::addRowCopy(size_t rowIndex)
{
	if (rowIndex >= rowsCount || columnsCount == 0)
		return false;

	Cell** copiedRow = new (std::nothrow) Cell * [columnsCount];
	if (!copiedRow)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
		copiedRow[i] = nullptr;

	try {
		for (size_t i = 0; i < columnsCount; ++i)
			copiedRow[i] = columns[i]->getCell(rowIndex)->clone(); 
	}
	catch (...) {
		for (size_t i = 0; i < columnsCount; ++i)
			delete copiedRow[i];
		delete[] copiedRow;
		return false;
	}

	bool success = addRow(copiedRow, columnsCount);

	delete[] copiedRow;
	return success;
}

bool CSVTable::addRowFromAggregation(NewRowType type)
{
	Cell** row = new (std::nothrow) Cell * [columnsCount] {};
	if (!row)
		return false;

	for (size_t i = 0; i < columnsCount; ++i)
		row[i] = nullptr;

	try {
		for (size_t i = 0; i < columnsCount; ++i) {
			switch (type) {
			case NewRowType::Min:
				row[i] = columns[i]->getMinValue();
				break;
			case NewRowType::Max:
				row[i] = columns[i]->getMaxValue();
				break;
			case NewRowType::MostFrequent:
				row[i] = columns[i]->getMostFrequentValue();
				break;
			}
		}
	}
	catch (...) {
		for (size_t i = 0; i < columnsCount; ++i)
			delete row[i];
		delete[] row;
		return false;
	}

	bool success = addRow(row, columnsCount);
	delete[] row;
	return success;
}

bool CSVTable::addRowFromMinValues()
{
	return addRowFromAggregation(NewRowType::Min);
}

bool CSVTable::addRowFromMaxValues()
{
	return addRowFromAggregation(NewRowType::Max);
}

bool CSVTable::addRowFromMostFrequentValues()
{
	return addRowFromAggregation(NewRowType::MostFrequent);
}