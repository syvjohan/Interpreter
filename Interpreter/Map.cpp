#include "Map.h"

Map::Map(ErrorHandler *errHandler) {
	this->errHandler = *errHandler;

	elementNumber = 0;
	capacity = 100;
	map = DBG_NEW Pair[capacity];
}

//copy constructor
Map::Map(const Map& obj) {
	map = DBG_NEW Pair;
	*map = *obj.map;
}

Map::~Map() {
	if (map != nullptr) {
		delete[] map;
		map = NULL;
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
	errHandler.updateLog("ERROR: 026");
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

void Map::rezise() {
	int newCapacity = capacity + 100;
	Pair *tmpMap = DBG_NEW Pair[newCapacity];

	memcpy(tmpMap, map, capacity);

	map = tmpMap;
	capacity = newCapacity;
	delete[] tmpMap;
	tmpMap = NULL;
}

int Map::length() {
	return elementNumber;
}

bool Map::isLinenumberUnique(int linenumber) {
	for (int i = 0; i != capacity; i++) {
		if (linenumber == map[i].linenumber)
			errHandler.updateLog("ERROR: 025");
	}

	return true;
}