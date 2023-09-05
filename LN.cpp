#include "LN.h"

#include <string_view>

#include <cmath>

using namespace std;

LN::LN(long long value) : data((uint32_t*)malloc(sizeof(uint32_t) * 2)), size(1), capacity(2)
{
	if (data == nullptr)
	{
		throw ErrorMemory();
	}
	data[0] = (uint32_t)(value & 0xFFFFFFFF);
}

LN::LN(const LN& other) : data((uint32_t*)malloc(sizeof(uint32_t) * other.size)), size(other.size), capacity(other.size)
{
	negative = other.negative;
	if (data == nullptr)
	{
		throw ErrorMemory();
	}
	memcpy(data, other.data, sizeof(uint32_t) * other.size);
}
LN::LN(uint32_t value) : data((uint32_t*)malloc(sizeof(uint32_t) * 2)), size(1), capacity(2)
{
	if (data == nullptr)
	{
		throw ErrorMemory();
	}
	data[0] = (int)(value & 0xFFFFFFFF);
}

LN::LN(std::string_view str) : LN(str.data()) {}

LN::LN(const char* str)
{
	negative = str[0] == '-';
	size_t len = strlen(str);
	size_t numElements = (len + 7 - negative) / 8;
	data = (uint32_t*)malloc(sizeof(uint32_t) * numElements);
	size = numElements;
	capacity = numElements;
	size_t dataIndex = 0;
	for (size_t strIndex = 0; strIndex < len - negative; strIndex += 8)
	{
		uint32_t currentDigit = 0;
		uint32_t currentPlace = 1;
		size_t l = std::min((size_t)(8), len - strIndex);
		for (size_t i = 0; i < l; i++)
		{
			char currentChar = str[len - strIndex - 1 - i];
			uint32_t digit = 0;
			if (currentChar >= '0' && currentChar <= '9')
			{
				digit = (uint32_t)(currentChar - '0');
			}
			else if (currentChar >= 'A' && currentChar <= 'F')
			{
				digit = (uint32_t)(currentChar - 'A' + 10);
			}
			else if (currentChar >= 'a' && currentChar <= 'f')
			{
				digit = (uint32_t)(currentChar - 'a' + 10);
			}
			currentDigit += digit * currentPlace;
			currentPlace *= 16;
		}
		data[dataIndex++] = currentDigit;
	}
}

LN::LN(LN&& other) noexcept : data(other.data), size(other.size), capacity(other.capacity)
{
	other.data = nullptr;
	other.size = 0;
	other.capacity = 0;
}

LN::LN() : data(nullptr), size(0), capacity(0)
{
	init();
}

void LN::init()
{
	capacity = 1024;
	size = 0;
	data = (uint32_t*)malloc(sizeof(uint32_t) * capacity);
}

void LN::add_element(uint32_t new_element)
{
	if (size >= capacity)
	{
		size_t new_capacity = capacity * 2;
		uint32_t* new_data = (uint32_t*)realloc(data, sizeof(uint32_t) * new_capacity);
		if (!new_data)
		{
			throw ErrorMemory();
		}
		data = new_data;
		capacity = new_capacity;
	}
	data[size++] = new_element;
}

uint32_t LN::get_element(size_t index) const
{
	if (index >= size)
	{
		return 0;
	}
	return data[index];
}

LN::~LN()
{
	if (data != nullptr)
	{
		free(data);
	}
}

LN& LN::operator=(const LN& other)
{
	if (this != &other)
	{
		negative = other.negative;
		free(data);
		size = 0;
		capacity = 0;
		init();
		for (size_t i = 0; i < other.size; i++)
		{
			add_element(other.data[i]);
		}
	}
	return *this;
}

LN& LN::operator=(LN&& other) noexcept
{
	if (this != &other)
	{
		negative = other.negative;
		free(data);
		data = other.data;
		size = other.size;
		capacity = other.capacity;
		other.data = nullptr;
		other.size = 0;
		other.capacity = 0;
	}
	return *this;
}

bool operator<(const LN& a, int b)
{
	if (a.size < 1)
	{
		return true;
	}
	return a.get_element(0) < b;
}

bool operator>=(const LN& a, int b)
{
	if (a.size < 1)
	{
		return false;
	}
	return a.get_element(0) >= b;
}

bool operator<(const LN& a, const LN& b)
{
	if (a.size < b.size)
	{
		return true;
	}
	else if (a.size > b.size)
	{
		return false;
	}

	for (int i = a.size - 1; i >= 0; --i)
	{
		if (a.data[i] < b.data[i])
		{
			return true;
		}
		else if (a.data[i] > b.data[i])
		{
			return false;
		}
	}
	return false;
}

bool operator<=(const LN& a, const LN& b)
{
	return !(b < a);
}

bool operator>(const LN& a, const LN& b)
{
	return b < a;
}

bool operator>=(const LN& a, const LN& b)
{
	return !(a < b);
}

bool operator==(const LN& a, const LN& b)
{
	if (a.size != b.size)
	{
		return false;
	}
	for (size_t i = 0; i < a.size; ++i)
	{
		if (a.data[i] != b.data[i])
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const LN& a, const LN& b)
{
	return !(a == b);
}

LN operator"" _ln(const char* str, std::size_t)
{
	return LN((uint32_t)(std::strtoull(str, nullptr, 10)));
};

LN operator+(const LN& a, const LN& b)
{
	LN result;
	uint32_t size_a = a.size;
	uint32_t size_b = b.size;
	uint32_t max_size = std::max(size_a, size_b);
	uint32_t carry = 0;

	if (a.negative && !b.negative)
	{
		LN abs_a = a;
		LN abs_b = b;
		abs_a.negative = false;
		abs_b.negative = false;
		if (abs_a > abs_b)
		{
			result = abs_a - abs_b;
			result.negative = true;
		}
		else
		{
			result = abs_b - abs_a;
		}
		return result;
	}
	else if (!a.negative && b.negative)
	{
		LN abs_a = a;
		LN abs_b = b;
		abs_a.negative = false;
		abs_b.negative = false;
		if (abs_a > abs_b)
		{
			result = abs_a - abs_b;
		}
		else
		{
			result = abs_b - abs_a;
			result.negative = true;
		}
		return result;
	}

	if (max_size >= result.capacity)
	{
		result.capacity = max_size + 1;
		result.data = (uint32_t*)(malloc(result.capacity * sizeof(uint32_t)));
	}

	for (size_t i = 0; i < max_size; i++)
	{
		uint64_t sum = carry;
		if (i < size_a)
		{
			sum += (uint64_t)(a.get_element(i));
		}
		if (i < size_b)
		{
			sum += (uint64_t)(b.get_element(i));
		}
		result.add_element((uint32_t)(sum % ((uint64_t)1 << 32)));
		carry = (uint32_t)(sum / ((uint64_t)1 << 32));
	}

	if (carry != 0)
	{
		result.add_element(carry);
	}

	if (a.negative && b.negative)
	{
		result.negative = true;
	}
	else if (!a.negative && !b.negative)
	{
		result.negative = false;
	}

	return result;
}

LN operator-(const LN& a, const LN& b)
{
	LN result;
	size_t size_a = a.size;
	size_t size_b = b.size;
	size_t max_size = std::max(size_a, size_b);
	size_t carry = 0;

	if (b > a)
	{
		if (a < 0)
		{
			result = a + b;
			result.negative = true;
			return result;
		}
		else
		{
			result = b - a;
			result.negative = true;
			return result;
		}
	}
	else if (a.negative && !b.negative)
	{
		LN abs_a = a;
		abs_a.negative = false;
		result = abs_a + b;
		result.negative = true;
		return result;
	}
	else if (!a.negative && b.negative)
	{
		LN abs_b = b;
		abs_b.negative = false;
		result = a + abs_b;
		result.negative = false;
		return result;
	}

	for (size_t i = 0; i < max_size; i++)
	{
		uint32_t diff = carry;
		if (i < size_a)
		{
			diff += a.get_element(i);
		}
		if (i < size_b)
		{
			diff -= b.get_element(i);
		}
		if (diff < 0)
		{
			diff += 10;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		result.add_element(diff);
	}
	result.negative = a.negative;
	return result;
}

LN operator*(const LN& a, const LN& b)
{
	LN result;
	size_t aSize = a.size;
	size_t bSize = b.size;
	result.negative = (a.negative != b.negative);

	size_t prodSize = aSize + bSize;
	uint32_t* prodDigits = (uint32_t*)malloc(sizeof(uint32_t) * prodSize);
	memset(prodDigits, 0, sizeof(uint32_t) * prodSize);
	for (size_t i = 0; i < bSize; ++i)
	{
		uint32_t carry = 0;

		for (size_t j = 0; j < aSize; ++j)
		{
			uint32_t prod = a.data[j] * b.data[i] + carry + prodDigits[i + j];
			prodDigits[i + j] = prod % 10;
			carry = prod / 10;
		}

		if (carry > 0)
		{
			prodDigits[i + aSize] += carry;
		}
	}

	size_t nonZeroIndex = prodSize - 1;
	while (nonZeroIndex > 0 && prodDigits[nonZeroIndex] == 0)
	{
		--nonZeroIndex;
	}

	result.size = nonZeroIndex + 1;
	result.data = (uint32_t*)malloc(sizeof(uint32_t) * result.size);
	memcpy(result.data, prodDigits, sizeof(uint32_t) * result.size);

	free(prodDigits);

	return result;
}

LN operator-(const LN& num)
{
	LN result = num;
	result.negative = !result.negative;
	return result;
}

LN& operator+=(LN& a, const LN& b)
{
	a = a + b;
	return a;
}

LN& operator-=(LN& a, const LN& b)
{
	a = a - b;
	return a;
}

LN& operator*=(LN& a, const LN& b)
{
	a = a * b;
	return a;
}