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

bool Map::pushBack(int key, std::string value) {
	if ((key != NULL) && (value != "") && (isKeyUnique(key))) {
		if (elementNumber + 1 == capacity) {
			rezise();
		}
		Pair pair;
		pair.key = key;
		pair.value = value;
		map[elementNumber] = pair;
		++elementNumber;
		return true;
	}

	return false;
}

Map::Pair Map::getElementAt(int key) {
	if (elementNumber > 0) {		
		Pair pair;
		pair.key = map[key].key;
		pair.value = map[key].value;

		return pair;
	}
}

Map::Pair* Map::getAllElements() {
	Pair *container = new Pair[elementNumber];
	for (int i = 0; i <= elementNumber; i++) {
		Pair pair;
		pair.key = map[i].key;
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

bool Map::isKeyUnique(int key) {
	for (int i = 0; i != capacity; i++) {
		if (key == map[i].key)
			return false;
	}

	return true;
}