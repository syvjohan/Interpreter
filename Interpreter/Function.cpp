#include "Function.h"

Function::Function(std::string &expression) {
	variablesLen = 0;
	variables = new LET[10];

	argsLen = 0;
	argsContainer = new LET[10];

	datatypesLen = 3;
	datatypes[0] = "int";
	datatypes[1] = "dec";
	datatypes[2] = "str";

	if (expression != "") {
		identifyPartsInExpression(expression);
	}
}

Function::Function()
{
}

Function::~Function()
{
}

void Function::identifyPartsInExpression(std::string &expression) {
	size_t findOpenParanthes = expression.find('(');
	size_t findCloseParanthes = expression.find(')');

	//find function name
	if (findOpenParanthes != std::string::npos) {
		setFunctionName(expression.substr(0, findOpenParanthes));
	}

	//find args
	std::string args = expression.substr(findOpenParanthes +1, findCloseParanthes - findOpenParanthes -1);
	LET *var = new LET();
	
	//get datatype.
	size_t findInt = args.find("int");
	size_t findStr = args.find("str");
	size_t findDec = args.find("dec");
	if (findInt != std::string::npos) {
		var->setDataType(1);
		var->setValue("0");
	}
	else if (findStr != std::string::npos) {
		var->setDataType(3);
		var->setValue("");
	}
	else if (findDec != std::string::npos) {
		var->setDataType(2);
		var->setValue("0");
	}
	else {
		//error in syntax datatype is missing...
		return;
	}

	std::string argName = args.substr(3, args.length() - 3);
	var->setName(argName);
	*(argsContainer + argsLen) = *var;
	++argsLen;
}

void Function::addVariable(LET variable) {
	*(variables + variablesLen) = variable;
	++variablesLen;
}

LET Function::getVariable(std::string name) {
	for (int i = 0; i != variablesLen; i++) {
		if (variables[i].getName() == name) {
			return variables[i];
		}
	}
	//trying to use a none existing variable!
}

void Function::setLineNumberStart(int start) {
	this->linenumberStart = start;
}

void Function::setLineNumberEnd(int end) {
	this->linenumberEnd = end;
}

void Function::setFunctionName(std::string name) {
	this->functionName = name;
}

std::string Function::getFunctionName() {
	return this->functionName;
}