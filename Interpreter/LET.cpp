#include "LET.h"

LET::LET() {
	initializeValues();
}

LET::LET(const std::string &expression, ErrorHandler *errHandler) : LET() {
	if (expression != "") {
		this->errHandler = *errHandler;

		identifyPartsInExpression(expression);
		transformValueAfterDatatype();
		return;
	}
	this->errHandler.updateLog("ERROR: 027");
}

//copy constructor.
LET::LET(const LET &obj) {
	data = DBG_NEW LetData;
	*data = *obj.data;
}

LET::~LET() {
	if (data != nullptr) {
		//delete data;
		//data = NULL;
	}
}

void LET::initializeValues() {
	data = DBG_NEW LetData();

	data->name = "";
	data->parsedValue = "";
	data->datatype = 0;
}

void LET::setName(const std::string &name) {
	data->name = name;
}

void LET::setValue(const std::string &value) {
	data->parsedValue = value;
}

void LET::setDataType(const int &datatype) {
	if (datatype == 2) {
		data->datatype = 2;
	}
	else if (datatype == 1) {
		data->datatype = 1;
	}
	else if (datatype == 3) {
		data->datatype = 3;
	}
	else {
		data->datatype = 1;
	}
}

std::string LET::getName() const {
	return data->name;
}

std::string LET::getValue() const {
	return data->parsedValue;
}

int LET::getDatatype() const {
	return data->datatype;
}

void LET::identifyPartsInExpression(const std::string &expression) {
	std::string expr = expression;
	expr = trimString(expr);
	for (int i = 0; i != expr.length(); i++) {

		//find datatype. DEFINED DATATYPES
		bool hasDefinedDatatype = false;
		size_t typeInt = expr.find("INT");
		size_t typeFloat = expr.find("DEC");
		size_t typeString = expr.find("STR");
		int datatype = 0;
		if (typeInt != std::string::npos) {
			setDataType(datatype = 1); //INT
			expr.erase(typeInt, 3);
			hasDefinedDatatype = true;
		}
		else if (typeFloat != std::string::npos) {
			setDataType(datatype = 2); //FLOAT
			expr.erase(typeFloat, 3);
			hasDefinedDatatype = true;
		}
		else if (typeString != std::string::npos) {
			setDataType(datatype = 3); //STRING
			expr.erase(typeString, 3);
			hasDefinedDatatype = true;
		}
		else {
			setDataType(datatype = 1); //INT
		}

		//find name.
		size_t opEqual = expr.find_first_of('=');
		if (opEqual != std::string::npos) {
			setName(expr.substr(0, opEqual));
			expr.erase(0, opEqual + 1);
		}
		else {
			setName(expr);
		}

		//find value and find UNDEFINED DATATYPES.
		if (opEqual != std::string::npos && opEqual + 1 != expr.length()) {
			size_t isFloat = expr.find_first_not_of(".");
			bool isAlpha = std::regex_match(expr, std::regex("^[A-Za-z]+$"));
			if (isFloat && !hasDefinedDatatype) {
				setDataType(datatype = 2); //FLOAT
			}
			else if (isAlpha && !hasDefinedDatatype) {
				setDataType(datatype = 3); //STRING
			}

			expr = transformKeywordsToValues(expr);
			expr = subdivideValue(expr);
			if (expr != "") {
				setValue(expr);
			}
			else {
				//Syntax wrong in expression.
				errHandler.updateLog("ERROR: 018");
			}
		}
		else {
			//No value found set default value.
			setDefaultValue();
		}

		return;
	}
}

std::string LET::subdivideValue(const std::string &expression) {	
	std::string expr = expression;
	std::string subExpression = "";
	int nextOp = expr.length();
	int beforeOp = 0;
	std::string restRight = "";
	std::string restLeft = "";

	size_t openParanthes = expr.find_first_of('(');
	size_t closeParanthes = expr.find_first_of(')');
	//Matching paranthesis
	if (openParanthes != std::string::npos && closeParanthes != std::string::npos) {
		std::string restAfterParanthesis = "";
		std::string restBeforeParanthesis = "";

		// check if there is some char after close paranthes.
		if (expr.length() != closeParanthes) {
			restAfterParanthesis = expr.substr(closeParanthes + 1, expr.length() - closeParanthes);
		}
		
		//check if there is some char before open paranthes.
		if (expr[0] != openParanthes) {
			restBeforeParanthesis = expr.substr(0, openParanthes);
		}

		int priorities = 1;
		while (priorities != 3 || priorities < 0) {
			for (int k = openParanthes; k != expr.length(); k++) {
 				if (prioritiesOperator(expr[k], priorities)) {
					//find next operator.
					for (int n = k + 1; n != expr.length(); n++) {
						if (isOperator(expr[n]) || isParanthesis(expr[n])) {
							nextOp = n;
							break;
						}
					}

					//find before operator
					for (int b = k - 1; b != 0; b--) {
						if (isOperator(expr[b]) || isParanthesis(expr[b])) {
							beforeOp = b;
							break;
						}
					}

					std::string rhs = expr.substr(k + 1, nextOp - k - 1); //find right value.
					std::string lhs = expr.substr(beforeOp + 1, k - beforeOp - 1); //find left value.

					if (!isSameDatatype(rhs, lhs)) { errHandler.updateLog("ERROR: 000"); }
					std::string result = doCalc(rhs, validateOperator(expr.at(k)), lhs);
					

					// find rest right.
					if (closeParanthes == nextOp) {
						restRight = "";
					}
					else if (nextOp != expr.length()) {
						restRight = expr.substr(nextOp, expr.length() - nextOp -1);
					}

					//find rest left
					if (openParanthes == beforeOp) {
						restLeft = "";
					}
					else if (beforeOp != 0) {
						restLeft = expr.substr(openParanthes +1, beforeOp - openParanthes);
					}
					else {
						restLeft = expr.substr(openParanthes, beforeOp);
					}

					// create the new expression.
					expr = "";
					expr.append(restBeforeParanthesis);
					expr.append(restLeft);
					expr.append(result);
					expr.append(restRight);
					expr.append(restAfterParanthesis);

					if (expr == "") {
						errHandler.updateLog("ERROR: 019");
					}

					return subdivideValue(expr);
				}
			}
			--priorities;
		}
	}
	//Missing parantheses!
	else if (openParanthes == std::string::npos && closeParanthes == std::string::npos) {
		int priorities = 1;
		while (priorities != 3) {
			for (int k = 0; k != expr.length(); k++) {
				if (prioritiesOperator(expr[k], priorities)) {

					//find next operator.
					for (int n = k + 1; n != expr.length(); n++) {
						if (isOperator(expr[n])) {
							nextOp = n;
							break;
						}
					}

					//find before operator
					for (int b = k -1; b != 0; b--) {
						if (isOperator(expr[b])) {
							beforeOp = b;
							break;
						}
					}

					int v1 = nextOp;
					std::string rhs = expr.substr(k + 1, v1 - k -1); //find right value.
					std::string lhs = expr.substr(beforeOp, k - beforeOp); //find left value.

					if (!isSameDatatype(rhs, lhs)) { errHandler.updateLog("ERROR: 000"); }

					std::string result = doCalc(rhs, validateOperator(expr.at(k)), lhs); //calculate the values

					// find rest right.
					if (nextOp != expr.length()) {
						restRight = expr.substr(nextOp - 1, expr.length() - nextOp + 1);
					}

					//find rest left
					if (beforeOp != 0) {
						restLeft = expr.substr(0, beforeOp +1);
					}
					else {
						restLeft = expr.substr(0, beforeOp);
					}

					// create the new expression.
					expr = "";
					expr.append(restLeft);
					expr.append(result);
					expr.append(restRight);

					//Check if there is more to calculate.
 					for (int op = 0; op != expr.length(); op++) {
						if (isOperator(expr[op])) {
							subdivideValue(expr);
						}
					}
					break;
				}
			}
			++priorities;
		}
	}
	else {
		//paranthesis mismatch!
	}

	if (expr == "") {
		errHandler.updateLog("ERROR: 019");
	}

	 //Invalid expression
	return expr;
}

char LET::validateOperator(char op) {
	if (op == '+' || op == '-' || op == '/' || op == '*') {
		return op;
	}
	return ' ';
}

bool LET::isOperator(char op) {
	return (op == '+' || op == '-' || op == '/' || op == '*');
}

int LET::validateOperatorType(char op) {
	if (op == '*' || op == '/') {
		return 1;
	}
	else if (op == '+' || op == '-') {
		return 2;
	}
	return 3;
}

bool LET::isParanthesis(char op) {
	return (op == '(' || op == ')');
}

float LET::generateRandomNumber() {
	return (rand() / (float)(RAND_MAX + 1));
}

std::string LET::doCalc(const std::string &str2, const char &op, const std::string &str1) {
	//check if it is a string str
	bool rhsIsAlpha = std::regex_match(str1, std::regex("^[A-Za-z]+$"));
	bool lhsIsAlpha = std::regex_match(str2, std::regex("^[A-Za-z]+$"));
	std::string resultStr = "";
	if (rhsIsAlpha && lhsIsAlpha && str1 != "" && str2 != "") {
		switch (op) {
		case '*':
			resultStr = "";
			break;
		case '/':
			resultStr = "";
			break;
		case '+':
			resultStr = str1 + str2;
			break;
		case '-':
			resultStr = "";
			break;
		}
	}
	else {
		float value1 = atof(str1.c_str());
		float value2 = atof(str2.c_str());

		float result = 0.0f;
		if (value1 == 0) return std::to_string(value2);
		if (value2 == 0) return std::to_string(value1);
		switch (op) {
		case '*':
			result = value1 * value2;
			break;
		case '/':
			result = value1 / value2;
			break;
		case '+':
			result = value1 + value2;
			break;
		case '-':
			result = value1 - value2;
			break;
		}

		resultStr = std::to_string(result);
		size_t findNegValue = resultStr.find('-');
		if (findNegValue != std::string::npos) {
			errHandler.updateLog("ERROR: 004");
		}
	}

	return resultStr;
}

bool LET::isNumber(const std::string str) {
	return str.find_first_not_of("0123456789.") == std::string::npos;
}

//Exchange words to values, ex: PI, RANDOM.
std::string LET::transformKeywordsToValues(const std::string &str) {
	std::string expr = str;

	bool onlyDigits = isNumber(expr);
	if (onlyDigits == true) { return expr; };

	size_t foundRandom = str.find("RANDOM");
	if (foundRandom != std::string::npos) {
		std::string random = std::to_string(generateRandomNumber());
		expr.erase(foundRandom, 6);
		expr.insert(foundRandom, random);
	}

	size_t foundPI = str.find("PI");
	if(foundPI != std::string::npos) {
		std::string pi = std::to_string(3.14159265359);
		expr.erase(foundPI, 2);
		expr.insert(foundPI, pi);
	}
	
	return expr;
}

void LET::setDefaultValue() {
	std::string value = "";
	if (data->datatype == 1) {
		setValue(value = "0");
	}
	else if (data->datatype == 2) {
		setValue(value = "0.0f");
	}
	else if (data->datatype == 3) {
		setValue(value = "");
	}
}

void LET::transformValueAfterDatatype() {
	std::string value = getValue();
	size_t findDot = value.find('.');
	if (findDot != std::string::npos) {
		if (getDatatype() == 1) {
			setValue(value.substr(0, findDot));
		}
		else if (getDatatype() == 2) {
			setValue(value.substr(0, findDot + 4));
		}
	}
}

std::string LET::trimString(std::string &str) {
	std::string tmp = "";
	for (int i = 0; i != str.length(); i++) {
		if (str.at(i) != ' ' && str.at(i) != '\t' && str.at(i) != '\n' && str.at(i) != '\v' && str.at(i) != '\"') {
			tmp += str.at(i);
		}
	}
	str = tmp;
	return str;
}

bool LET::prioritiesOperator(char op, int priority) {
	if (priority == 1) {
		return op == '*' || op == '/';
	}
	else if (priority == 2) {
		return op == '+' || op == '-';
	}

	return false;
}

bool LET::isSameDatatype(const std::string str1, const std::string str2) {
	size_t isDigit1 = str1.find_first_not_of(".0123456789");
	bool isAlpha1 = std::regex_match(str1, std::regex("^[A-Za-z]+$"));

	size_t isDigit2 = str2.find_first_not_of(".0123456789");
	bool isAlpha2 = std::regex_match(str2, std::regex("^[A-Za-z]+$"));

	if (isDigit1 && isDigit2) {
		return true;
	}
	else if (isAlpha1 && isAlpha2) {
		return true;
	}

	return false;
}
