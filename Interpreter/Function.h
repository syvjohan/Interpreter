#pragma once

#include "LET.h"

#include <string>

class Function
{
public:
	Function(std::string &expression);
	Function();
	~Function();

	void addVariable(LET variable);
	LET getVariable(std::string name);
	void setLineNumberStart(int start);
	void setLineNumberEnd(int end);
	std::string getFunctionName();
	void setFunctionName(std::string name);

private:
	std::string functionName;

	LET *variables;
	int variablesLen;

	LET *argsContainer;
	int argsLen;

	std::string datatypes[3];
	int datatypesLen;

	int linenumberStart;
	int linenumberEnd;

	void identifyPartsInExpression(std::string &expression);
};

