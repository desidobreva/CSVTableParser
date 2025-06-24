
#include <iostream>
#include <string>
#include <fstream>
#include "CSVTable.h"
#include "CommandProcessor.h"

void printMenu() 
{
    std::cout << "\n=== MENU ===\n";
    std::cout << "open               - Open CSV file\n";
    std::cout << "save               - Save current table\n";
    std::cout << "saveas             - Save table to another file\n";
    std::cout << "print              - Print the table\n";
    std::cout << "undo               - Undo last change\n";
    std::cout << "sort               - Sort table by column\n";
    std::cout << "filter             - Filter table by condition\n";
    std::cout << "renamecol          - Rename a column\n";
    std::cout << "setcolnames        - Set column names\n";
    std::cout << "clearcolname       - Clear column name by index\n";
    std::cout << "removecol          - Remove column by name or index\n";
    std::cout << "duplicatecol       - Duplicate column\n";
    std::cout << "reordercols        - Reorder columns\n";
    std::cout << "editcell           - Edit a specific cell\n";
    std::cout << "addrow_copy        - Add a copy of an existing row\n";
    std::cout << "addrow_min         - Add row with min values\n";
    std::cout << "addrow_max         - Add row with max values\n";
    std::cout << "addrow_freq        - Add row with most frequent values\n";
    std::cout << "exit               - Exit the program\n";
    std::cout << "===================\n";
}

void run() 
{
    CSVTable table;
    std::string currentFile;
    bool changesMade = false;

    while (true) 
    {
        printMenu();

        std::cout << "> ";
        std::string command;
        std::cin >> command;

        if (command == "open") openFile(table, currentFile, changesMade);
        else if (command == "save") saveToFile(table, currentFile, changesMade);
        else if (command == "saveas") saveAsToFile(table, currentFile, changesMade);
        else if (command == "print") table.print();
        else if (command == "undo") changesMade |= handleUndo(table);
        else if (command == "sort") changesMade |= handleSort(table);
        else if (command == "filter") changesMade |= handleFilter(table);
        else if (command == "renamecol") changesMade |= handleRenameColumn(table);
        else if (command == "setcolnames") changesMade |= handleSetColumnNames(table);
        else if (command == "clearcolname") changesMade |= handleClearColumnName(table);
        else if (command == "removecol") changesMade |= handleRemoveColumn(table);
        else if (command == "duplicatecol") changesMade |= handleDuplicateColumn(table);
        else if (command == "reordercols") changesMade |= handleReorderColumns(table);
        else if (command == "editcell") changesMade |= handleEditCell(table);
        else if (command == "addrow_copy") changesMade |= handleAddRowCopy(table);
        else if (command == "addrow_min") changesMade |= handleAddRowMin(table);
        else if (command == "addrow_max") changesMade |= handleAddRowMax(table);
        else if (command == "addrow_freq") changesMade |= handleAddRowMostFrequent(table);
        else if (command == "exit") {
            if (handleExit(table, currentFile, changesMade))
                break;
        }
        else 
        {
            std::cout << "Unknown command.\n";
        }
    }
}

// main за конзолно приложение (може да се види как работят всички функции)
// и командите open, save, saveas, undo, print
int main()
{
    run();
}

// тестове без конзолата
//int main()
//{
//    try
//    {
//        CSVTable table;
//        std::ifstream ifs("test.csv");
//        if (!ifs.is_open())
//        {
//            throw std::invalid_argument("Bad file name!");
//        }
//
//        std::ofstream ofs("test2.csv");
//        if (!ofs.is_open())
//        {
//            throw std::invalid_argument("Bad file name!");
//        }
//
//        table.loadFromFile(ifs);
//        table.print();
//        std::cout << '\n';
//        table.filterByColumn(1, ">", "56");
//        table.print();
//        std::cout << '\n';
//        table.sortByColumn((size_t)0, false);
//        table.print();
//        std::cout << '\n';
//        table.addRowCopy(1);
//        table.print();
//        std::cout << '\n';
//        table.removeColumn(3);
//        table.print();
//        std::cout << '\n';
//        table.addRowFromMaxValues();
//        table.print(ofs);
//        std::cout << '\n';
//        table.renameColumn(2, "New Column name");
//        table.print();
//    }
//    catch (std::exception& exc)
//    {
//        std::cerr << exc.what();
//    }
//    catch (const std::string& s)
//    {
//        std::cerr << s;
//    }
//}