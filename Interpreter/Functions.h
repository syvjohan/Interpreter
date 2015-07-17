#pragma once

#include "Function.h"
#include "Defs.h"

#include <string>

class Functions
{
public:
	Functions(std::string &expression);
	Functions();
	~Functions();

	void addNewFunction(const Function function);
	Function getFunction(std::string functionName) const;

private:
	std::vector<Function> functions;
};

