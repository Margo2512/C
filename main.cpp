#include "LN.h"
#include "return_codes.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>

using namespace std;
bool isOperator(const std::string &str)
{
	static const std::string operators[] = { "+", "-", "*", "/", "%", "~", "_", "<", "<=", ">", ">=", "==", "!=" };
	static const int numOperators = sizeof(operators) / sizeof(operators[0]);

	for (int i = 0; i < numOperators; ++i)
	{
		if (str == operators[i])
		{
			return true;
		}
	}
	return false;
}

void output(std::ostream &os, const LN &num)
{
	if (num.getSize() == 0)
	{
		os << "0";
		return;
	}
	if (num.getNegative())
	{
		os << "-";
	}
	std::string hexDigit;
	for (size_t i = 0; i < num.getSize(); i++)
	{
		uint32_t currentDigit = num.getData()[i];
		for (size_t j = 0; j < 8; j++)
		{
			uint32_t remainder = currentDigit % 16;
			if (remainder < 10)
			{
				hexDigit.push_back((char)(remainder + '0'));
			}
			else
			{
				hexDigit.push_back((char)('A' + (remainder - 10)));
			}
			currentDigit /= 16;
		}
	}
	while (hexDigit.back() == '0')
	{
		hexDigit.pop_back();
	}
	if (hexDigit.empty())
	{
		os << '0';
		return;
	}
	reverse(hexDigit.begin(), hexDigit.end());
	os << hexDigit;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		return ERROR_PARAMETER_INVALID;
	}

	std::ifstream inputFile(argv[1]);

	if (!inputFile)
	{
		return ERROR_CANNOT_OPEN_FILE;
	}

	std::stack< LN > stack;
	std::string line;
	try
	{
		while (std::getline(inputFile, line))
		{
			if (isOperator(line))
			{
				LN result;
				LN operand2 = stack.top();
				stack.pop();
				if (line == "+")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 + operand2;
				}
				else if (line == "-")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 - operand2;
				}
				else if (line == "*")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 * operand2;
				}
				else if (line == "/")
				{
					return ERROR_UNSUPPORTED;
				}
				else if (line == "%")
				{
					return ERROR_UNSUPPORTED;
				}
				else if (line == "~")
				{
					return ERROR_UNSUPPORTED;
				}
				else if (line == "<")
				{
					LN operand1 = stack.top();
					stack.pop();
					if ((operand1 < operand2) == 0)
						result = LN("0");
					else
						result = LN("1");
				}
				else if (line == "<=")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 <= operand2 ? LN("1") : LN("0");
				}
				else if (line == ">")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 > operand2 ? LN("1") : LN("0");
				}
				else if (line == ">=")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 >= operand2 ? LN("1") : LN("0");
				}
				else if (line == "==")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 == operand2 ? LN("1") : LN("0");
				}
				else if (line == "!=")
				{
					LN operand1 = stack.top();
					stack.pop();
					result = operand1 != operand2 ? LN("1") : LN("0");
				}
				stack.push(result);
			}
			else
			{
				if (!line.empty() && line[0] == '-' && std::isdigit(line[1]))
				{
					std::string number = line.substr(1);
					LN operand2(number);
					operand2 = -operand2;
					stack.push(operand2);
				}
				else
				{
					stack.emplace(line.c_str());
				}
			}
		}
	} catch (ErrorMemory e)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	inputFile.close();
	std::ofstream outputFile(argv[2]);

	if (!outputFile)
	{
		return ERROR_CANNOT_OPEN_FILE;
	}

	while (!stack.empty())
	{
		output(outputFile, stack.top());
		outputFile << std::endl;
		stack.pop();
	}
	outputFile.close();
	return 0;
}