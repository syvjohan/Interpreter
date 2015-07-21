#include "ErrorHandler.h"

ErrorHandler::ErrorHandler()
{
}

ErrorHandler::~ErrorHandler()
{
}

void ErrorHandler::init() {
	fillVectorWithMsg("instructions/errorMessages.txt");
	eraseContentInLog();
}

void ErrorHandler::fillVectorWithMsg(const std::string path) {
	if (path == "") return updateLog("ERROR: 024");
	std::ifstream file(path);
	if (file.is_open()) {
		for (std::string line = ""; std::getline(file, line);) {
			if (line.length() > 12) {
				std::string code = line.substr(0, 11);
				std::string msg = line.substr(12, line.length() - 10);
				containerErrMsg.push_back(std::make_pair(code, msg));
			}
		}
	}
}

void ErrorHandler::updateLog(const std::string errCode) {
	std::ofstream file;
	file.open("instructions/errorLog.txt");

	for (int i = 0; i != containerErrMsg.size(); i++) {
		if (errCode == containerErrMsg[i].first) {
			file << errCode << " on line: " << getLineNumber() << ". error message: " <<
				containerErrMsg[i].second << "\n";
			break;
		}
	}

	file.close();
	exit(0);
}

void ErrorHandler::eraseContentInLog() {
	std::ofstream file;
	file.open("instructions/errorLog.txt", std::ofstream::out | std::ofstream::trunc);
	file.close();
}

std::string ErrorHandler::getLineNumber() const {
	return lineNumber;
}

void ErrorHandler::setLineNumber(std::string lineNumber) {
	this->lineNumber = lineNumber;
}