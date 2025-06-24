# CSVTableParser
## Project for OOP course @FMI, 2025
Project Description:

The project implements operations on CSV files through console commands. It allows loading the contents of a given file and saving the changes made to the file either in the same or a different user-specified file. The following commands are supported:

    open: Load a CSV file.

    save: Save the current data to the file.

    saveas: Save the data to a new file.

    print: Print the current table to the console.

    addrow_min: Add a new row with the minimum values for each column.

    addrow_max: Add a new row with the maximum values for each column.

    addrow_mostfrequent: Add a new row with the most frequent values for each column.

    undo: Undo the last change made to the table.

    exit: Exit the application, with a prompt to save any unsaved changes.

Additional functionalities:

    sort: Sort data by a specified column (either by column index or name) in ascending or descending order.

    filter: Filter rows based on conditions like ==, !=, <, <=, >, >= for a specified column.

    removeDuplicates: Remove duplicate rows from the table.

    renameColumn: Rename a column by index or name.

    removeColumn: Remove a column by index or name.

    duplicateColumn: Duplicate a column with a unique name.

    reorderColumns: Reorder columns based on a user-specified permutation of column indices.


