#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

#include "Scanner.h"
#include "LET.h"
#include "Functions.h"
#include "Defs.h"

class Manager
{
public:
	Manager();
	~Manager();

	void init();

private:
	Scanner scanner;
	std::vector<LET> variablesHeap;
	std::vector<LET> variablesStack;
	int positionStackForScope;

	Functions *functions;
	Function *function;

	bool insideFunction;
	bool readLines;

	int tableIndex; 
	int headLoopIndex;
	int endLoopIndex;

	std::string *nestedForLoops;
	int lengthNestedForLoops;
	int capacityFoorLoops;

	std::string callingPoint;

	//Help Functions.
	std::string exchangeVariableNameToValue(const std::string expression);
	void overwriteOldVariable(const LET newVar);
	size_t getCompareOperatorPos(const std::string *expression);
	size_t getOperatorPos(const std::string *expression);
	std::string getDatatypeAsString(const int datatype);
	void incrementNestedForLoops(const std::string &variableName);
	void decrementNestedForLoops();
	void gotoLoopHead();
	void endLoop();
	void eraseVariablesFromStack();
	void addVariable(const LET &variable, const int memoryType);
	bool isAFunction(const std::string expression);
	void doFunction(const std::string &expression);
	void table(const std::string keyword, const std::string expression);

	//Evaluation
	void evalPRINT(const std::string &expression);
	void evalINPUT(const std::string &expression);
	void evalLET(const std::string &expression);
	void evalLET(const std::string &expression, const int datatype);
	void evalIF(const std::string &expression);
	void evalGOTO(const std::string &expression);
	void evalFOR(const std::string &expression);
	void evalNEXT(const std::string &expression);
	void evalDEFINE(const std::string &expression);
	void evalRETURN(const std::string &expression);
	void evalENDDEF();
};

