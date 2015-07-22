#include "Functions.h"

Functions::Functions() {
}

Functions::Functions(ErrorHandler *errHandler) : Functions() {
	this->errHandler = *errHandler;
}

Functions::~Functions() {}

void Functions::addNewFunction(const Function &function) {
	functions.push_back(function);
}

Function Functions::getFunction(const std::string functionName) {
	Function function;
	for (int i = 0; i != functions.size(); i++) {
		if (functions[i].getFunctionName() == functionName) {
			return functions[i];
		}
	}

	return function;
}