#ifndef UNTITLED7_LN_H
#define UNTITLED7_LN_H
#include <string_view>

#include <cstring>
#include <iostream>

class LN
{
	uint32_t *data;
	size_t capacity;
	size_t size;
	bool negative;
	std::string value;

  public:
	uint32_t *getData() const { return data; }
	size_t getSize() const { return size; }
	bool getNegative() const { return negative; }
	LN(long long value);
	LN(const LN &other);
	LN(uint32_t value);
	LN(std::string_view str);
	LN(const char *str);
	LN();
	LN(LN &&other) noexcept;
	~LN();

	LN &operator=(const LN &other);
	LN &operator=(LN &&other) noexcept;

	void init();
	void add_element(uint32_t new_element);
	uint32_t get_element(size_t index) const;

	friend LN operator+(const LN &a, const LN &b);
	friend LN &operator+=(LN &a, const LN &b);
	friend LN operator-(const LN &a, const LN &b);
	friend LN &operator-=(LN &a, const LN &b);
	friend LN operator*(const LN &a, const LN &b);
	friend LN &operator*=(LN &a, const LN &b);
	friend LN operator-(const LN &a);
	friend bool operator<(const LN &a, const LN &b);
	friend bool operator<=(const LN &a, const LN &b);
	friend bool operator>(const LN &a, const LN &b);
	friend bool operator>=(const LN &a, const LN &b);
	friend bool operator==(const LN &a, const LN &b);
	friend bool operator!=(const LN &a, const LN &b);
	friend bool operator<(const LN &a, int b);
	friend bool operator>=(const LN &a, int b);
};

class ErrorMemory : public std::exception
{
};
LN operator"" _ln(const char *str, std::size_t);

#endif
