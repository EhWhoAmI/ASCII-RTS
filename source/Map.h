#pragma once

#include <libtcod.hpp>

#define MAX_ROOM_SIZE 15
#define MIN_ROOM_SIZE 10

class Map;
class BspListener;


struct Point {
	double x;
	double y;

	Point(double _x, double _y) {
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
	bool transparent;
	bool walkable = false;
	bool visible = false;
};

class Map {
public:

	Tile* tiles;
	TCODMap* map;

	int width;
	int height;

	int spawnX;
	int spawnY;

	std::vector<Point> enemytospawn;

	Map(int _width, int _height) {
		width = _width;
		height = _height;
		tiles = new Tile[width * height];
		map = new TCODMap(_width, _height);
		for (int tilex = 0; tilex < width; tilex++) {
			for (int tiley = 0; tiley < height; tiley++) {
				Tile t;
				t.walkable = false;
				t.transparent = false;
				t.character = '#';
				t.background = TCODColor::orange;
				t.foreground = TCODColor::red;
				setTile(tilex, tiley, t);
			}
		}
	}

	void setTile(int x, int y, Tile t) {
		tiles[x + y * width] = t;
		map->setProperties(x, y, t.transparent, t.walkable);
	}

	Tile getTile(int x, int y) {
		return tiles[x + y * width];
	}

	void dig(int x1, int y1, int x2, int y2) {
		if (x2 < x1) {
			int tmp = x2;
			x2 = x1;
			x1 = tmp;
		}
		if (y2 < y1) {
			int tmp = y2;
			y2 = y1;
			y1 = tmp;
		}
		TCODRandom* rng = TCODRandom::getInstance();
		for (int tilex = x1; tilex <= x2; tilex++) {
			for (int tiley = y1; tiley <= y2; tiley++) {
				Tile t;
				t.walkable = true;
				t.transparent = true;
				if (!rng->getInt(0, 3)) {
					t.character = '.';
				}
				else {
					t.character = ' ';
				}
				
				t.background = TCODColor::black;
				t.foreground = TCODColor::darkerGreen;
				setTile(tilex, tiley, t);
			}
		}
	}

	void createRoom(bool first, int x1, int y1, int x2, int y2) {
		dig(x1, y1, x2, y2);
		if (first) {
			//Then spawn players
			spawnX = (x1 + x2) / 2;
			spawnY = (y1 + y2) / 2;
		}
		else {
			//Spawn enemies
			enemytospawn.push_back(Point((x1 + x2) / 2, (y1 + y2) / 2));
		}
	}
};


class BspListener : public ITCODBspCallback {
	Map& map; // a map to dig
	int roomNum; // room number
	int lastx = 0, lasty = 0; // center of the last room
public:
	BspListener(Map& _map) : map(_map), roomNum(0) {}

	bool visitNode(TCODBsp* node, void* userData) {
		if (node->isLeaf()) {
			int x, y, w, h;
			// dig a room
			TCODRandom* rng = TCODRandom::getInstance();
			w = rng->getInt(MIN_ROOM_SIZE, node->w - 2);
			h = rng->getInt(MIN_ROOM_SIZE, node->h - 2);
			x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 1, x, y, x + w - 1, y + h - 1);
			if (roomNum != 1) {
				// dig a corridor from last room
				map.dig(lastx, lasty, x + w / 2, lasty);
				map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
			}
			lastx = x + w / 2;
			lasty = y + h / 2;
			roomNum++;
		}
		return true;
	}
};

#define ACTION_MOVE 0
#define ACTION_ATTACK 1


struct Action {
public:
	Point position;
	int type;
	int getX() {
		return (int) position.x;
	}
	int getY() {
		return (int) position.y;
	}
};