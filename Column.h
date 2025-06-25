#include "ColumnType.h"
#include "Cell.h"

class Column
{
public:
    Column(const char* name, ColumnType* type);
    Column(const Column& other);
    Column(Column&& other) noexcept;
    Column& operator=(const Column& other);
    Column& operator=(Column&& other) noexcept;
    ~Column() noexcept;

    ColumnType* getType() const;
    bool setType(ColumnType* type);
    bool setName(const char* name);
    const char* getName() const;

    void addCell(const Cell* cell);
    void addCell(Cell* cell);
    Cell* getCell(size_t rowIndex) const;

    Cell* getMinValue() const;
    Cell* getMaxValue() const;
    Cell* getMostFrequentValue() const;

    size_t getSize() const;
    size_t getCapacity() const;
    bool empty() const;
    
    void setCell(size_t rowIndex, Cell* cell);
    void setCell(size_t rowIndex, const Cell* cell);

    Column* clone() const;
    void swap(Column& other);
    void clearCells();
private:
    void free();
    void nullify(Cell** cells, size_t cap);
    void resize(size_t newCap);
private:
    char* name;
    ColumnType* type;
    Cell** cells;
    size_t size;
    size_t cap;
};
