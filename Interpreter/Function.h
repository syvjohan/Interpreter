#pragma once

#include "LET.h"
#include "Defs.h"

#include <string>
#include "ErrorHandler.h"

class Function
{
public:
	Function(const std::string expression, ErrorHandler *errHandler);
	Function(const Function &obj);
	Function();
	~Function();
	void identifyPartsInHead(const std::string &expression);

	bool addVariable(const std::string &name, const std::string &value, const int &datatype);
	bool addArg(const std::string &name, const std::string &value, const int &datatype);

	void setFunctionName(const std::string &name);

	void setArgValue(int pos, const std::string value);

	void setLineNumberStart(const int start);
	void setLineNumberEnd(const int end);
	void setCallingPoint(const std::string &linenumber);

	void setReturnValue(const std::string &value);

	std::string getFunctionName() const;

	LET getVariableByName(const std::string &name);

	std::string getLinenumberStart() const;
	std::string getLinenumberEnd() const;
	std::string getCallingPoint() const;
	int getArgLen() const;

	std::string getReturnValue() const;

	void removeVariable(const LET &variable);
	int doVarExist(std::string name, std::vector<LET> container);
	Function& eraseBodyContent(Function &function);

private:

	ErrorHandler errHandler;
	std::string argsDatatype[3];
	int argsLen;

	struct FunctionData
	{
		std::string functionName;

		std::vector<LET> varContainer;

		std::vector<LET> argsContainer;

		int linenumberStart;
		int linenumberEnd;
		std::string callingPoint;
		std::string returnValue;

	}*data;

	void initializeValues();
};

