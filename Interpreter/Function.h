#pragma once

#include "LET.h"
#include "Defs.h"

#include <string>

class Function
{
public:
	Function(std::string expression);
	Function(const Function &obj);
	Function();
	~Function();
	void identifyPartsInHead(const std::string &expression);

	bool addVariable(const std::string &name, const std::string &value, const int &datatype);
	bool addArg(const std::string &name, const std::string &value, const int &datatype);

	void setFunctionName(const std::string &name);

	void setArgValue(const std::string value);

	void setLineNumberStart(const int start);
	void setLineNumberEnd(const int end);
	void setCallingPoint(const std::string &linenumber);

	void setReturnValue(const std::string &value);

	std::string getFunctionName() const;

	LET getVariableByName(const std::string &name);

	std::string getLinenumberStart() const;
	std::string getLinenumberEnd() const;
	std::string getCallingPoint() const;

	std::string getReturnValue() const;

	void removeVariable(const LET &variable);
	int doVarExist(std::string name, LET *container, int size);
	Function& emptyBody(Function &function);

private:
	struct FunctionData
	{
		std::string functionName;

		LET *varContainer;
		int varLen;
		int varCapacity;

		LET *argsContainer;
		int argsLen;
		int argsCapacity;

		std::string datatypes[3];
		int datatypesLen;

		int linenumberStart;
		int linenumberEnd;
		std::string callingPoint;
		std::string returnValue;

	}*data;

	void initializeValues();
};

