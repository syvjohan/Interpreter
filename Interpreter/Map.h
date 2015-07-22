#pragma once
#include "Defs.h"

#include <string>
#include <iostream>
#include "ErrorHandler.h"

class Map {
public:
	Map(ErrorHandler *errHandler);
	Map(const Map& obj); //copy constructor.
	~Map();

	struct Pair {
		int linenumber = 0;
		std::string value = "";
	};

	bool pushBack(int linenumber, std::string value);
	Pair getElementAt(int linenumber);
	int length();

private:
	ErrorHandler errHandler;
	int elementNumber;
	int capacity;
	Pair *map;
	

	bool isLinenumberUnique(int key);
	void rezise();
};