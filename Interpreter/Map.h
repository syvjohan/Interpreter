#pragma once

#include <string>
#include <iostream>

class Map {
public:
	Map();
	Map(const Map& obj); //copy constructor.
	~Map();

	struct Pair {
		int key;
		std::string value;
	};

	bool pushBack(int key, std::string value);
	Pair getElementAt(int key);
	Pair* getAllElements();
	int length();

private:
	int elementNumber;
	int capacity;
	Pair *map;
	

	bool isKeyUnique(int key);
	void rezise();
};