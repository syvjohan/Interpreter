#include "Manager.h"

Manager::Manager() {
	createErrorHandler();

	scanner = DBG_NEW Scanner(&errHandler);

	lengthNestedForLoops = 0;
	capacityFoorLoops = 10;
	nestedForLoops = DBG_NEW std::string[capacityFoorLoops];

	insideFunction = false;
	readLines = true;

	functions = DBG_NEW Functions(&errHandler);
}

Manager::~Manager()
{
	if (nestedForLoops != nullptr) {
		delete nestedForLoops;
		nestedForLoops = NULL;
	}

	if (functions != nullptr) {
		delete functions;
		functions = NULL;
	}
	
	if (function != nullptr) {
		delete function;
		function = NULL;
	}

	if (scanner != nullptr) {
		delete scanner;
		scanner = NULL;
	}
}

void Manager::init() {
	scanner->readFile("instructions/1.3Instructions.txt"); //code file to be read from!

	for (tableIndex = 0; tableIndex != scanner->length(); tableIndex++) {
		errHandler.setLineNumber(scanner->getLinenumber(tableIndex));
		table(scanner->getInstructionAt(tableIndex).first, scanner->getInstructionAt(tableIndex).second);
	}
}

void Manager::createErrorHandler() {
	ErrorHandler *pErrHandler;
	pErrHandler = &errHandler;

	errHandler.init();
}

void Manager::table(const std::string keyword, const std::string expression) {
	if (!readLines) {
		if (keyword == "ENDDEF") {
			function->setLineNumberEnd(std::stoi(scanner->getLinenumber(tableIndex)));
			readLines = true;
		}
		return;
	}

	if (keyword != "END" && keyword == "") { errHandler.updateLog("ERROR: 028"); }
	if (expression == "" && keyword != "END") { errHandler.updateLog("ERROR: 027"); }

	if (keyword == "PRINT") {
		evalPRINT(expression);
	}
	else if (keyword == "LET") {
		evalLET(expression);
	}
	else if (keyword == "INPUT") {
		evalINPUT(expression);
	}
	else if (keyword == "IF") {
		evalIF(expression);
	}
	else if (keyword == "GOTO") {
		evalGOTO(expression);
	}
	else if (keyword == "END") {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		exit(0);
	}
	else if (keyword == "FOR") {
		headLoopIndex = tableIndex;
		evalFOR(expression);
	}
	else if (keyword == "NEXT") {
		evalNEXT(expression);
	}
	else if (keyword == "INT") {
		evalLET(expression, 1);
	}
	else if (keyword == "STR") {
		evalLET(expression, 3);
	}
	else if (keyword == "DEC") {
		evalLET(expression, 2);
	} 
	else if (keyword == "DEFINE") {
		evalDEFINE(expression);
	}
	else if (keyword == "RETURN") {
		evalRETURN(expression);
	}
	else if (keyword == "ENDDEF") {
		evalENDDEF();
	}
}

void Manager::evalPRINT(const std::string &expression) {
	std::string expr = expression;
	bool isString = scanner->isString(expr);
	expr = scanner->trimPRINT(expr);

	size_t foundSemicolon = expr.find(';');
	if (foundSemicolon != std::string::npos) {
		expr = expr.erase(expr.length() -1, 1);
	}

	//if it is a variable and not a string.
	if (!isString) {
		expr = exchangeVariableNameToValue(expr);
	}

	if (foundSemicolon != std::string::npos) {
		std::cout << expr;
	}
	else {
		std::cout << expr << std::endl;
	}
}

void Manager::evalLET(const std::string &expression) {
	std::string expr = expression;
	//check if rhs value is a Functions.
	if (isAFunction (expression)) {
		doFunction(expr);
		return;
	}

	expr = exchangeVariableNameToValue(expr);
	 
	LET *var = DBG_NEW LET(expr, &errHandler);

	overwriteOldVariable(*var);

	delete var;
	var = NULL;
}

void Manager::evalLET(const std::string &expression, const int datatype) {
	std::string expr = expression;
	int d = datatype;
	//search for variable.
	std::string varExpression = "";
	for (int i = expr.length(); i != -1; i--) {
		if (isspace(expr[i])) {
			std::string varName = expr.substr(i + 1, expr.length() - i);
			expr.erase(i, expr.length() - i);

			varExpression.append(getDatatypeAsString(d));
			varExpression.append(varName);

			LET *variable = DBG_NEW LET(varExpression, &errHandler);

			//Store variable on heap or stack
			if (lengthNestedForLoops < 1) {
				addVariable(*variable, 1);
			}
			else {
				addVariable(*variable, 2);
			}

			varExpression = "";
			delete variable;
			variable = NULL;
		}
	}
}

void Manager::evalINPUT(const std::string &expression) {
	std::string expr = expression;
	std::string input = "";
	std::cin.clear();
	std::getline(std::cin, input);

	expr.append("=");
	expr.append(input);

	if (!isValidInput(input)) { errHandler.updateLog("ERROR: 029"); }

	LET *var = DBG_NEW LET(expr, &errHandler);

	overwriteOldVariable(*var);

	delete var;
	var = NULL;
}

void Manager::evalIF(const std::string &expression) {
	std::string expr = expression;
	LET var1;
	LET var2;
	LET *tmp = nullptr;

	bool isVariable1 = false;
	bool isVariable2 = false;

	size_t foundTHEN = expr.find("THEN");
	size_t opPos = getCompareOperatorPos(&expr);

	std::string foundVar1 = expr.substr(0, opPos);
	std::string foundVar2 = expr.substr(opPos + 1, foundTHEN - opPos - 1);

	//get first variable. HEAP
	for (int i = 0; i != variablesHeap.size(); i++) {
		if (foundVar1 == variablesHeap.at(i).getName() && opPos > 0) {
			var1 = variablesHeap.at(i);
			isVariable1 = true;
			break;
		}
	}

	//get first variable. STACK
	if (!isVariable1 && lengthNestedForLoops > 0) {
		for (int i = 0; i != variablesStack.size(); i++) {			
			if (foundVar1 == variablesStack.at(i).getName() && opPos > 0) {
				var1 = variablesStack.at(i);
				isVariable1 = true;
				break;
			}
		}
	}
	
	//get second variable. HEAP
	for (int k = 0; k != variablesHeap.size(); k++) {
		if (foundVar2 == variablesHeap.at(k).getName() && opPos < foundTHEN) {
			var2 = variablesHeap.at(k);
			isVariable2 = true;
			break;
		}
	}

	//get second variable. STACK
	if (!isVariable2 && lengthNestedForLoops > 0) {
		for (int i = 0; i != variablesStack.size(); i++) {
			if (foundVar2 == variablesStack.at(i).getName() && opPos < foundTHEN) {
				var2 = variablesStack.at(i);
				isVariable1 = true;
				break;
			}
		}
	}

	//If compared values are no variablesHeap instead they are hardcoded values.
	std::string value = "";
	if (!isVariable2) {
		if (opPos != std::string::npos && foundTHEN != std::string::npos) {
			value = expr.substr(opPos + 1, foundTHEN - opPos - 1);
		}
		else {
			//syntax error, problem with equal operator and keyword THEN.
			errHandler.updateLog("ERROR: 020");
		}
	}
	else if (!isVariable1) {
		if (opPos != std::string::npos && foundTHEN != std::string::npos) {
			value = expr.substr(0, opPos);
		}
		else {
			//syntax error, problem with equal operator and keyword THEN.
			errHandler.updateLog("ERROR: 020");
		}
	}

	if (!isVariable1 || !isVariable2) {
		std::string newExpression = "";
		//is value a string or a number. 
		bool isAlpha = std::regex_match(value, std::regex("^[A-Za-z]+$"));
		size_t isNumber = value.find_first_not_of("0123456789.");

		//If it is text
		if (isAlpha) {
			newExpression.append("=");
			newExpression.append(value);
			tmp = DBG_NEW LET(newExpression, &errHandler);
		}
		//if it is a number.
		else if (isNumber != std::string::npos) {
			newExpression.append(value);
			tmp = DBG_NEW LET(newExpression, &errHandler);
		}
		else {
			//syntax error, numbers and letters are mixed!
		}

		// create a temporary variable.
		if (!isVariable1) {
			var1 = *tmp;
		}
		else {
			var2 = *tmp;
		}
	}
	
	//compare datatypes.
	if (var1.getValue() != "" && var2.getValue() != "") {
		//get operator.
		size_t findOpGreater = expr.find('>');
		size_t findOpLess = expr.find('<');
		size_t findOpEqual = expr.find("=");
		bool result = false;

		//string
		if (var1.getDatatype() == 3 && var2.getDatatype() == 3) {
			//comapre variablesHeap.
			if (findOpGreater != std::string::npos) {
				result = var1.getValue().length() > var2.getValue().length();
			}
			else if (findOpLess != std::string::npos) {
				result = var1.getValue().length() < var2.getValue().length();
			}
			else if (findOpEqual != std::string::npos) {
				result = var1.getValue().compare(var2.getValue()) == 0;
			}
			else {
				result = false;
				//No operator was found
			}
		}
		// digits
		else if ((var1.getDatatype() == 1 || var1.getDatatype() == 2) && (var2.getDatatype() == 1 || var2.getDatatype() == 2)) {
			float val1 = std::stof(var1.getValue());
			float val2 = std::stof(var2.getValue());
			//comapre variablesHeap.
			if (findOpGreater != std::string::npos) {
				result = val1 > val2;
			}
			else if (findOpLess != std::string::npos) {
				result = val1 < val2;
			}
			else if (findOpEqual != std::string::npos) {
				result = val1 == val2;
			}
			else {
				result = false;
				//No operator was found
			}
		}
		else {
			//cannot compare digits with letters.
			errHandler.updateLog("ERROR: 000");
		}

		if (result) {
			//get linenumber.
			size_t line = expr.find_first_of("0123456789");
			if (line != expr.length() && line != std::string::npos) {
				std::string linenumber = expr.substr(line, expr.length() - line);

				//GOTO that line
				table("GOTO", linenumber);
			}
			else {
				// Line number is missing
				errHandler.updateLog("ERROR: 002");
			}
		}
		else {
			// Move to next line.
		}
	}
	else {
		//syntax error variablesHeap to compare has not been defined!
		errHandler.updateLog("ERROR: 008");
	}

	delete tmp;
	tmp = NULL;
}

void Manager::evalGOTO(const std::string &expression) {
	std::string expr = expression;
	int len = scanner->length();
	std::string linenumber = "";
	for (int i = 0; i != len; i++) {
		linenumber = scanner->getLinenumber(i);
		if (linenumber == expr) {
			std::string value = scanner->getInstructionAt(i).second;
			std::string key = scanner->getInstructionAt(i).first;

			// get instruction index.
			tableIndex = scanner->getIndex(std::stoi(linenumber));

			table(key, value);
			return;
		}
	}
	// Line has not been defiend, cannot goto undefiend line.
	errHandler.updateLog("ERROR: 002");
}

void Manager::evalFOR(const std::string &expression) {
	std::string expr = expression;
	size_t foundOpEqual = expr.find("=");
	size_t foundTO = expr.find("TO");

	std::string stopVar = expr.substr(foundTO + 2, expr.length() - foundTO);
	std::string incrementVarName = expr.substr(0, foundOpEqual);

	//check if increment variable already exist.
	if (lengthNestedForLoops > 0) {
		if (*(nestedForLoops + lengthNestedForLoops -1) == incrementVarName) {
			for (int h = 0; h != variablesHeap.size(); h++) {
				if (variablesHeap[h].getName() == stopVar) {
					for (int s = 0; s != variablesStack.size(); s++) {
						//check if stop value has been reached if so exit loop.
						int value1 = std::stoi(variablesStack[s].getValue());
						int value2 = std::stoi(variablesHeap[h].getValue());
						if (value1 > value2) {
							endLoop();
							break;
						}
						break;
					}
				}
			}
			return;
		}
	}
	
	if (foundOpEqual && foundTO != std::string::npos && foundTO != expr.length()) {
		//create increment variable
		std::string incrementVarValue = expr.substr(foundOpEqual + 1, foundTO - foundOpEqual - 1);
		std::string incrementVar = "";
		incrementVar.append(incrementVarName).append("=").append(incrementVarValue);
		LET *var = DBG_NEW LET(incrementVar, &errHandler);
		addVariable(*var, 2);
		positionStackForScope = variablesStack.size(); //sets first variable position on stack i for loop.

		//stop value.
		bool isAlpha = std::regex_match(stopVar, std::regex("^[A-Za-z]+$"));
		size_t isNumber = stopVar.find_first_not_of("0123456789");
		bool validRhsValue = false;
		if (isAlpha || isNumber) {
			//check if variable exist
			for (int i = 0; i != variablesStack.size(); i++) {
				if (variablesHeap[i].getName() == stopVar && var->getDatatype() == 3 || var->getDatatype() == 1) {
					validRhsValue = true;
					incrementNestedForLoops(var->getName());
				}
			}

			if (!validRhsValue) {
				// cannot use a none initialized variable.
				errHandler.updateLog("ERROR: 001");
			}
		}
		else {
			// No valid rhs value!
			errHandler.updateLog("ERROR: 018");
		}
	}
	else {
		// syntax error keyword TO and equal operator are incorrect.
		errHandler.updateLog("ERROR: 021");
	}
}

void Manager::evalNEXT(const std::string &variable) {
	std::string var = variable;
	bool VariableonStack = false;
	if (var.length() != 0) {
		//check if variable exist.
		if (lengthNestedForLoops > 0) {
			if (var == nestedForLoops[lengthNestedForLoops -1]) {
				for (int i = 0; i != variablesStack.size(); i++) {
					if (variablesStack[i].getName() == var) {
						VariableonStack = true;
						std::string strValue = variablesStack[i].getValue();
						int datatype = variablesStack[i].getDatatype();
						if (datatype == 1) {
							int value = stoi(strValue) + 1;
							strValue = std::to_string(value);
							variablesStack[i].setValue(strValue);
							gotoLoopHead();
							break;
						}
						else {
							//increment variable datatype can only be an INT.
							errHandler.updateLog("ERROR: 022");
						}
					}
				}
		}
			if (!VariableonStack) {
				// variable was not found on stack!
				errHandler.updateLog("ERROR: 007");
			}
		}
		else {
			// refer to wrong varible in hierarchy to increment.
			errHandler.updateLog("ERROR: 001");
		}
	}
	else {
		// incorrect syntax no increment variable has been added.
		errHandler.updateLog("ERROR: 018");
	}
}

void Manager::evalDEFINE(const std::string &expression) {
	std::string expr = expression;
	readLines = false;
	function = DBG_NEW Function(expr, &errHandler);
	//get linenumber for header.
	function->setLineNumberStart(std::stoi(scanner->getLinenumber(tableIndex + 1)));
	functions->addNewFunction(*function);
}

void Manager::evalRETURN(const std::string &expression) {
	if (insideFunction && expression != "") {
		LET var = function->getVariableByName(expression);
		function->setReturnValue(var.getValue());
	}
}

void Manager::evalENDDEF() {
	//get linenumber for end of Functions.
	function->setLineNumberEnd(std::stoi(scanner->getLinenumber(tableIndex - 1)));
}

//Change variable name to variable value in string if variable name exist in expression.
std::string Manager::exchangeVariableNameToValue(const std::string expression) {
	std::string expr = expression;
	//if it is not a variable
	size_t foundStr = expr.find_first_of('"');
	if (foundStr != std::string::npos) {
		return expr;
	}

	size_t foundOperator = getOperatorPos(expr);
	std::string tmpLhs = expr.substr(0, foundOperator +1);
	std::string name = "";

	//value shall not be exchanged
	if (foundOperator != std::string::npos) {
		if (expr.substr(foundOperator, 1) == "=") {
			expr.erase(0, tmpLhs.length());
			name = tmpLhs;
		}
	}

	std::string tmp = "";
	if (expr != "") {
		for (int i = 0; i <= expr.length(); i++) {
			size_t foundOperator = getOperatorPos(expr);
			std::string lhs = "";
			std::string lhsValue = "";
			std::string op = "";
			std::string rhs = "";
			std::string paranthes = "";
			if (foundOperator != std::string::npos) {
				lhs = expr.substr(0, foundOperator);

				// check for typecast datatype.
				size_t typeInt = lhs.find("INT");
				size_t typeFloat = lhs.find("DEC");
				size_t typeString = lhs.find("STR");
				std::string typecast = "";
				if (typeInt != std::string::npos) {
					typecast = lhs.substr(typeInt, 3);
					lhs.erase(typeInt, 3);
				}
				else if (typeString != std::string::npos) {
					typecast = lhs.substr(typeString, 3);
					lhs.erase(typeString, 3);
				}
				else if (typeFloat != std::string::npos) {
					typecast = lhs.substr(typeFloat, 3);
					lhs.erase(typeFloat, 3);
				}

				//if lhs contains operators remove them.
				size_t foundOperatorLhs = getOperatorPos(lhs);
				if (foundOperatorLhs != std::string::npos) {
					paranthes = lhs.substr(foundOperatorLhs, 1);
					if (paranthes != ")" && paranthes != "(") {
						paranthes = "";
					}
					lhs.erase(foundOperatorLhs, 1);
				}

				op = expr.substr(foundOperator, 1);
				rhs = expr.substr(foundOperator + 1, expr.length() - foundOperator);

				if (lhs != "") {
					if (lengthNestedForLoops > 0) {
						//exchange name to value. STACK!
						for (int k = 0; k != variablesStack.size(); k++) {
							if (lhs == variablesStack[k].getName()) {
								lhsValue = variablesStack[k].getValue();
								break;
							}
						}
					}
					else {
						//exchange name to value. HEAP!
						for (int k = 0; k != variablesHeap.size(); k++) {
							if (lhs == variablesHeap[k].getName()) {
								lhsValue = variablesHeap[k].getValue();
								break;
							}
						}
					}

					//if no stored variable was found
					if (lhsValue == "") {
						lhsValue = lhs;
					}

					//exchange name to value. LOCAL VARIABLE
					if (lhs == lhsValue && function != nullptr) {
						LET tmp = function->getVariableByName(lhs);
						if (lhs == tmp.getName()) {
							lhsValue = tmp.getValue();
						}
					}

					//if there was a paranthes in lhs put it back.
					if (foundOperatorLhs != std::string::npos && paranthes != "") {
						lhsValue.insert(foundOperatorLhs, paranthes);
					}

					tmp.append(typecast);
					tmp.append(lhsValue);
					tmp.append(op);
				}

				expr.erase(0, lhs.length() + typecast.length() + op.length());
			}
		}

		//only one value
		if (expr.length() != 0) {
			std::string paranthes = "";
			size_t foundOperatorLhs = getOperatorPos(expr);
			if (foundOperatorLhs != std::string::npos) {
				paranthes = expr.substr(foundOperatorLhs, 1);
				if (paranthes != ")" && paranthes != "(") {
					paranthes = "";
				}
				expr.erase(foundOperatorLhs, 1);
			}

			//change name to value, STACK
			for (int k = 0; k != variablesStack.size(); k++) {
				if (expr == variablesStack[k].getName()) {
					expr = variablesStack[k].getValue();
					break;
				}
			}

			//change name to value, HEAP
			for (int k = 0; k != variablesHeap.size(); k++) {
				if (expr == variablesHeap[k].getName()) {
					expr = variablesHeap[k].getValue();
					break;
				}
			}

			if (insideFunction) {
				LET letTmp = function->getVariableByName(expr);
				if (expr == letTmp.getName()) {
					expr = letTmp.getValue();
				}
			}

			//if there was a paranthes in lhs put it back.
			if (foundOperatorLhs != std::string::npos && paranthes != "") {
				expr.insert(foundOperatorLhs, paranthes);
			}

			tmp.append(expr);
			expr.erase(0, expr.length());
		}
		expr = tmp;
	}
	expr.insert(0, name);
	return expr;
}

//overwrite the old value with the new value.
void Manager::overwriteOldVariable(const LET newVar) {
	if (lengthNestedForLoops > 0) {
		for (int s = 0; s != variablesStack.size(); s++) {
			if (newVar.getName() == variablesStack.at(s).getName()) {
				variablesStack.at(s).setValue(newVar.getValue());
				variablesStack.at(s).setDataType(newVar.getDatatype());
				return;
			}
		}
		addVariable(newVar, 2);
	}
	else {
		for (int h = 0; h != variablesHeap.size(); h++) {
			if (newVar.getName() == variablesHeap.at(h).getName()) {
				variablesHeap.at(h).setValue(newVar.getValue());
				variablesHeap.at(h).setDataType(newVar.getDatatype());
				//std::swap(variablesHeap[h], variablesHeap.back());
				//variablesHeap.pop_back();
				return;
			}
		}
		addVariable(newVar, 1);
	}

	if (insideFunction) {
		addVariable(newVar, 0);
	}
}

size_t Manager::getCompareOperatorPos(const std::string *expression) {
	size_t opEqual = expression->find("=");
	size_t opLess = expression->find("<");
	size_t opLarger = expression->find(">");
	if (opEqual != std::string::npos) {
		return opEqual;
	}
	else if (opLess != std::string::npos) {
		return opLess;
	}
	else if (opLarger != std::string::npos) {
		return opLarger;
	}

	errHandler.updateLog("ERROR: 023");
	return std::string::npos;
}

int Manager::getOperatorPos(const std::string expression) {
	for (int i = 0; i != expression.length(); i++) {
		if (expression[i] == '=') {
			return i;
		}
		else if (expression[i] == '<') {
			return i;
		}
		else if (expression[i] == '>') {
			return i;
		}
		else if (expression[i] == '*') {
			return i;
		}
		else if (expression[i] == '+') {
			return i;
		}
		else if (expression[i] == '-') {
			return i;
		}
		else if (expression[i] == '/') {
			return i;
		}
		else if (expression[i] == '%') {
			return i;
		}
		else if (expression[i] == '(') {
			return i;
		}
		else if (expression[i] == ')') {
			return i;
		}
		else if (expression[i] == '<') {
			return i;
		}
	}
	return -1;
}

bool Manager::isOperator(char op) {
	return (op == '+' || op == '-' || op == '/' || op == '*' || op == ')' ||
		op == '(' || op == '=');
}

void Manager::incrementNestedForLoops(const std::string &variableName) {
	*(nestedForLoops + lengthNestedForLoops) = variableName;
	++lengthNestedForLoops;
}

void Manager::decrementNestedForLoops() {
	--lengthNestedForLoops;
}

void Manager::gotoLoopHead() {
	endLoopIndex = tableIndex + 1;
	tableIndex = headLoopIndex;
	
	eraseVariablesFromStack();

	std::string value = scanner->getInstructionAt(headLoopIndex).second;
	std::string key = scanner->getInstructionAt(headLoopIndex).first;

	return table(key, value);
}

void Manager::endLoop() {
	std::string value = scanner->getInstructionAt(endLoopIndex).second;
	std::string key = scanner->getInstructionAt(endLoopIndex).first;

	decrementNestedForLoops();
	positionStackForScope = 0;
	tableIndex = endLoopIndex;
	variablesStack.clear();

	return table(key, value);
}

std::string Manager::getDatatypeAsString(const int datatype) {
	if (datatype == 1) {
		return "INT";
	}
	else if (datatype == 2) {
		return "FLOAT";
	}
	else if (datatype == 3) {
		return "STR";
	}

	errHandler.updateLog("ERROR: 029");
	return "";
}

void Manager::eraseVariablesFromStack() {
	for (int i = variablesStack.size(); i != positionStackForScope; i--) {
		variablesStack.pop_back();
		break;
	}
}

void Manager::addVariable(const LET &variable, const int memoryType) {
	if (lengthNestedForLoops > 0) {
		//incrementNestedForLoops(variable.getName());
		variablesStack.push_back(variable);
	}
	else if (insideFunction && function != nullptr) {
		int val = variable.getDatatype();
		function->addVariable(variable.getName(), variable.getValue(), val);
	}
	else if (!insideFunction && lengthNestedForLoops == 0) {
		if (memoryType == 1) {
			variablesHeap.push_back(variable);
		}
		else {
			variablesStack.push_back(variable);
		}
	}
}

void Manager::doFunction(const std::string &expression) {
	std::string expr = expression;
	size_t foundOpEqual = expr.find('=');
	size_t foundOpenParanthes = expr.find('(');
	size_t foundCloseParanthes = expr.find(')');

	//create variable.
	std::string varName = expr.substr(0, foundOpEqual);
	LET *var = DBG_NEW LET();
	//add info
	var->setName(varName);
	var->setDataType(1);
	var->setValue("0");

	//get function parameter
	std::string parameter = expr.substr(foundOpenParanthes +1, foundCloseParanthes - foundOpenParanthes -1);
	//get function parameter value.
	std::vector<std::pair<size_t, std::string>> args;
	for (int i = 0; i != variablesHeap.size(); i++) {
		size_t findarg = parameter.find(variablesHeap[i].getName());
		if (findarg != std::string::npos) {
			args.push_back(std::make_pair(findarg, variablesHeap[i].getValue()));
		}
	}

	if (function->getArgLen() > args.size() || function->getArgLen() < args.size()) { errHandler.updateLog("ERROR: 031"); }

	//Sort arg position.
	std::sort(std::begin(args), std::end(args));
	for (int i = 0; i != args.size(); i++) {
		//set new parameter value.
		function->setArgValue(i, args[i].second);
	}

	//get function name
	std::string functionName = expr.substr(foundOpEqual +1, foundOpenParanthes - foundOpEqual -1);

	//execute function body
	function->setCallingPoint(scanner->getLinenumber(tableIndex));
	insideFunction = true;

	//loop function body. 
	std::string linenumber = "";
	int startIndex = scanner->getIndex(std::stoi(function->getLinenumberStart()));
	int endIndex = scanner->getIndex(std::stoi(function->getLinenumberEnd()));
	for (int i = startIndex; i != endIndex; i++) {
		linenumber = scanner->getLinenumber(i);
		evalGOTO(linenumber);
	}

	insideFunction = false;
	
	//set variable to function return value.
	var->setValue(function->getReturnValue());

	//set table index to continue after function.
	tableIndex = scanner->getIndex(std::stoi(function->getCallingPoint()));

	//clear memory since leaving function.
	//empty function
	function = &function->eraseBodyContent(*function);

	//check datatype
	bool isAlpha = std::regex_match(var->getValue(), std::regex("^[A-Za-z]+$"));
	size_t isINT = var->getValue().find_first_not_of("0123456789");
	size_t isFLOAT = var->getValue().find_first_not_of("0123456789.");
	if (isAlpha) {
		var->setDataType(3);
	}
	else if (isINT == std::string::npos) {
		var->setDataType(1);
	}
	else if (isFLOAT == std::string::npos) {
		var->setDataType(2);
	}
	else {
		//datatype error, datatype no recognize.
		errHandler.updateLog("ERROR: 014");
	}

	//check if variable exist on heap
	for (int i = 0; i != variablesHeap.size(); i++) {
		if (variablesHeap[i].getName() == var->getName()) {
			variablesHeap[i].setDataType(var->getDatatype());
			variablesHeap[i].setValue(var->getValue());
			return;
		}
	}
	//save variable on heap
	variablesHeap.push_back(*var);
}

bool Manager::isAFunction(const std::string expression) {
	size_t findEqual = expression.find('=');
	size_t findOpenParanthes = expression.find('(');
	if (findEqual != std::string::npos && findOpenParanthes != std::string::npos) {
		std::string name = expression.substr(findEqual + 1, findOpenParanthes - findEqual -1);
		//check if function name exist
		Function tmp = functions->getFunction(name);
		if (tmp.getFunctionName() != "") {
			return true;
		}
	}

	return false;
}

bool Manager::isValidInput(const std::string expression) {
	bool isAlpha = std::regex_match(expression, std::regex("^[A-Za-z]+$"));
	size_t foundNumber = expression.find_first_not_of("0123456789.");
	if (!isAlpha && foundNumber != std::string::npos) { 
		return false; 
	}
	else if (foundNumber < 0) {
		errHandler.updateLog("ERROR: 004");
	}
	return true;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Manager manager;
	manager.init();
	
	system("pause");

	return 0;
}