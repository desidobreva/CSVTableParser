#pragma once

#include <iostream>
#include <fstream>
#include "Column.h"

class CSVTable
{
public:
	CSVTable();
	CSVTable(const CSVTable& other);
	CSVTable(CSVTable&& other) noexcept;
	CSVTable& operator=(const CSVTable& other);
	CSVTable& operator=(CSVTable&& other) noexcept;
	~CSVTable() noexcept;
	CSVTable* clone() const;

	bool loadFromFile(std::ifstream& ifs, bool hasTitleRow = true);
	bool print(std::ostream& out = std::cout) const;

	bool sortByColumn(size_t columnIndex, bool ascending = true);
	bool sortByColumn(const char* columnName, bool ascending = true);
	bool filterByColumn(size_t columnIndex, const char* op, const char* value);
	bool filterByColumn(const char* columnName, const char* op, const char* value);
	bool removeDuplicateRows();

	size_t getColumnsCount() const;
	size_t getRowsCount() const;

	bool renameColumn(size_t index, const char* newName);
	const char* getColumnName(size_t index) const;
	int getColumnIndexByName(const char* name) const;
	bool setColumnNames(const char** newNames, size_t count);
	bool clearColumnNames();
	bool clearColumnName(size_t index);

	bool removeColumn(size_t columnIndex);
	bool removeColumn(const char* columnName);
	bool duplicateColumn(size_t columnIndex);
	bool duplicateColumn(const char* name);
	bool reorderColumns(const size_t* permutation, size_t size);

	bool editCell(size_t colIndex, size_t rowIndex, const char* newValue);
	bool editCell(const char* colName, size_t rowIndex, const char* newValue);

	bool addRowCopy(size_t rowIndex);
	bool addRowFromMinValues();
	bool addRowFromMaxValues();
	bool addRowFromMostFrequentValues();

	bool undo();
	void setSeparator(char ch);
	char getSeparator() const;
	void swap(CSVTable& other);
	bool isEmpty() const;
private:
	bool loadRowFromFile(std::ifstream& ifs, size_t currRowCount, Column** cols);
	static size_t getCellsInRowCount(const char*, char separator);
	static size_t splitLine(const char* line, char separator, char** tokens, size_t maxTokens);

	void free();
	void nullify();
	void clearColumns();
	bool safeCloneToPreviousState();

	bool addColumn(const Column* column);
	bool addColumn(Column* column);
	bool addRow(Cell** newRow, size_t size);
	bool addRow(const Cell** newRow, size_t size);

	enum class NewRowType {
		Min = 0,
		Max,
		MostFrequent
	}; // за добавянето на тип нов ред

	bool addRowFromAggregation(NewRowType type);
private:
	Column** columns;
	size_t columnsCount;
	size_t rowsCount;
	char separator = ',';
	bool hasTitleRow = false;

	CSVTable* previousState = nullptr;// за логиката свързана с undo
};