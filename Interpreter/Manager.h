#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

#include "Scanner.h"
#include "LET.h"
#include "Function.h"

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

	std::vector<Function> *functionHeap;
	int capacityFunction;
	int lengthfunctionHeap;
	bool insideFunction;

	int tableIndex; 
	int headLoopIndex;
	int endLoopIndex;

	std::string *nestedForLoops;
	int lengthNestedForLoops;
	int capacityFoorLoops;

	//Help functions.
	std::string exchangeVariableNameToValue(std::string expression);
	void overwriteOldVariableValue(LET *newVar);
	size_t getCompareOperatorPos(std::string *expression);
	size_t getOperatorPos(std::string *expression);
	std::string getDatatypeAsString(int datatype);
	void incrementNestedForLoops(std::string &variableName);
	void decrementNestedForLoops();
	void gotoLoopHead();
	void endLoop();
	void eraseVariablesFromStack();
	void addVariable(LET variable, int memoryType);
	void doFunction(std::string &expression);
	void table(std::string keyword, std::string expression);

	//Evaluation
	void evalPRINT(std::string &expression);
	void evalINPUT(std::string &expression);
	void evalLET(std::string &expression);
	void evalLET(std::string &expression, int datatype);
	void evalIF(std::string &expression);
	void evalGOTO(std::string &expression);
	void evalFOR(std::string &expression);
	void evalNEXT(std::string &expression);
	void evalDEFINE(std::string &expression);
	LET evalRETURN(std::string &expression);
	void evalENDDEF(std::string &expression);
};

