#include "Function.h"

Function::Function() {
	data = DBG_NEW FunctionData();
	initializeValues();
}

Function::Function(const std::string expression, ErrorHandler *errHandler) : Function()
{
	this->errHandler = *errHandler;
	if (expression != "") {
		identifyPartsInHead(expression);
	}
}

//copy constructor.
Function::Function(const Function &obj) {
	data = new FunctionData;
	*data = *obj.data;
}

void Function::initializeValues() {
	data->functionName = "";

	data->argsContainer = NULL;
	data->argsCapacity = 10;
	data->argsLen = 0;

	data->varLen = 0;
	data->varCapacity = 10;
	data->varContainer = DBG_NEW LET[data->varCapacity];

	data->callingPoint = "";

	data->datatypesLen = 3;
	data->datatypes[0] = "int";
	data->datatypes[1] = "dec";
	data->datatypes[2] = "str";

	data->linenumberEnd = -1;
	data->linenumberStart = -1;
	data->callingPoint = "";
	data->returnValue = "";
}

Function::~Function()
{
	if (data != nullptr) {
		delete data;
		data = NULL;
	}
}

void Function::identifyPartsInHead(const std::string &expression) {
	std::string expr = expression;
	size_t findOpenParanthes = expr.find('(');
	size_t findCloseParanthes = expr.find(')');

	//find Functions name
	if (findOpenParanthes != std::string::npos) {
		setFunctionName(expr.substr(0, findOpenParanthes));
	}

	//find args
	std::string args = expr.substr(findOpenParanthes + 1, findCloseParanthes - findOpenParanthes - 1);

	std::string argName = args.substr(3, args.length() - 3);

	//get datatype.
	size_t findInt = args.find("int");
	size_t findStr = args.find("str");
	size_t findDec = args.find("dec");

	std::string defaultValue = "";
	int datatype = 0;
	if (findInt != std::string::npos) {
		addArg(argName, defaultValue = "0", datatype = 1);
	}
	else if (findStr != std::string::npos) {
		addArg(argName, defaultValue = "", datatype = 3);
	}
	else if (findDec != std::string::npos) {
		addArg(argName, defaultValue = "0", datatype = 2);
	}
	else {
		//error in syntax datatype is missing...
		errHandler.updateLog("ERROR: 014");
		return;
	}
}

bool Function::addVariable(const std::string &name, const std::string &value, const int &datatype) {
	if (name == "" || value == "" || datatype == 0) {
		errHandler.updateLog("ERROR: 013");
	}

	std::string n = name;
	std::string v = value;
	int d = datatype;

	int isExistVar = doVarExist(name, data->varContainer, data->varLen);
	int isExistArg = doVarExist(name, data->argsContainer, data->argsLen);
	if (isExistVar != -1) {
		data->varContainer[isExistVar].setDataType(d);
		data->varContainer[isExistVar].setValue(v);
		return true;
	}
	else if (isExistArg != -1) {
		return false;
	}

	data->varContainer = DBG_NEW LET();

	data->varContainer->setName(n);
	data->varContainer->setDataType(d);
	data->varContainer->setValue(v);

	data->varLen++;

	return true;
}

bool Function::addArg(const std::string &name, const std::string &value, const int &datatype) {
	if (name == "" || value == "" || datatype == 0) {
		errHandler.updateLog("ERROR: 013");
	}
	std::string n = name;
	std::string v = value;
	int d = datatype;

	int isExistVar = doVarExist(name, data->varContainer, data->varLen);
	int isExistArg = doVarExist(name, data->argsContainer, data->argsLen);

	if (data->argsContainer == nullptr) {
		data->argsContainer = DBG_NEW LET();
	}

	if (isExistArg != -1) {
		data->varContainer[isExistArg].setDataType(d);
		data->varContainer[isExistArg].setValue(v);
		return true;
	}
	else if (isExistVar != -1) {
		return false;
	}

	data->argsContainer->setName(n);
	data->argsContainer->setDataType(d);
	data->argsContainer->setValue(v);

	data->argsLen++;

	return true;
}

int Function::doVarExist(std::string name, LET *container, int size) {
	for (int i = 0; i != size; i++) {
		if (name == container[i].getName()) {
			return i;
		}
	}
	return -1;
}

LET Function::getVariableByName(const std::string &name) {
	LET variable;
	if (data != nullptr) {
		for (int i = 0; i != data->varLen; i++) {
			if (data->varContainer[i].getName() == name) {
				return data->varContainer[i];
			}
		}

		for (int i = 0; i != data->argsLen; i++) {
			if (data->argsContainer->getName() == name) {
				return data->argsContainer[i];
			}
		}
	}

	//trying to get a none existing variable!
	errHandler.updateLog("ERROR: 001");
	return variable;
}

//Observe function does not erase function name.
Function& Function::eraseBodyContent(Function &function) {
	function.data->argsContainer = NULL;
	function.data->argsCapacity = 10;
	function.data->argsLen = 0;

	function.data->varLen = 0;
	function.data->varCapacity = 10;
	function.data->varContainer = DBG_NEW LET[data->varCapacity];

	function.data->callingPoint = "";

	function.data->datatypesLen = 3;
	function.data->datatypes[0] = "int";
	function.data->datatypes[1] = "dec";
	function.data->datatypes[2] = "str";

	function.data->linenumberEnd = -1;
	function.data->linenumberStart = -1;
	function.data->callingPoint = "";
	function.data->returnValue = "";

	return function;
}

void Function::setLineNumberStart(const int start) {
	data->linenumberStart = start;
}

void Function::setLineNumberEnd(const int end) {
	data->linenumberEnd = end;
}

void Function::setFunctionName(const std::string &name) {
	data->functionName = name;
}

std::string Function::getFunctionName() const {
	return data->functionName;
}

void Function::setArgValue(const std::string value) {
	data->argsContainer->setValue(value);
}

std::string Function::getLinenumberStart() const {
	return std::to_string(data->linenumberStart);
}

std::string Function::getLinenumberEnd() const {
	return std::to_string(data->linenumberEnd);
}

void Function::setCallingPoint(const std::string &linenumber) {
	data->callingPoint = linenumber;
}

std::string Function::getCallingPoint() const {
	return data->callingPoint;
}

void Function::setReturnValue(const std::string &value) {
	data->returnValue = value;
}

std::string Function::getReturnValue() const {
	return data->returnValue;
}
