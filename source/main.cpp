#ifdef DEBUG
#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif
#endif
#include "libtcod.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include "Unit.h"


void input(TCOD_key_t *key, TCOD_mouse_t *mouse);
void update();
void render();
void callControlGroup(TCOD_key_t* key, int keyNum);

TCODImage minimapImg(160, 100);


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

int viewportX = 0;
int viewportY = 0;

bool rightMouseClicked = false;
std::vector<Projectile*> projectiles;
std::vector<Unit*> *controlGroups = new std::vector<Unit*>[10];
bool end = false;

int hotkeyStartChar = 30;

int playerTeam = 1;
std::map<std::string, int> map;
#ifdef DEBUG
#ifdef _WIN32
int sq_main();
void WinMain() {
	sq_main();
}
int sq_main() {

#else 
int main(int argc, char* argv[])
#endif
#else
int main(int argc, char* argv[]) {
#endif // !DEBUG
	TCODConsole::initRoot(160, 100, "Squads", false);
	
	TCODSystem::setFps(60);

	Unit* test = new Unit();
	test->character = '@';
	test->postion.x = 80;
	test->postion.y = 50;
	test->goingTo = test->postion;
	test->team = 1;
	test->color = TCODColor::green;

	Unit* test2 = new Unit();
	test2->character = '@';
	test2->postion.x = 81;
	test2->postion.y = 51;
	test2->team = 1;
	test2->goingTo = test2->postion;
	test2->color = TCODColor::red;

	Unit* test3 = new Unit();
	test3->character = '@';
	test3->postion.x = 83;
	test3->postion.y = 51;
	test3->team = 1;
	test3->goingTo = test3->postion;
	test3->color = TCODColor::orange;

	Unit* test4 = new Unit();
	test4->character = '@';
	test4->postion.x = 81;
	test4->postion.y = 55;
	test4->team = 1;
	test4->goingTo = test4->postion;
	test4->color = TCODColor::blue;

	Unit* testEnemy = new Unit();
	testEnemy->character = TCOD_CHAR_SMILIE;
	testEnemy->postion.x = 10;
	testEnemy->postion.y = 15;
	testEnemy->team = 2;
	testEnemy->goingTo = testEnemy->postion;
	testEnemy->color = TCODColor::azure;
	testEnemy->aiming = test4;

	unitList.push_back(test);
	unitList.push_back(test2);
	unitList.push_back(test3);
	unitList.push_back(test4);
	unitList.push_back(testEnemy);


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
			/*if ( (y == 10 && x > 10)) {
				tile.character = '#';
			} 
			if ((y == 30 && x < 130)) {
				tile.character = '#';
			}*/


			if (tile.character == '.' || tile.character == ' ' || tile.character == '\"') {
				tile.foreground = TCODColor::green;
				tile.background = TCODColor::darkerGreen;
				tile.invisible = true;
				tile.walkable = true;
				testMap.map->setProperties(x, y, true, true);
			}
			else {
				tile.foreground = TCODColor::orange;
				tile.background = TCODColor::red;

				tile.invisible = false;
				tile.walkable = false;
				testMap.map->setProperties(x, y, false, false);
			}
			minimapImg.putPixel(x, y, tile.foreground);
			testMap.setTile(x, y, tile);
		}
	}
	
	while (!TCODConsole::isWindowClosed() && !end) {
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
	int scrollSpeed = 5;
	bool shiftPressed = key->shift;
	if (mouse->x <= 1) {
		TCODMouse::move(1, mouse->y);
		viewportX+= scrollSpeed;
	}
	if (mouse->y <= 1) {
		TCODMouse::move(mouse->x, 1);
		viewportY+= scrollSpeed;
	}

	int charWidth, charHeight;
	TCODSystem::getCharSize(&charWidth, &charHeight);
	if (mouse->x >= charWidth*TCODConsole::root->getWidth() -1) {
		TCODMouse::move(charWidth * TCODConsole::root->getWidth()-1, mouse->y);
		viewportX-= scrollSpeed;
	}
	if (mouse->y >= charHeight * TCODConsole::root->getHeight() -1) {
		TCODMouse::move(mouse->x, charHeight * TCODConsole::root->getHeight() - 1);
		viewportY-= scrollSpeed;
	}
	 
	switch (key->vk) {
	case TCODK_ESCAPE: 
		end = true;
		break;
		if (key->pressed) {
	case TCODK_0:
		callControlGroup(key, 0);
		break;
	case TCODK_1:
		callControlGroup(key, 1);
		break;
	case TCODK_2:
		callControlGroup(key, 2);
		break;
	case TCODK_3:
		callControlGroup(key, 3); break;
	case TCODK_4:
		callControlGroup(key, 4); break;
	case TCODK_5:
		callControlGroup(key, 5); break;
	case TCODK_6:
		callControlGroup(key, 6); break;
	case TCODK_7:
		callControlGroup(key, 7); break;
	case TCODK_8:
		callControlGroup(key, 8); break;
	case TCODK_9:
		callControlGroup(key, 9);
		break;
		}
	default:break;

	}

	int worldMouseX = mouse->cx - viewportX;
	int worldMouseY = mouse->cy - viewportY;
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
			if (u->postion.x >= beginningX-viewportX && u->postion.x <= (beginningX + width) - viewportX && u->postion.y >= beginningY - viewportY && u->postion.y <= (beginningY + height) - viewportY && u->team == playerTeam) {
				selectedUnits.push_back(u);
			}
		}
	}

	if (mouse->lbutton) {
		//If for character
		if (mouse->cy == 80) {
			int currentChar = mouse->cx - hotkeyStartChar;
			//Then is selecting things
			if (selectedUnits.size() > currentChar) {
				Unit* u = selectedUnits.at(currentChar);
				selectedUnits.clear();
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
				u->actionQueue.push_back(Point(worldMouseX + units, worldMouseY));
			}
			else {
				//Abort next move
				u->actionQueue.clear();
				u->goingTo = Point(worldMouseX + units, worldMouseY);
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
	std::vector<int> projs;
	int k = 0;
	for (Projectile* projectile : projectiles) {
		
		projectile->positionX -= cos(projectile->degree) * projectile->speed;
		projectile->positionY -= sin(projectile->degree) * projectile->speed;

		//Check for intersection in ai
		for (Unit* u : unitList) {
			if (round(projectile->positionX) == round(u->postion.x) && round(projectile->positionY) == round(u->postion.y)) {
				u->hp--;
				projs.push_back(k);
				break;
			}
		}
		projectile->time++;
		if (projectile->time> 500) {
			projs.push_back(k);
		}
		k++;
	}

	int b = 0;
	for (int pro : projs) {
		projectiles.erase(projectiles.begin() + pro - b);
		b++;
	}

	for (Unit* u : unitList) {
		testMap.map->setProperties(u->postion.x, u->postion.y, true, false);
	}
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
					int x;
					int y;
					testMap.map->setProperties(u->postion.x, u->postion.y, true, true);

					u->path->get(u->pathTicker, &x, &y);
					u->postion.x = x;
					u->postion.y = y;
					u->pathTicker++;
				}
			}
		}

		//Control AIs
		TCODRandom* rand = TCODRandom::getInstance();
		if (u->team != playerTeam && u->reload % 30 == 0) {
			Projectile* proj = new Projectile();
			proj->positionX = u->postion.x;
			proj->positionY = u->postion.y;
			proj->degree = std::atan2(-(u->aiming->postion.y - u->postion.y), -(u->aiming->postion.x - u->postion.x));
			//proj->degree +=rand->getDouble(-0.05, 0.05);
			proj->speed = 1;
			projectiles.push_back(proj);
			u->reload = 0;
		}

		if (u->hp < 0) {
			//Ded...
		}

		u->reload++;
	}
	
}

void callControlGroup(TCOD_key_t* key, int keyNum) {
	if (key->lctrl) {
		controlGroups[keyNum] = selectedUnits;
	}
	else {
		if (!controlGroups[keyNum].empty()) {
			selectedUnits = controlGroups[keyNum];
		}
	}
}
void render() {
	TCODConsole::root->setDefaultBackground(TCODColor::black);

	//Draw map
	int x = 0, y = 0;
	for (int x = 0; x < testMap.width; x++) {
		for(int y = 0; y < testMap.height; y++) {
			TCODConsole::root->setChar(x + viewportX, y + viewportY, testMap.getTile(x, y).character);
			TCODConsole::root->setCharForeground(x + viewportX, y + viewportY, testMap.getTile(x, y).foreground);
			TCODConsole::root->setCharBackground(x + viewportX, y + viewportY, testMap.getTile(x, y).background);

		}
	}

	TCODConsole::root->print(0, 0, "FPS: %d", TCODSystem::getFps());
	
	if (!(boxWidth == 0 || boxHeight == 0) && drawBox) {
		TCODConsole::root->printFrame(boxStartX, boxStartY, boxWidth, boxHeight, TCOD_BKGND_NONE);
	}

	for (Unit* u : unitList) {
		TCODConsole::root->putChar((u->postion.x) + viewportX, (u->postion.y) + viewportY, u->character);
		TCODConsole::root->setCharForeground((u->postion.x) + viewportX, (u->postion.y) + viewportY, u->color);
	}

	for (Unit* u : selectedUnits) {
		TCODConsole::root->putChar((u->postion.x - 1) + viewportX, (u->postion.y - 1) + viewportY, TCOD_CHAR_NW);
		TCODConsole::root->putChar((u->postion.x + 1) + viewportX, (u->postion.y - 1) + viewportY, TCOD_CHAR_NE);
		TCODConsole::root->putChar((u->postion.x - 1) + viewportX, (u->postion.y + 1) + viewportY, TCOD_CHAR_SW);
		TCODConsole::root->putChar((u->postion.x + 1) + viewportX, (u->postion.y + 1) + viewportY, TCOD_CHAR_SE);

		//Draw destination
		if (u->postion.x != u->goingTo.x && u->goingTo.y != u->postion.y) {
			TCODConsole::root->setCharBackground((u->goingTo.x) + viewportX, (u->goingTo.y) + viewportY, TCODColor::darkYellow);
		}
		//If there is a queue, draw that
		Point lastPosition = u->goingTo;
		for (int i = 0; i < u->actionQueue.size(); i++) {
			Point pt = u->actionQueue.at(i);
			TCODLine::init((lastPosition.x), (lastPosition.y), pt.x, pt.y);
			int counterX = lastPosition.x, counterY = lastPosition.y;
			int ticker = 0;
			do {
				if (ticker % 2 == 0) 
					TCODConsole::root->setCharBackground(counterX + viewportX, counterY + viewportY, TCODColor::lighterGreen);
				ticker++;
			} while (!TCODLine::step(&counterX, &counterY));
			lastPosition.x = counterX;
			lastPosition.y = counterY;
		}
	}

	for (Projectile* proj : projectiles) {
		TCODConsole::root->putChar((proj->positionX) + viewportX, (proj->positionY) + viewportY, 'o');
		TCODConsole::root->setCharForeground((proj->positionX) + viewportX, (proj->positionY) + viewportY, TCODColor::white);
	}

	//Bottom console
	TCODConsole::root->printFrame(0, 70, 160, 30, true, TCOD_BKGND_SET);

	//Minimap
	TCODConsole::root->printFrame(0, 70, 30, 30, true, TCOD_BKGND_SET);

	//Hotkey displayers
	for (int k = 1; k < 10; k++) {
		std::vector<Unit*> controlGroup = controlGroups[k];
		int index = k - 1;
		if (!controlGroup.empty()) {
			TCODConsole::root->putChar(hotkeyStartChar + index * 4, 71, k + 48);
			TCODConsole::root->putChar(hotkeyStartChar + index * 4 + 2, 71, controlGroup.at(0)->character);
			TCODConsole::root->setCharForeground(hotkeyStartChar + index * 4 + 2, 71, controlGroup.at(0)->color);
			TCODConsole::root->putChar(hotkeyStartChar + index * 4 + 3, 71,TCOD_CHAR_VLINE);
		}
	}

	std::vector<Unit*> controlGroup = controlGroups[0];
	if (!controlGroup.empty()) {
		TCODConsole::root->putChar(hotkeyStartChar + 10*4, 71, '0');
		TCODConsole::root->putChar(hotkeyStartChar + 10*4 + 2, 71, controlGroup.at(0)->character);
		TCODConsole::root->setCharForeground(hotkeyStartChar + 10 * 4 + 2, 71, controlGroup.at(0)->color);
		TCODConsole::root->putChar(hotkeyStartChar +10 * 4 + 3, 71, TCOD_CHAR_VLINE);

	}
	//Selected characters
	for (int i = 0; i < selectedUnits.size(); i++) {
		TCODConsole::root->putChar(hotkeyStartChar + i, 80, selectedUnits.at(i)->character);
		TCODConsole::root->setCharForeground(hotkeyStartChar + i, 80, selectedUnits.at(i)->color);
	}

	//Print hitpoints
	int k = 3;
	int multipler = 3;
	for (Unit* u : unitList) {
		TCODConsole::root->setChar(0, k, u->character);
		TCODConsole::root->setCharForeground(0, k, u->color);
		TCODConsole::root->print(2, k,"%d", u->hp);
		//Set hp bars

		int side = 1;
		// fill the background
		TCODConsole::root->setDefaultBackground(TCODColor::blue);
		double width = 20;
		TCODConsole::root->rect(side, k, width, 1, false, TCOD_BKGND_SET);
		double twenty = 20;
		int barWidth = (int)(((double)u->hp) / twenty * width);
		if (barWidth > 0) {
			// draw the bar
			TCODConsole::root->setDefaultBackground(TCODColor::red);
			TCODConsole::root->rect(side, k, barWidth, 1, false, TCOD_BKGND_SET);
		}
		// print text on top of the bar
		k++;
	}
	//Minimap
	//minimapImg.blit(TCODConsole::root, 20, 80, TCOD_BKGND_SET, 0.125, 0.125);

	TCODConsole::flush();
}