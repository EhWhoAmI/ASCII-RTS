#pragma once

#include <deque>
#include "Map.h"
#include <libtcod.h>
class Unit {
public:
	TCODPath* path;
	int pathTicker = 0;
	int moveSpeed = 2;
	int moveTicker = 0;

	Point postion;
	std::deque<Point> actionQueue;
	Point goingTo;

	char character;
	TCODColor color;
};