#pragma once

#include "Function.h"
#include "Defs.h"

#include <string>
#include "ErrorHandler.h"

class Functions
{
public:
	Functions(const std::string &expression);
	Functions(const ErrorHandler &errHandler);
	Functions();
	~Functions();

	void addNewFunction(const Function &function);
	Function getFunction(const std::string functionName);

private:
	ErrorHandler errHandler;
	std::vector<Function> functions;
};

