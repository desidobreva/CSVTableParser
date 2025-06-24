#pragma once
#include "CSVTable.h"

void openFile(CSVTable& table, std::string& currentFile, bool& changesMade);
void saveToFile(const CSVTable& table, const std::string& currentFile, bool& changesMade);
void saveAsToFile(const CSVTable& table, std::string& currentFile, bool& changesMade);
bool handleExit(const CSVTable& table, const std::string& currentFile, bool changesMade);

bool handleUndo(CSVTable& table);
bool handleSort(CSVTable& table);
bool handleFilter(CSVTable& table);
bool handleRenameColumn(CSVTable& table);
bool handleSetColumnNames(CSVTable& table);
bool handleClearColumnName(CSVTable& table);
bool handleRemoveColumn(CSVTable& table);
bool handleDuplicateColumn(CSVTable& table);
bool handleReorderColumns(CSVTable& table);
bool handleEditCell(CSVTable& table);
bool handleAddRowCopy(CSVTable& table);
bool handleAddRowMin(CSVTable& table);
bool handleAddRowMax(CSVTable& table);
bool handleAddRowMostFrequent(CSVTable& table);