#include "Functions.h"

Functions::Functions() {
}

Functions::~Functions() {}

void Functions::addNewFunction(const Function function) {
	functions.push_back(function);
}

Function Functions::getFunction(std::string functionName) const {
	Function function;
	for (int i = 0; i != functions.size(); i++) {
		if (functions[i].getFunctionName() == functionName) {
			return functions[i];
		}
	}
	return function;
}