#include "libtcod.hpp"
#include <iostream>
#include <vector>
#include "Unit.h"



void input(TCOD_key_t *key, TCOD_mouse_t *mouse);
void update();
void render();
std::vector<Unit*> unitList;

std::vector<Unit*> selectedUnits;

Map testMap(160, 100);

int selectionBoxStartX = 0;
int selectionBoxStartY = 0;

int boxStartX = 0;
int boxStartY = 0;
int boxWidth = 0;
int boxHeight = 0;
bool drawBox = false;
bool leftMouseClicking = false;

bool rightMouseClicked = false;

int main(int argc, char* argv[]) {
	TCODConsole::initRoot(160, 100, "Squads", false);
	TCODConsole::root->setFullscreen(true);
	TCODSystem::setFps(60);

	Unit* test = new Unit();
	test->postion.x = 80;
	test->postion.y = 50;
	test->goingTo = test->postion;
	test->color = TCODColor::green;

	Unit* test2 = new Unit();
	test2->postion.x = 81;
	test2->postion.y = 51;
	test2->goingTo = test2->postion;
	test2->color = TCODColor::red;

	unitList.push_back(test);
	unitList.push_back(test2);

	//Need to preinitialize these because libtcod gets crazy without initialization...
	TCOD_key_t key;
	key.shift = false;

	TCOD_mouse_t mouse;

	//Fill map
	std::cout << "filling map" << std::endl;
	TCODRandom* rand = TCODRandom::getInstance();
	for (int x = 0; x < testMap.width; x++) {
		for (int y = 0; y < testMap.height; y++) {
			Tile tile;
			
			tile.character = '.';
			
			if (rand->getInt(0, 7) != 5) {
				tile.character = ' ';
			}
			if (rand->getInt(0, 15) == 5) {
				tile.character = '\"';
			}
			if (x == 4 || (y == 10 && x > 10)) {
				tile.character = '#';
			}
			if (tile.character == '.' || tile.character == ' ' || tile.character == '\"') {
				tile.foreground = TCODColor::green;
				tile.invisible = true;
				tile.walkable = true;
				testMap.map->setProperties(x, y, true, true);
			}
			else {
				tile.foreground = TCODColor::orange;
				tile.invisible = false;
				tile.walkable = false;
				testMap.map->setProperties(x, y, false, false);
			}
			testMap.setTile(x, y, tile);
		}
	}
	
	while (!TCODConsole::isWindowClosed()) {
		//Game loop
		
		TCODConsole::root->clear();

		TCODSystem::checkForEvent(TCOD_EVENT_ANY, &key, &mouse);
		
		input(&key, &mouse);
		update();
		render();
	}

	//Cleanup
	for (Unit* u : unitList) {
		delete u;
	}
	return 0;
}

void input(TCOD_key_t* key, TCOD_mouse_t* mouse) {
	bool shiftPressed = key->shift;
	if (mouse->x <= 1) {
		TCODMouse::move(1, mouse->y);
	}
	if (mouse->y <= 1) {
		TCODMouse::move(mouse->x, 1);
	}
	switch (key->vk) {
	//case TCOD: break;
	default:break;

	}
	TCODConsole::root->print(0, 6, key->text);
	//If to draw box
	if (leftMouseClicking && mouse->lbutton) {
		int beginningX = selectionBoxStartX;
		int beginningY = selectionBoxStartY;
		int endX = mouse->cx;
		int endY = mouse->cy;

		int width = endX - beginningX;
		int height = endY - beginningY;

		//Get top corner
		if (width < 0) {
			beginningX += width;
			width *= -1;
		}
		if (height < 0) {
			beginningY += height;
			height *= -1;
		}

		boxStartX = beginningX;
		boxStartY = beginningY;
		boxWidth = width;
		boxHeight = height;
		drawBox = false;
		if (!(width == 0 || height == 0)) {
			selectedUnits.clear();
			drawBox = true;
		}

		//Check if player is
		for (Unit* u : unitList) {
			if (u->postion.x >= beginningX && u->postion.x <= (beginningX + width) && u->postion.y >= beginningY && u->postion.y <= (beginningY + height)) {
				selectedUnits.push_back(u);
			}
		}
	}

	//Selection box code
	if (!leftMouseClicking && mouse->lbutton) {
		selectionBoxStartX = mouse->cx;
		selectionBoxStartY = mouse->cy;
		leftMouseClicking = true;
	}

	if (leftMouseClicking && !mouse->lbutton) {
		leftMouseClicking = false;
		drawBox = false;
	}

	//Move code
	if (mouse->rbutton && !rightMouseClicked) {
		int units = 0;
		for (Unit* u : selectedUnits) {
			if (shiftPressed) {
				u->actionQueue.push_back(Point(mouse->cx+ units, mouse->cy));
			}
			else {
				//Abort next move
				u->actionQueue.clear();
				u->goingTo = Point(mouse->cx+ units, mouse->cy);
				u->pathTicker = 0;

				u->path = new TCODPath(testMap.map);
				u->path->compute(u->postion.x, u->postion.y, u->goingTo.x, u->goingTo.y);
			}
			units++;
		}
	}


	if (mouse->rbutton && !rightMouseClicked) {
		rightMouseClicked = true;
	}

	if (!mouse->rbutton && rightMouseClicked) {
		rightMouseClicked = false;
	}
}

void update() {
	for (Unit *u: unitList) {
		if ((u->postion.x == u->goingTo.x && u->postion.y == u->goingTo.y) || u->path->size() <= 0) {
			u->pathTicker = 0;
			if (!u->actionQueue.empty()) {
				u->goingTo = u->actionQueue.front();
				u->actionQueue.pop_front();
				u->path = new TCODPath(testMap.map);
				u->path->compute(u->postion.x, u->postion.y, u->goingTo.x, u->goingTo.y);
			}
			else {
				u->goingTo = u->postion;
			}
		}
		else {
			u->moveTicker++;
			if (u->moveTicker % u->moveSpeed == 0) {
				if (u->path->size() > u->pathTicker) {
					u->path->get(u->pathTicker, &u->postion.x, &u->postion.y);
					u->pathTicker++;
				}
			}
		}
	}
}

void render() {
	//Draw map
	int x = 0, y = 0;
	for (int x = 0; x < testMap.width; x++) {
		for(int y = 0; y < testMap.height; y++) {
			TCODConsole::root->setChar(x, y, testMap.getTile(x, y).character);
			TCODConsole::root->setCharForeground(x, y, testMap.getTile(x, y).foreground);
		}
	}

	TCODConsole::root->print(0, 0, "FPS: %d", TCODSystem::getFps());
	
	if (!(boxWidth == 0 || boxHeight == 0) && drawBox) {
		TCODConsole::root->printFrame(boxStartX, boxStartY, boxWidth, boxHeight, TCOD_BKGND_NONE);
	}

	for (Unit* u : unitList) {
		TCODConsole::root->putChar(u->postion.x, u->postion.y, '@');
		TCODConsole::root->setCharForeground(u->postion.x, u->postion.y, u->color);
	}

	for (Unit* u : selectedUnits) {
		TCODConsole::root->putChar(u->postion.x - 1, u->postion.y - 1, TCOD_CHAR_NW);
		TCODConsole::root->putChar(u->postion.x + 1, u->postion.y - 1, TCOD_CHAR_NE);
		TCODConsole::root->putChar(u->postion.x - 1, u->postion.y + 1, TCOD_CHAR_SW);
		TCODConsole::root->putChar(u->postion.x + 1, u->postion.y + 1, TCOD_CHAR_SE);

		//Draw destination
		if (u->postion.x != u->goingTo.x && u->goingTo.y != u->postion.y) {
			TCODConsole::root->setCharBackground(u->goingTo.x, u->goingTo.y, TCODColor::darkYellow);
		}
		//If there is a queue, draw that
		Point lastPosition = u->goingTo;
		for (int i = 0; i < u->actionQueue.size(); i++) {
			Point pt = u->actionQueue.at(i);
			TCODLine::init(lastPosition.x, lastPosition.y, pt.x, pt.y);
			int counterX = lastPosition.x, counterY = lastPosition.y;
			do {
				TCODConsole::root->setCharBackground(counterX, counterY, TCODColor::yellow);
			} while (!TCODLine::step(&counterX, &counterY));
			lastPosition.x = counterX;
			lastPosition.y = counterY;
		}
	}

	TCODConsole::flush();
}