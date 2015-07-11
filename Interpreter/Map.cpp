#include "Map.h"

Map::Map() {
	elementNumber = 0;
	capacity = 100;
	map = new Pair[capacity];
}

//copy constructor
Map::Map(const Map& obj) {
	map = new Pair;
	*map = *obj.map;
}

Map::~Map() {
	if (map != nullptr) {
		delete[] map;
	}
}

bool Map::pushBack(int linenumber, std::string value) {
	if ((linenumber != NULL) && (value != "") && (isLinenumberUnique(linenumber))) {
		if (elementNumber + 1 == capacity) {
			rezise();
		}
		Pair pair;
		pair.linenumber = linenumber;
		pair.value = value;
		map[elementNumber] = pair;
		++elementNumber;
		return true;
	}

	return false;
}

Map::Pair Map::getElementAt(int linenumber) {
	if (elementNumber > 0) {		
		Pair pair;
		pair.value = map[linenumber].value;
		pair.linenumber = map[linenumber].linenumber;

		return pair;
	}
}

Map::Pair* Map::getAllElements() {
	Pair *container = new Pair[elementNumber];
	for (int i = 0; i <= elementNumber; i++) {
		Pair pair;
		pair.value = map[i].value;
		container[i] = pair;
	}
	return container;
}

void Map::rezise() {
	int newCapacity = capacity + 100;
	Pair *tmpMap = new Pair[newCapacity];

	memcpy(tmpMap, map, capacity);

	map = tmpMap;
	capacity = newCapacity;
	delete[] tmpMap;
}

int Map::length() {
	return elementNumber;
}

bool Map::isLinenumberUnique(int linenumber) {
	for (int i = 0; i != capacity; i++) {
		if (linenumber == map[i].linenumber)
			return false;
	}

	return true;
}