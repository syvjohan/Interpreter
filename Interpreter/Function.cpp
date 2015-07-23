#include "Function.h"

Function::Function() {
	data = DBG_NEW FunctionData();
	initializeValues();
	argsLen = 3;
	argsDatatype[0] = "int";
	argsDatatype[1] = "dec";
	argsDatatype[2] = "str";
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

	data->callingPoint = "";

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
	std::string getArgs = expr.substr(findOpenParanthes + 1, findCloseParanthes - findOpenParanthes - 1);
	//if args exist.
	if (getArgs != "") {
		//Assume there is more then 1 arg get there position.
		std::vector<std::pair<std::string, std::string>> args;
		std::string tmp = getArgs;
		std::string name = "";
		for (int i = 0; i != tmp.length(); i++) {

			size_t findINT = tmp.find("int");
			size_t findSTR = tmp.find("str");
			size_t findDEC = tmp.find("dec");

			if (findINT < findSTR && findINT < findDEC) {
				tmp.erase(findINT, 3);

				size_t findInt = tmp.find("int");
				size_t findStr = tmp.find("str");
				size_t findDec = tmp.find("dec");
				if (findInt < findStr && findInt < findDec) {
					name = tmp.substr(0, findInt);
				}
				else if (findStr < findInt && findStr < findDec) {
					name = tmp.substr(0, findStr);
				}
				else if (findDec < findStr && findDec < findInt) {
					name = tmp.substr(0, findDec);
				}
				else {
					name = tmp;
				}

				tmp.erase(0, name.length());
				args.push_back(std::make_pair(name, "int"));
			}
			else if (findDEC < findSTR && findDEC < findINT) {
				tmp.erase(findDEC, 3);

				size_t findInt = tmp.find("int");
				size_t findStr = tmp.find("str");
				size_t findDec = tmp.find("dec");
				if (findInt < findStr && findInt < findDec) {
					name = tmp.substr(0, findInt);
				}
				else if (findStr < findInt && findStr < findDec) {
					name = tmp.substr(0, findStr);
				}
				else if (findDec < findStr && findDec < findInt) {
					name = tmp.substr(0, findDec);
				}
				else {
					name = tmp;
				}

				tmp.erase(0, name.length());
				args.push_back(std::make_pair(name, "dec"));
			}
			else if (findSTR < findINT && findSTR < findDEC) {
				tmp.erase(findSTR, 3);

				size_t findInt = tmp.find("int");
				size_t findStr = tmp.find("str");
				size_t findDec = tmp.find("dec");
				if (findInt < findStr && findInt < findDec) {
					name = tmp.substr(0, findInt);
				}
				else if (findStr < findInt && findStr < findDec) {
					name = tmp.substr(0, findStr);
				}
				else if (findDec < findStr && findDec < findInt) {
					name = tmp.substr(0, findDec);
				}
				else {
					name = tmp;
				}

				tmp.erase(0, name.length());
				args.push_back(std::make_pair(name, "str"));
			}
			else {
				errHandler.updateLog("ERROR: 018");
			}

			i = -1;
		}
		
		for (int i = 0; i != args.size(); i++) {
			std::string defaultValue = "";
			int datatype = 0;
			if (args[i].second == "int") {
				addArg(args[i].first, defaultValue = "0", datatype = 1);
			}
			else if (args[i].second == "str") {
				addArg(args[i].first, defaultValue = "", datatype = 3);
			}
			else if (args[i].second == "dec") {
				addArg(args[i].first, defaultValue = "0", datatype = 2);
			}
			else {
				//error in syntax datatype is missing...
				errHandler.updateLog("ERROR: 014");
				return;
			}
		}
	}
}

bool Function::addVariable(const std::string &name, const std::string &value, const int &datatype) {
	if (!datatype == 3 && name != "") {
		if (name == "" || value == "" || datatype == 0) {
			errHandler.updateLog("ERROR: 013");
		}
	}

	int isExistVar = doVarExist(name, data->varContainer);
	int isExistArg = doVarExist(name, data->argsContainer);
	if (isExistVar != -1) {
		data->varContainer[isExistVar].setDataType(datatype);
		data->varContainer[isExistVar].setValue(value);
		return true;
	}
	else if (isExistArg != -1) {
		errHandler.updateLog("ERROR: 030");
		return false;
	}

	//create a new variable to insert
	LET var;
	var.setName(name);
	var.setDataType(datatype);
	var.setValue(value);

	data->varContainer.push_back(var);

	return true;
}

bool Function::addArg(const std::string &name, const std::string &value, const int &datatype) {
	if (!datatype == 3 && name != "") {
		if (name == "" || value == "" || datatype == 0) {
			errHandler.updateLog("ERROR: 013");
		}
	}
	
	int isExistVar = doVarExist(name, data->varContainer);
	int isExistArg = doVarExist(name, data->argsContainer);

	if (isExistArg != -1) {
		data->varContainer[isExistArg].setDataType(datatype);
		data->varContainer[isExistArg].setValue(value);
		return true;
	}
	else if (isExistVar != -1) {
		errHandler.updateLog("ERROR: 030");
		return false;
	}

	//create a new variable to insert
	LET var;
	var.setName(name);
	var.setDataType(datatype);
	var.setValue(value);

	data->argsContainer.push_back(var);

	return true;
}

int Function::doVarExist(std::string name, std::vector<LET> container) {
	for (int i = 0; i != container.size(); i++) {
		if (name == container[i].getName()) {
			return i;
		}
	}
	return -1;
}

LET Function::getVariableByName(const std::string &name) {
	LET variable;
	if (data != nullptr) {
		for (int i = 0; i != data->varContainer.size(); i++) {
			if (data->varContainer[i].getName() == name) {
				return data->varContainer[i];
			}
		}

		for (int i = 0; i != data->argsContainer.size(); i++) {
			if (data->argsContainer[i].getName() == name) {
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
	function.data->argsContainer.clear();

	function.data->varContainer.clear();

	function.data->callingPoint = "";

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

void Function::setArgValue(int pos, const std::string value) {
	if (pos > data->argsContainer.size()) { return errHandler.updateLog("ERROR: 031"); }
	data->argsContainer[pos].setValue(value);
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

int Function::getArgLen() const {
	return data->argsContainer.size();
}
