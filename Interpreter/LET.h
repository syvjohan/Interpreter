#pragma once

#include <string>
#include <regex>

#include "Map.h"

class LET
{
public:
	LET(std::string &expression);
	LET();
	~LET();

	std::string getName();
	std::string getValue();
	int getDatatype();

	void setName(std::string name);
	void setValue(std::string value);
	void setDataType(int datatype);

private:
	std::string name;
	std::string parsedValue;
	int datatype; //float = 2. int = 1, string = 3.

	void identifyPartsInExpression(std::string &expression);

	char validateOperator(char op);
	bool isOperator(char op);
	float generateRandomNumber();
	float doCalc(float value1, char op, float value2);
	std::string sortStringForward(std::string str);

	std::string& subdivideValue(std::string &expression);
	void setDefaultValue();

	int validateOperatorType(char op);
	bool isParanthesis(char op);
	bool isNumber(const std::string str);
	std::string transformKeywordsToValues(std::string &str);
	void transformValueAfterDatatype();
};

