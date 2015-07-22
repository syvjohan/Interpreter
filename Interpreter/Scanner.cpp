#include "Scanner.h"

Scanner::Scanner() {
	keywordsLength = 16;
	keywords[0] = "PRINT";
	keywords[1] = "LET";
	keywords[2] = "INPUT";
	keywords[3] = "IF";
	keywords[4] = "GOTO";
	keywords[5] = "ENDDEF";
	keywords[6] = "FOR";
	keywords[7] = "THEN";
	keywords[8] = "TO";
	keywords[9] = "NEXT";
	keywords[10] = "INT";
	keywords[11] = "STR";
	keywords[12] = "DEC";
	keywords[13] = "DEFINE";
	keywords[14] = "END";
	keywords[15] = "RETURN";
}

Scanner::Scanner(ErrorHandler *errHandler) : Scanner() {
	this->errHandler = *errHandler;
	map = DBG_NEW Map(errHandler);
}

Scanner::~Scanner()
{
	if (map != nullptr) {
		delete[] map;
	}
}

void Scanner::readFile(const std::string path) {
	if (path == "") return errHandler.updateLog("ERROR: 024");
	std::ifstream file(path);
	if (file.is_open()) {

		for (std::string line = ""; getline(file, line);) {
			int firstWhiteSpace = line.find_first_of(' ');
			int linenumber = stoi(line.substr(0, firstWhiteSpace));

			std::string value = line.substr(firstWhiteSpace, line.length() - firstWhiteSpace);

			size_t findDatatype1 = value.find("INT");
			size_t findDatatype2 = value.find("DEC");
			size_t findDatatype3 = value.find("STR");

			//find PRINT
			size_t findPRINT = value.find("PRINT");
			if (findPRINT == std::string::npos && findDatatype1 == std::string::npos &&
				findDatatype2 == std::string::npos && findDatatype3 == std::string::npos) {
				value = trimString(value);
			}
			else if (findDatatype1 != std::string::npos &&
				findDatatype2 != std::string::npos && findDatatype3 != std::string::npos) {
				value = trimString(value);
			}

			value = removeFirstAndLastWhitesspace(value);

			map->pushBack(linenumber, value);
		}
	}
}

std::string Scanner::removeFirstAndLastWhitesspace(std::string &str) {
	//erase first whitespace
	while (isspace(str.front())) {
		str.erase(0, 1);
	}
	//erase last whitespace
	while (isspace(str.back())) {
		str.erase(str.length() - 1, 1);
	}

	return str;
}

std::string Scanner::trimString(std::string str) {
	std::string tmp = "";
	for (int i = 0; i != str.length(); i++) {
		if (str.at(i) != ' ' && str.at(i) != '\t' && str.at(i) != '\n' && str.at(i) != '\v' && str.at(i) != '\"') {
			tmp += str.at(i);
		}
	}
	str = tmp;
	return str;
}

std::string Scanner::trimPRINT(std::string str) {
	str = removeFirstAndLastWhitesspace(str);

	// if it is text.
	size_t foundQuote = str.find_first_of('\"');
	size_t foundQuote2 = str.find_last_of('\"');
	if (foundQuote != std::string::npos && foundQuote2 != std::string::npos) {
		str.erase(foundQuote, 1);
		foundQuote2 = str.find_last_of('\"');
		str.erase(foundQuote2, 1);

		//if there is whitespace after quote
		for (int i = foundQuote2; i <= str.length(); i++) {
			if (isspace(str[i])) {
				str.erase(i, 1);
			}
		}
	}
	
	//if it is a variable
	else {
		for (int i = 0; i <= str.length(); i++) {
			if (isspace(str[i])) {
				str.erase(i, 1);
			}
		}
	}

	return str;
}

bool Scanner::isString(std::string str) {
	size_t foundQuote = str.find_first_of('\"');
	size_t foundQuote2 = str.find_last_of('\"');
	if (foundQuote && foundQuote2 != std::string::npos) {
		return true;
	}

	return false;
}

std::string Scanner::getkeyword(std::string str) {
for (int i = 0; i != keywordsLength; i++) {
		size_t found = str.find(keywords[i]);
		if (found != std::string::npos) {
			return keywords[i];
		}
	}

	return "";
}

std::string Scanner::getExpression(std::string str, std::string strIrrelevant) {
	return str.substr(strIrrelevant.length(), (str.length() - strIrrelevant.length()));
}

std::pair<std::string, std::string> Scanner::getInstructionAt(int index) {
	std::string value = map->getElementAt(index).value;
	std::string keyword = getkeyword(value);
	std::string expression = getExpression(value, keyword);
	return std::make_pair(keyword, expression);
}

std::string Scanner::getLinenumber(int index) {
	return std::to_string(map->getElementAt(index).linenumber);
}

int Scanner::getIndex(int linenumber) {
	for (int i = 0; i != length(); i++) {
		if (map->getElementAt(i).linenumber == linenumber) {
			return i;
		}
	}

	return -1;
}

int Scanner::length() {
	return map->length();
}