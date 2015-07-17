#pragma once

#include <string>
#include <regex>

#include "Map.h"
#include "Defs.h"

class LET
{
public:
	LET(const std::string &expression);
	LET(const LET &let); //copy constructor
	LET();
	~LET();

	std::string getName() const;
	std::string getValue() const;
	int getDatatype() const;

	void setName(const std::string &name);
	void setValue(const std::string &value);
	void setDataType(const int &datatype);

private:
	struct LetData {
		std::string name;
		std::string parsedValue;
		int datatype; //float = 2. int = 1, string = 3.
	}*data;

	void initializeValues();
	void identifyPartsInExpression(const std::string &expression);

	char validateOperator(char op);
	bool isOperator(char op);
	float generateRandomNumber();
	float doCalc(float value1, char op, float value2);
	std::string sortStringForward(std::string str);

	std::string subdivideValue(const std::string &expression);
	void setDefaultValue();

	int validateOperatorType(char op);
	bool isParanthesis(char op);
	bool isNumber(const std::string str);
	std::string transformKeywordsToValues(const std::string &str);
	void transformValueAfterDatatype();
};

