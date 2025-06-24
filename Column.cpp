#include "Column.h"
#pragma warning (disable: 4996)

Column::Column(const char* name, ColumnType* type)
	: name(nullptr), type(nullptr), cells(nullptr), size(0), cap(8)
{
	if (!setName(name))
	{
		throw std::invalid_argument("Some error while setting new name!");
	}

	if (type && !setType(type))
	{
		delete[] this->name;
		throw std::invalid_argument("Bad type!");
	}

	try
	{
		cells = new Cell * [cap];
		nullify(cells, cap);
	}
	catch (...)
	{
		free();
		throw;
	}
}

void Column::nullify(Cell** cells, size_t cap)
{
	for (size_t i = 0; i < cap; i++)
	{
		cells[i] = nullptr;
	}
}

Column::Column(const Column& other)
	: name(nullptr), type(nullptr), cells(nullptr), size(other.size), cap(other.cap)
{
	if (other.name)
	{
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
	}

	try
	{
		type = other.type->clone();
		cells = new Cell * [cap];

		nullify(cells, cap);

		for (size_t i = 0; i < size; i++)
		{
			cells[i] = other.cells[i]->clone();
		}
	}
	catch (...)
	{
		free();
		throw;
	}
}

Column::Column(Column&& other) noexcept
	: name(nullptr), type(nullptr), cells(nullptr), size(other.size), cap(other.cap)
{
	name = other.name;
	type = other.type;
	cells = other.cells;

	other.name = nullptr;
	other.type = nullptr;
	other.cells = nullptr;
}

void Column::swap(Column& other)
{
	std::swap(name, other.name);
	std::swap(type, other.type);
	std::swap(cells, other.cells);
	std::swap(size, other.size);
	std::swap(cap, other.cap);
}

Column& Column::operator=(const Column& other)
{
	if (this != &other)
	{
		Column copy(other);
		*this = std::move(copy);
	}

	return *this;
}

Column& Column::operator=(Column&& other) noexcept
{
	swap(other);
	return *this;
}

Column::~Column() noexcept
{
	free();
}

void Column::free()
{
	delete[] name;
	delete type;

	for (size_t i = 0; i < size; i++)
	{
		delete cells[i];
	}

	delete[] cells;
	name = nullptr;
	type = nullptr;
	cells = nullptr;
}

void Column::clearCells()
{
	for (size_t i = 0; i < size; i++)
	{
		delete cells[i];
	}

	delete[] cells;
	cells = nullptr;
	size = 0;
	cap = 0;
}

ColumnType* Column::getType() const
{
	return type;
}

bool Column::setType(ColumnType* type)
{
	if (!type)
	{
		return false;
	}

	delete this->type;
	this->type = type->clone();
	return true;
}

bool Column::setName(const char* name)
{
	if (!name)
	{
		name = "";
	}

	char* newName = new (std::nothrow) char[strlen(name) + 1];
	if (!newName)
	{
		return false;
	}

	strcpy(newName, name);
	delete[] this->name;
	this->name = newName;

	return true;
}

const char* Column::getName() const
{
	return name;
}

void Column::addCell(const Cell* cell)
{
	if (!cell)
	{
		throw std::invalid_argument("Bad cell!");
	}

	if (size >= cap)
	{
		size_t newCap = cap == 0 ? 1 : cap;
		resize(newCap * 2);
	}

	cells[size++] = cell->clone();
}

void Column::addCell(Cell* cell)
{
	if (!cell)
	{
		throw std::invalid_argument("Bad cell!");
	}

	if (size >= cap)
	{
		size_t newCap = cap == 0 ? 1 : cap;
		resize(newCap * 2);
	}

	cells[size++] = cell;
}

Cell* Column::getCell(size_t rowIndex) const
{
	if (rowIndex >= size)
	{
		throw std::invalid_argument("Bad index!");
	}

	return cells[rowIndex];
}

Column* Column::clone() const
{
	return new Column(*this);
}

void Column::resize(size_t newCap)
{
	if (newCap <= cap) return;

	Cell** newCells = new Cell * [newCap];
	nullify(newCells, newCap);

	for (size_t i = 0; i < size; i++)
	{
		newCells[i] = cells[i];
	}

	delete[] cells;
	cells = newCells;
	cap = newCap;
}

size_t Column::getSize() const
{
	return size;
}

size_t Column::getCapacity() const
{
	return cap;
}

bool Column::empty() const
{
	return size == 0;
}

void Column::setCell(size_t rowIndex, Cell* cell)
{
	if (rowIndex >= size)
		throw std::out_of_range("Bad index");
	delete cells[rowIndex];
	cells[rowIndex] = cell;
}

void Column::setCell(size_t rowIndex, const Cell* cell)
{
	if (rowIndex >= size)
		throw std::out_of_range("Bad index");
	delete cells[rowIndex];
	cells[rowIndex] = cell->clone();
}

Cell* Column::getMinValue() const
{
	if (size == 0) return nullptr;
	Cell* min = cells[0];
	for (size_t i = 1; i < size; ++i) {
		if (type->compare(cells[i], min) < 0)
			min = cells[i];
	}
	return min->clone();
}

Cell* Column::getMaxValue() const
{
	if (size == 0) return nullptr;
	Cell* max = cells[0];
	for (size_t i = 1; i < size; ++i) {
		if (type->compare(cells[i], max) > 0)
			max = cells[i];
	}
	return max->clone();
}

Cell* Column::getMostFrequentValue() const
{
	if (size == 0) return nullptr;

	size_t bestCount = 0;
	Cell* best = nullptr;

	for (size_t i = 0; i < size; ++i) {
		size_t count = 1;
		for (size_t j = i + 1; j < size; ++j) {
			if (type->compare(cells[i], cells[j]) == 0)
				++count;
		}
		if (count > bestCount || (count == bestCount && type->compare(cells[i], best) < 0)) {
			bestCount = count;
			if (best) delete best;
			best = cells[i]->clone();
		}
	}

	return best;
}