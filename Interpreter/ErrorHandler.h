#pragma once

#include "Defs.h"

#include <vector>
#include <string>
#include <fstream>

class ErrorHandler
{
public:
	ErrorHandler();
	~ErrorHandler();
	void init();
	void updateLog(const std::string errCode);
	void setLineNumber(std::string lineNumber);

private:
	std::vector<std::pair<std::string, std::string>> containerErrMsg;
	std::vector<std::string> log;
	std::string lineNumber;

	void fillVectorWithMsg(const std::string path);
	void eraseContentInLog();
	std::string getLineNumber() const;
};

