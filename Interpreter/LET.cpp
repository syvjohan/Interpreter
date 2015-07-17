#include "LET.h"

LET::LET() {
	data = DBG_NEW LetData();
	initializeValues();
}

LET::LET(const std::string &expression) : LET() {
	if (expression != "") {
		identifyPartsInExpression(expression);
		transformValueAfterDatatype();
	}
}

//copy constructor.
LET::LET(const LET &obj) {
	data = DBG_NEW LetData;
	*data = *obj.data;
}

LET::~LET() {
	if (data != nullptr) {
		delete data;
		data = NULL;
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
		data->datatype == 1;
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
	for (int i = 0; i != expr.length(); i++) {

		//find datatype. DEFINED DATATYPES
		bool hasDefinedDatatype = false;
		size_t typeInt = expr.find("INT");
		size_t typeFloat = expr.find("FLOAT");
		size_t typeString = expr.find("STR");
		int datatype = 0;
		if (typeInt != std::string::npos) {
			setDataType(datatype = 1); //INT
			expr.erase(typeInt, typeInt + 3);
			hasDefinedDatatype = true;
		}
		else if (typeFloat != std::string::npos) {
			setDataType(datatype = 2); //FLOAT
			expr.erase(typeFloat, typeFloat + 5);
			hasDefinedDatatype = true;
		}
		else if (typeString != std::string::npos) {
			setDataType(datatype = 3); //STRING
			expr.erase(typeString, typeString + 3);
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
	std::string restInsideParanthes = "";

	size_t openParanthes = expr.find_first_of('(');
	size_t closeParanthes = expr.find_first_of(')');
	//Matching paranthesis
	if (openParanthes != std::string::npos && closeParanthes != std::string::npos) {
		subExpression = expr.substr(openParanthes + 1, (closeParanthes - 1) - openParanthes);

		std::string restAfterParanthesis = "";
		// check if there is some char after close paranthes.
		if (subExpression.length() != closeParanthes) {
			restAfterParanthesis = expr.substr(closeParanthes + 1, expr.length() - closeParanthes);
		}
		
		//check number of operators.
		int countOp = 0;
		for (int op = 0; op != subExpression.length(); op++) {
			if (isOperator(subExpression[op])) {
				++countOp;
			}
		}

		for (int k = 0; k != subExpression.length(); k++) {
			if (isOperator(subExpression[k])) {

				//find next operator.
				for (int n = k + 1; n != subExpression.length(); n++) {
					if (isOperator(subExpression[n])) {
						nextOp = n;
						break;
					}

					nextOp = subExpression.length();
				}

				//find right value.
				size_t v1 = subExpression.length();
				std::string rhs = subExpression.substr(k + 1, v1 - k);
				float rNumber = atof(rhs.c_str());

				//find left value.
				std::string lhs = subExpression.substr(0, k);
				float lNumber = atof(lhs.c_str());

				// find rest.
				if (nextOp != subExpression.length()) {
					restInsideParanthes = subExpression.substr(nextOp, subExpression.length() - nextOp + 1);
				}

				//calculate the values
				float result = doCalc(lNumber, validateOperator(subExpression.at(k)), rNumber);
				std::string tmpResult = std::to_string(result);

				// create the new expression.
				expr = "";
				expr.append(tmpResult);
				expr.append(restInsideParanthes);
				expr.append(restAfterParanthesis);
				if (countOp > 1) {
					expr.insert(tmpResult.length() + restInsideParanthes.length(), ")");
					expr.insert(0, "(");
				}

				return subdivideValue(expr);
			}
		}
	}
	//Missing parantheses!
	else if (openParanthes == std::string::npos && closeParanthes == std::string::npos) {
		for (int k = 0; k != expr.length(); k++) {
			if (isOperator(expr[k])) {

				//find next operator.
				for (int n = k + 1; n != expr.length(); n++) {
					if (isOperator(expr[n])) {
						nextOp = n;
					}
				}

				//find right value.
				size_t v1 = nextOp;
				std::string rhs = expr.substr(k + 1, v1 - k);
				float rNumber = atof(rhs.c_str());

				//find left value.
				std::string lhs = expr.substr(0, k);
				float lNumber = atof(lhs.c_str());

				// find rest.
				if (nextOp != expr.length()) {
					restInsideParanthes = expr.substr(nextOp - 1, expr.length() - nextOp + 1);
				}
					
				//calculate the values
				float result = doCalc(lNumber, validateOperator(expr.at(k)), rNumber);
				std::string tmpResult = std::to_string(result);

				// create the new expression.
				expr = "";
				expr.append(tmpResult);
				expr.append(restInsideParanthes);

				//Check if there is more to calculate.
				for (int op = 0; op != expr.length(); op++) {
					if (isOperator(expr[op])) {
						subdivideValue(expr);
					}
				}
				break;
			}
		}
	}
	else {
		//paranthesis mismatch!
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

float LET::doCalc(float value1, char op, float value2) {
	float result = 0.0f;
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

	return result;
}

std::string LET::sortStringForward(std::string str) {
	std::string tmp;
	for (int i = str.length() -1; i >= 0; i--) {
		tmp += str[i];
	}
	str = tmp;
	return str;
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
			setValue(value.substr(0, findDot + 2));
		}
	}
}