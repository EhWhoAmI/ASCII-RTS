#pragma once

struct Point {
	int x;
	int y;

	Point(int _x, int _y) {
		x = _x;
		y = _y;
	}

	Point() {
		x = 0;
		y = 0;
	}
};

struct Tile {
public:
	char character;
	TCODColor foreground;
	TCODColor background;
	bool invisible;
	bool walkable;
};

struct Map {
public:
	Tile* tiles;
	TCODMap *map;

	int width;
	int height;
	Map(int _width, int _height) {
		width = _width;
		height = _height;
		tiles = new Tile[width * height];
		map = new TCODMap(_width, _height);
	}

	void setTile(int x, int y, Tile t) {
		tiles[x + y * width] = t;
	}

	Tile getTile(int x, int y) {
		return tiles[x + y * width];
	}
};