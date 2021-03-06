#pragma once

#include <string>
#include <regex>

#include "Map.h"
#include "Defs.h"
#include "ErrorHandler.h"

class LET
{
public:
	LET(const std::string &expression, ErrorHandler *errHandler);
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

	ErrorHandler errHandler;

	void initializeValues();
	void identifyPartsInExpression(const std::string &expression);

	char validateOperator(char op);
	bool isOperator(char op);
	bool prioritiesOperator(char op, int priority);

	float generateRandomNumber();
	std::string doCalc(const std::string &str1, const char &op, const std::string &str2);

	std::string subdivideValue(const std::string &expression);
	void setDefaultValue();
	std::string trimString(std::string &str);

	int validateOperatorType(char op);
	bool isParanthesis(char op);
	bool isNumber(const std::string str);
	std::string transformKeywordsToValues(const std::string &str);
	void transformValueAfterDatatype();
	bool isSameDatatype(const std::string str1, const std::string str2);
};

