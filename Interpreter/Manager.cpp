#include "Manager.h"

Manager::Manager() {
	lengthNestedForLoops = 0;
	capacity = 10;
	nestedForLoops = new std::string[capacity];
}

Manager::~Manager()
{
}

void Manager::init() {
	scanner.readFile("1.2Instructions.txt"); //code file to be read from!

	int len = scanner.length();
	for (tableIndex = 0; tableIndex != len; tableIndex++) {
		table(scanner.getInstructionAt(tableIndex).first, scanner.getInstructionAt(tableIndex).second);
	}
}

void Manager::table(std::string keyword, std::string expression) {
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
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
}

void Manager::evalPRINT(std::string &expression) {
	bool isString = scanner.isString(expression);
	expression = scanner.trimPRINT(expression);

	size_t foundSemicolon = expression.find(';');
	if (foundSemicolon != std::string::npos) {
		expression = expression.erase(expression.length() - 1, 1);
	}

	//if it is a variable and not a string.
	if (!isString) {
		expression = exchangeVariableNameToValue(expression);
	}

	if (foundSemicolon != std::string::npos) {
		std::cout << expression;
	}
	else {
		std::cout << expression << std::endl;
	}
}

void Manager::evalLET(std::string &expression) {
	expression = exchangeVariableNameToValue(expression);
	
	LET *var = new LET(expression);
	overwriteOldVariableValue(var);
}

void Manager::evalLET(std::string &expression, int datatype) {
	//search for variable.
	std::string varExpression = "";
	for (int i = expression.length(); i != -1; i--) {
		if (isspace(expression[i])) {
			std::string varName = expression.substr(i +1, expression.length() - i);
			expression.erase(i, expression.length() - i);

			varExpression.append(getDatatypeAsString(datatype));
			varExpression.append(varName);

			LET *variable = new LET(varExpression);

			//Store variable on heap or stack
			if (lengthNestedForLoops < 1) {
				variablesHeap.push_back(*variable);
			}
			else {
				variablesStack.push_back(*variable);
			}

			varExpression = "";
			delete variable;
			variable = NULL;
		}
	}
}

void Manager::evalINPUT(std::string &expression) {
	std::string input = "";
	std::cin.clear();
	std::getline(std::cin, input);

	expression.append("=");
	expression.append(input);

	LET *var = new LET(expression);

	overwriteOldVariableValue(var);
}

void Manager::evalIF(std::string &expression) {
	LET var1;
	LET var2;
	LET *tmp = nullptr;

	bool isVariable1 = false;
	bool isVariable2 = false;

	size_t foundTHEN = expression.find("THEN");
	size_t opPos = getCompareOperatorPos(&expression);

	std::string foundVar1 = expression.substr(0, opPos);
	std::string foundVar2 = expression.substr(opPos + 1, foundTHEN - opPos - 1);

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
			value = expression.substr(opPos + 1, foundTHEN - opPos - 1);
		}
	}
	else if (!isVariable1) {
		if (opPos != std::string::npos && foundTHEN != std::string::npos) {
			value = expression.substr(0, opPos);
		}
	}
	else {
		//syntax error, problem with equal operator and keyword THEN.
	}

	if (!isVariable1 || !isVariable2) {
		std::string newExpression = "";
		//is value a string or a number. 
		bool isAlpha = std::regex_match(value, std::regex("^[A-Za-z]+$"));
		size_t isNumber = value.find_first_not_of("0123456789.");

		//If it is text
		if (isAlpha) {
			newExpression.append("tmp");
			newExpression.append("=");
			newExpression.append(value);
			tmp = new LET(newExpression);
		}
		//if it is a number.
		else if (isNumber != std::string::npos) {
			newExpression.append("tmp");
			newExpression.append(value);
			tmp = new LET(newExpression);
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
	if (var1.getDatatype() != var2.getDatatype()) {
		// cannot compare different datatypes.
	}
	else {
		//get operator.
		if (var1.getValue() != "" && var2.getValue() != "") {
			size_t findOpGreater = expression.find('>');
			size_t findOpLess = expression.find('<');
			size_t findOpEqual = expression.find("=");

			//comapre variablesHeap.
			bool result = false;
			if (findOpGreater != std::string::npos) {
				result = var1.getValue() > var2.getValue();
			}
			else if (findOpLess != std::string::npos) {
				result = var1.getValue() < var2.getValue();
			}
			else if (findOpEqual != std::string::npos) {
				result = var1.getValue() == var2.getValue();
			}
			else {
				result = false;
				//No operator was found
			}

			if (result) {
				//get linenumber.
				size_t line = expression.find_first_of("0123456789");
				if (line != expression.length() && line != std::string::npos) {
					std::string linenumber = expression.substr(line, expression.length() - line);
					
					//GOTO that line
					table("GOTO", linenumber);
				}
				else {
					// Line number is missing
				}
			}
			else {
				// Move to next line.
			}
		}
		else {
			//syntax error variablesHeap to compare has not been defined!
		}
	}

	delete tmp;
	tmp = NULL;
}

void Manager::evalGOTO(std::string &expression) {
	int len = scanner.length();
	std::string linenumber = "";
	for (int i = 0; i != len; i++) {
		linenumber = scanner.getLinenumber(i);
 		if (linenumber == expression) {
			std::string value = scanner.getInstructionAt(i).second;
			std::string key = scanner.getInstructionAt(i).first;

			// get instruction index.
			tableIndex = scanner.getIndex(std::stoi(linenumber));

			table(key, value);
			break;
		}
	}
	// Line has not been defiend, cannot goto undefiend line.
}

void Manager::evalFOR(std::string &expression) {
	size_t foundOpEqual = expression.find("=");
	size_t foundTO = expression.find("TO");

	std::string stopVar = expression.substr(foundTO + 2, expression.length() - foundTO);
	std::string incrementVarName = expression.substr(0, foundOpEqual);

	//check if increment variable already exist.
	if (lengthNestedForLoops > 0) {
		if (*(nestedForLoops + lengthNestedForLoops) == incrementVarName) {
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
					}
				}
			}
			return;
		}
	}
	
	if (foundOpEqual && foundTO != std::string::npos && foundTO != expression.length()) {
		//create increment variable
		std::string incrementVarValue = expression.substr(foundOpEqual + 1, foundTO - foundOpEqual -1);
		std::string incrementVar = "";
		incrementVar.append(incrementVarName).append("=").append(incrementVarValue);
		LET *var = new LET(incrementVar);
		variablesStack.push_back(*var);
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
			}
		}
		else {
			// No valid rhs value!
		}
	}
	else {
		// syntax error keyword TO and equal operator are incorrect.
	}
}

void Manager::evalNEXT(std::string &variable) {
	bool VariableonStack = false;
	if (variable.length() != 0) {
		//check if variable exist.
		if (lengthNestedForLoops > 0) {
			if (variable == nestedForLoops[lengthNestedForLoops]) {
				for (int i = 0; i != variablesStack.size(); i++) {
					if (variablesStack[i].getName() == variable) {
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
						}
					}
				}
		}
			if (!VariableonStack) {
				// variable was not found on stack!
			}
		}
		else {
			// refer to wrong varible in hierarchy to increment.
		}
	}
	else {
		// incorrect syntax no increment variable has been added.
	}
}
// sqr = 3*3 byter ut sqr mot ett värde.
//Change variable name to variable value in string if variable name exist in expression.
std::string Manager::exchangeVariableNameToValue(std::string expression) {
	//if it is not a variable
	size_t foundStr = expression.find_first_of('"');
	if (foundStr != std::string::npos) {
		return expression;
	}

	std::string tmp = "";
	if (expression != "") {
		for (int i = 0; i <= expression.length(); i++) {
			size_t foundOperator = getOperatorPos(&expression);
			std::string lhs = "";
			std::string lhsValue = "";
			std::string op = "";
			std::string rhs = "";
			if (foundOperator != std::string::npos) {
				lhs = expression.substr(0, foundOperator);
				op = expression.substr(foundOperator, 1);
				rhs = expression.substr(foundOperator + 1, expression.length() - foundOperator);

				if (lengthNestedForLoops > 0) {
					//exchange name to value. STACK!
					for (int k = 0; k != variablesStack.size(); k++) {
						if (lhs == variablesStack[k].getName()) {
							lhsValue = variablesStack[k].getValue();
							break;
						}
						lhsValue = lhs;
					}
				}
				else {
					//exchange name to value. HEAP!
					for (int k = 0; k != variablesHeap.size(); k++) {
						if (lhs == variablesHeap[k].getName()) {
							lhsValue = variablesHeap[k].getValue();
							break;
						}
						lhsValue = lhs;
					}
				}

				tmp.append(lhsValue);
				tmp.append(op);

				expression.erase(0, lhs.length() + op.length());
			}
		}
		//only one value
		if (expression.length() != 0) {
			//change name to value, HEAP
			if (lengthNestedForLoops > 0) {
				//change name to value, STACK
				for (int k = 0; k != variablesStack.size(); k++) {
					if (expression == variablesStack[k].getName()) {
						expression = variablesStack[k].getValue();
						break;
					}
				}
			}
			else {
				for (int k = 0; k != variablesHeap.size(); k++) {
					if (expression == variablesHeap[k].getName()) {
						expression = variablesHeap[k].getValue();
						break;
					}
				}
			}
			tmp.append(expression);
			expression.erase(0, expression.length());
		}
		expression = tmp;
	}

	return expression;
}

//overwrite the old value with the new value.
void Manager::overwriteOldVariableValue(LET *newVar) {
	if (lengthNestedForLoops > 0) {
		for (int s = 0; s != variablesStack.size(); s++) {
			if (newVar->getName() == variablesStack.at(s).getName()) {
				variablesStack.erase(variablesStack.begin() + s);
			}
		}
		variablesStack.push_back(*newVar);
	}
	else {
		for (int h = 0; h != variablesHeap.size(); h++) {
			if (newVar->getName() == variablesHeap.at(h).getName()) {
				variablesHeap.erase(variablesHeap.begin() + h);
			}
		}
		variablesHeap.push_back(*newVar);
	}
}

size_t Manager::getCompareOperatorPos(std::string *expression) {
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

	return std::string::npos;
}

size_t Manager::getOperatorPos(std::string *expression) {
	size_t opEqual = expression->find("=");
	size_t opLess = expression->find("<");
	size_t opLarger = expression->find(">");
	size_t opAsterix = expression->find("*");
	size_t opPlus = expression->find("+");
	size_t opMinus = expression->find("-");
	size_t opDividing = expression->find("/");
	size_t opModulos = expression->find("%");
	if (opEqual != std::string::npos) {
		return opEqual;
	}
	else if (opLess != std::string::npos) {
		return opLess;
	}
	else if (opLarger != std::string::npos) {
		return opLarger;
	}
	else if (opAsterix != std::string::npos) {
		return opAsterix;
	}
	else if (opPlus != std::string::npos) {
		return opPlus;
	}
	else if (opMinus != std::string::npos) {
		return opMinus;
	}
	else if (opMinus != std::string::npos) {
		return opMinus;
	}
	else if (opDividing != std::string::npos) {
		return opDividing;
	}
	else if (opModulos != std::string::npos) {
		return opModulos;
	}

	return std::string::npos;
}

void Manager::incrementNestedForLoops(std::string &variableName) {
	++lengthNestedForLoops;
	*(nestedForLoops + lengthNestedForLoops) = variableName;
}

void Manager::decrementNestedForLoops() {
	--lengthNestedForLoops;
}

void Manager::gotoLoopHead() {
	endLoopIndex = tableIndex + 1;
	tableIndex = headLoopIndex;
	
	eraseVariablesFromStack();

	std::string value = scanner.getInstructionAt(headLoopIndex).second;
	std::string key = scanner.getInstructionAt(headLoopIndex).first;

	return table(key, value);
}

void Manager::endLoop() {
	std::string value = scanner.getInstructionAt(endLoopIndex).second;
	std::string key = scanner.getInstructionAt(endLoopIndex).first;

	decrementNestedForLoops();
	positionStackForScope = 0;
	tableIndex = endLoopIndex;
	variablesStack.pop_back();

	return table(key, value);
}

std::string Manager::getDatatypeAsString(int datatype) {
	if (datatype == 1) {
		return "INT";
	}
	else if (datatype == 2) {
		return "FLOAT";
	}
	else if (datatype == 3) {
		return "STR";
	}

	return "";
}

void Manager::eraseVariablesFromStack() {
	for (int i = variablesStack.size(); i != positionStackForScope; i--) {
		variablesStack.pop_back();
	}
}

int main() {

	Manager manager;
	manager.init();

	system("pause");
	return 0;
}