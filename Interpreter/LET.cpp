#include "LET.h"

LET::LET() {}

LET::LET(std::string &expression) {
	if (expression != "") {
		identifyPartsInExpression(expression);
		transformValueAfterDatatype();
	}
}

LET::~LET() {}

void LET::setName(std::string name) {
	this->name = name;
}

void LET::setValue(std::string value) {
	this->parsedValue = value;
}

void LET::setDataType(int datatype) {
	if (datatype == 2) {
		this->datatype = 2;
	}
	else if (datatype == 1) {
		this->datatype = 1;
	}
	else if (datatype == 3) {
		this->datatype = 3;
	}
	else {
		this->datatype == 1;
	}
}

std::string LET::getName() {
	return this->name;
}

std::string LET::getValue() {
	return parsedValue;
}

int LET::getDatatype() {
	return datatype;
}

void LET::identifyPartsInExpression(std::string &expression) {
	for (int i = 0; i != expression.length(); i++) {

		//find datatype. DEFINED DATATYPES
		bool hasDefinedDatatype = false;
		size_t typeInt = expression.find("INT");
		size_t typeFloat = expression.find("FLOAT");
		size_t typeString = expression.find("STR");
		if (typeInt != std::string::npos) {
			setDataType(1); //INT
			expression.erase(typeInt, typeInt + 3);
			hasDefinedDatatype = true;
		}
		else if (typeFloat != std::string::npos) {
			setDataType(2); //FLOAT
			expression.erase(typeFloat, typeFloat + 5);
			hasDefinedDatatype = true;
		}
		else if (typeString != std::string::npos) {
			setDataType(3); //STRING
			expression.erase(typeString, typeString + 3);
			hasDefinedDatatype = true;
		}
		else {
			setDataType(1); //INT
		}

		//find name.
		size_t opEqual = expression.find_first_of('=');
		if (opEqual != std::string::npos) {
			setName(expression.substr(0, opEqual));
			expression.erase(0, opEqual + 1);
		}
		else {
			setName(expression);
		}

		//find value and find UNDEFINED DATATYPES.
		if (opEqual != std::string::npos && opEqual + 1 != expression.length()) {
			size_t isFloat = expression.find_first_not_of(".");
			bool isAlpha = std::regex_match(expression, std::regex("^[A-Za-z]+$"));
			if (isFloat && !hasDefinedDatatype) {
				setDataType(2); //FLOAT
			}
			else if (isAlpha && !hasDefinedDatatype) {
				setDataType(3); //STRING
			}

			expression = transformKeywordsToValues(expression);
			expression = subdivideValue(expression);
			if (expression != "") { 
				setValue(expression); 
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

std::string& LET::subdivideValue(std::string &expression) {	
	std::string subExpression = "";
	int nextOp = expression.length();
	std::string restInsideParanthes = "";

	size_t openParanthes = expression.find_first_of('(');
	size_t closeParanthes = expression.find_first_of(')');
	//Matching paranthesis
	if (openParanthes != std::string::npos && closeParanthes != std::string::npos) {
		subExpression = expression.substr(openParanthes + 1, (closeParanthes - 1) - openParanthes);

		std::string restAfterParanthesis = "";
		// check if there is some char after close paranthes.
		if (subExpression.length() != closeParanthes) {
			restAfterParanthesis = expression.substr(closeParanthes + 1, expression.length() - closeParanthes); 
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
				expression = "";
				expression.append(tmpResult);
				expression.append(restInsideParanthes);
				expression.append(restAfterParanthesis);
				if (countOp > 1) {
					expression.insert(tmpResult.length() + restInsideParanthes.length(), ")");
					expression.insert(0, "(");
				}

				return subdivideValue(expression);
			}
		}
	}
	//Missing parantheses!
	else if (openParanthes == std::string::npos && closeParanthes == std::string::npos) {
		for (int k = 0; k != expression.length(); k++) {
			if (isOperator(expression[k])) {

				//find next operator.
				for (int n = k + 1; n != expression.length(); n++) {
					if (isOperator(expression[n])) {
						nextOp = n;
					}
				}

				//find right value.
				size_t v1 = nextOp;
				std::string rhs = expression.substr(k + 1, v1 - k);
				float rNumber = atof(rhs.c_str());

				//find left value.
				std::string lhs = expression.substr(0, k);
				float lNumber = atof(lhs.c_str());

				// find rest.
				if (nextOp != expression.length()) {
					restInsideParanthes = expression.substr(nextOp - 1, expression.length() - nextOp + 1);
				}
					
				//calculate the values
				float result = doCalc(lNumber, validateOperator(expression.at(k)), rNumber);
				std::string tmpResult = std::to_string(result);

				// create the new expression.
				expression = "";
				expression.append(tmpResult);
				expression.append(restInsideParanthes);

				//Check if there is more to calculate.
				for (int op = 0; op != expression.length(); op++) {
					if (isOperator(expression[op])) {
						subdivideValue(expression);
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
	 return expression;
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
std::string LET::transformKeywordsToValues(std::string &str) {

	size_t foundRandom = str.find("RANDOM");
	if (foundRandom != std::string::npos) {
		std::string random = std::to_string(generateRandomNumber());
		str.erase(foundRandom, 6);
		str.insert(foundRandom, random);
	}

	size_t foundPI = str.find("PI");
	if(foundPI != std::string::npos) {
		std::string pi = std::to_string(3.14159265359);
		str.erase(foundPI, 2);
		str.insert(foundPI, pi);
	}
	
	return str;
}

void LET::setDefaultValue() {
	std::string value = "";
	if (datatype == 1) {
		setValue(value = "0");
	}
	else if (datatype == 2) {
		setValue(value = "0.0f");
	}
	else if (datatype == 3) {
		setValue("");
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