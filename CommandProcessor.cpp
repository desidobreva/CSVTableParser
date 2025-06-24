#include "CommandProcessor.h"
#include "utils.h"
#include <string>
#pragma warning (disable: 4996)

void openFile(CSVTable& table, std::string& currentFile, bool& changesMade) 
{
    std::cout << "Enter file name: ";
    std::cin >> currentFile;

    while (!HelperFunctions::hasCSVExtension(currentFile))
    {
        std::cout << "Incorrect format! Enter .csv file: ";
        std::cin >> currentFile;
    }

    std::cout << "Does the file contain a header row? (yes/no): ";
    std::string hasHeaderStr;
    std::cin >> hasHeaderStr;

    bool hasHeader = (hasHeaderStr == "yes" || hasHeaderStr == "y");

    std::ifstream in(currentFile);
    if (!in.is_open()) {
        std::cout << "File not found.\n";
        return;
    }

    if (!table.loadFromFile(in, hasHeader)) 
    {
        std::cout << "Failed to load file.\n";
    }
    else {
        changesMade = false;
        std::cout << "File loaded successfully.\n";
    }

    in.close();
}

void saveToFile(const CSVTable& table, const std::string& currentFile, bool& changesMade) 
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return;
    }

    if (currentFile.empty()) {
        std::cout << "No file opened.\n";
        return;
    }

    std::ofstream out(currentFile);
    if (!out) {
        std::cout << "Cannot open file for writing.\n";
        return;
    }

    table.print(out);
    changesMade = false;
    std::cout << "Saved.\n";

    out.close();
}

void saveAsToFile(const CSVTable& table, std::string& currentFile, bool& changesMade) 
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return;
    }

    std::cout << "Enter file name: ";
    std::string filename;
    std::cin >> filename;

    while (!HelperFunctions::hasCSVExtension(filename))
    {
        std::cout << "Incorrect format! Enter .csv file: ";
        std::cin >> filename;
    }

    std::ofstream out(filename);
    if (!out) {
        std::cout << "Cannot open file for writing.\n";
        return;
    }

    table.print(out);
    currentFile = filename;
    changesMade = false;
    std::cout << "Saved as " << filename << ".\n";

    out.close();
}

bool handleExit(const CSVTable& table, const std::string& currentFile, bool changesMade) {
    if (changesMade) {
        std::cout << "There are unsaved changes. Save? (yes/no): ";
        std::string answer;
        std::cin >> answer;

        if (answer == "yes" || answer == "y") {
            if (currentFile.empty()) {
                std::cout << "No file to save to.\n";
                return false;
            }

            std::ofstream out(currentFile);
            if (!out) {
                std::cout << "Cannot open file for writing.\n";
                return false;
            }

            table.print(out);
            out.close();
        }
    }

    std::cout << "Bye.\n";
    return true;
}

bool handleUndo(CSVTable& table) 
{
    if (table.undo()) {
        std::cout << "Undo successful.\n";
        return true;
    }
    std::cout << "Nothing to undo.\n";
    return false;
}

bool handleSort(CSVTable& table) 
{
    if (table.isEmpty()) 
    {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    std::string col;
    std::string order;
    std::cout << "Enter column index or name: ";
    std::cin.ignore();
    std::getline(std::cin, col);
    std::cout << "Ascending? (yes/no): ";
    std::cin >> order;
    bool asc = (order == "yes" || order == "y");

    bool result = isdigit(col[0])
        ? table.sortByColumn(static_cast<size_t>(std::stoi(col)), asc)
        : table.sortByColumn(col.c_str(), asc);

    if (result) {
        std::cout << "Sorted successfully.\n";
    }
    else
    {
        std::cout << "Sorting failed.\n";
    }

    return result;
}

bool handleFilter(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    std::string col, op, val;
    std::cout << "Column index or name: ";
    std::cin.ignore();
    std::getline(std::cin, col);
    std::cout << "Operator (==, !=, <, <=, >, >=): ";
    std::cin >> op;
    std::cout << "Value: ";
    std::cin.ignore();
    std::getline(std::cin, val);

    bool result = isdigit(col[0])
        ? table.filterByColumn(static_cast<size_t>(std::stoi(col)), op.c_str(), val.c_str())
        : table.filterByColumn(col.c_str(), op.c_str(), val.c_str());

    if (result) 
    {
        std::cout << "Filter applied.\n";
    }
    else 
    {
        std::cout << "Filter failed.\n";
    }

    return result;
}

bool handleRenameColumn(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    size_t index;
    std::string newName;
    std::cout << "Column index: ";
    std::cin >> index;
    std::cin.ignore(); // Изчистваме новия ред след числото

    std::cout << "New name: ";
    std::getline(std::cin, newName);

    return table.renameColumn(index, newName.c_str());
}

bool handleSetColumnNames(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    size_t count = table.getColumnsCount();

    const char** names = new const char* [count];

    for (size_t i = 0; i < count; ++i) {
        std::string input;
        std::cout << "Column " << i << " name: ";
        std::cin.ignore();
        std::getline(std::cin, input); 

        char* name = new char[input.size() + 1];
        std::strcpy(name, input.c_str());
        names[i] = name;
    }

    bool result = table.setColumnNames(names, count);

    for (size_t i = 0; i < count; ++i)
        delete[] names[i];
    delete[] names;

    return result;
}

bool handleClearColumnName(CSVTable& table) {
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    size_t index;
    std::cout << "Column index to clear name: ";
    std::cin >> index;
    return table.clearColumnName(index);
}

bool handleRemoveColumn(CSVTable& table) {
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    std::string col;
    std::cout << "Column index or name to remove: ";
    std::cin.ignore();
    std::getline(std::cin, col);

    return isdigit(col[0])
        ? table.removeColumn(std::stoi(col))
        : table.removeColumn(col.c_str());
}

bool handleDuplicateColumn(CSVTable& table) {
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    std::string col;
    std::cout << "Column index or name to duplicate: ";
    std::cin.ignore();
    std::getline(std::cin, col);

    return isdigit(col[0])
        ? table.duplicateColumn(std::stoi(col))
        : table.duplicateColumn(col.c_str());
}

bool handleReorderColumns(CSVTable& table) {
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    size_t count = table.getColumnsCount();

    size_t* perm = new size_t[count];
    std::cout << "Enter permutation (space-separated indices): ";
    for (size_t i = 0; i < count; ++i)
        std::cin >> perm[i];

    bool result = table.reorderColumns(perm, count);
    delete[] perm;
    return result;
}

bool handleEditCell(CSVTable& table) 
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }
    std::string col;
    size_t row;
    std::string newValue;

    std::cout << "Column index or name: ";
    std::cin.ignore();
    std::getline(std::cin, col);
    std::cout << "Row index: ";
    std::cin >> row;
    std::cout << "New value: ";
    std::cin.ignore();
    std::getline(std::cin, newValue);

    return isdigit(col[0])
        ? table.editCell(std::stoi(col), row, newValue.c_str())
        : table.editCell(col.c_str(), row, newValue.c_str());
}

bool handleAddRowCopy(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    size_t row;
    std::cout << "Row index to copy: ";
    std::cin >> row;
    return table.addRowCopy(row);
}

bool handleAddRowMin(CSVTable& table) {
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    if (table.addRowFromMinValues()) {
        std::cout << "Row added from min values.\n";
        return true;
    }

    std::cout << "Failed to add row.\n";
    return false;
}

bool handleAddRowMax(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    if (table.addRowFromMaxValues()) {
        std::cout << "Row added from max values.\n";
        return true;
    }

    std::cout << "Failed to add row.\n";
    return false;
}

bool handleAddRowMostFrequent(CSVTable& table)
{
    if (table.isEmpty()) {
        std::cout << "No data loaded. Use 'open' first.\n";
        return false;
    }

    if (table.addRowFromMostFrequentValues()) 
    {
        std::cout << "Row added from most frequent values.\n";
        return true;
    }

    std::cout << "Failed to add row.\n";
    return false;
}