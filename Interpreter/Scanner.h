#pragma once

#include "Map.h"
#include "Defs.h"
#include "ErrorHandler.h"

#include <fstream>
#include <string>

//#pragma optimize("", on)

class Scanner {
public:
	Scanner();
	Scanner(ErrorHandler *errorHandler);
	~Scanner();
	
	void readFile(const std::string path);

	std::string getkeyword(std::string str);
	std::string getLinenumber(int index);
	int getIndex(int linenumber);
	std::pair<std::string, std::string> getInstructionAt(int index);
	int length();
	std::string trimPRINT(std::string str);
	bool isString(std::string str);

private:
	Map *map;
	ErrorHandler errHandler;

	int keywordsLength;
	std::string keywords[16];

	std::string trimString(std::string str);
	std::string removeFirstAndLastWhitesspace(std::string &str);
	std::string getExpression(std::string str, std::string strIrrelevant);
};

