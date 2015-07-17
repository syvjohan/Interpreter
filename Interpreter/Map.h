#pragma once
#include "Defs.h"

#include <string>
#include <iostream>

class Map {
public:
	Map();
	Map(const Map& obj); //copy constructor.
	~Map();

	struct Pair {
		int linenumber;
		std::string value;
	};

	bool pushBack(int linenumber, std::string value);
	Pair getElementAt(int linenumber);
	Pair* getAllElements();
	int length();

private:
	int elementNumber;
	int capacity;
	Pair *map;
	

	bool isLinenumberUnique(int key);
	void rezise();
};