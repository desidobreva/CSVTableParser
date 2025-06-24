#pragma once

class Cell;

class ColumnType
{
public:
	virtual ColumnType* clone() const = 0;
	virtual bool validate(const char* str) const = 0;
	virtual Cell* createCell(const char* str) const = 0;
	virtual int compare(const Cell* a, const Cell* b) const = 0;
	virtual ~ColumnType() noexcept = default;
};

class TypeText : public ColumnType
{
public:
	virtual ColumnType* clone() const override;
	virtual bool validate(const char* str) const override;
	virtual Cell* createCell(const char* str) const override;
	virtual int compare(const Cell* a, const Cell* b) const override;
};

class TypeNumber : public ColumnType
{
public:
	virtual ColumnType* clone() const override;
	virtual bool validate(const char* str) const override;
	virtual Cell* createCell(const char* str) const override;
	virtual int compare(const Cell* a, const Cell* b) const override;
};

class TypeCurrency : public ColumnType
{
public:
	virtual ColumnType* clone() const override;
	virtual bool validate(const char* str) const override;
	virtual Cell* createCell(const char* str) const override;
	virtual int compare(const Cell* a, const Cell* b) const override;
};

class TypeUCN : public ColumnType
{
public:
	virtual ColumnType* clone() const override;
	virtual bool validate(const char* str) const override;
	virtual Cell* createCell(const char* str) const override;
	virtual int compare(const Cell* a, const Cell* b) const override;
};

class TypeFacultyNumber : public ColumnType
{
public:
	virtual ColumnType* clone() const override;
	virtual bool validate(const char* str) const override;
	virtual Cell* createCell(const char* str) const override;
	virtual int compare(const Cell* a, const Cell* b) const override;
};