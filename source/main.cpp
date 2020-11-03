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


	//Need to preinitialize these because libtcod gets crazy without initialization...
	TCOD_key_t key;
	key.shift = false;

	TCOD_mouse_t mouse;

	//Fill map
	std::cout << "filling map" << std::endl;
	TCODRandom* rand = TCODRandom::getInstance();
	
	TCODBsp bsp(0, 0, testMap.width, testMap.height);

	bsp.splitRecursive(NULL, 10, MAX_ROOM_SIZE, MAX_ROOM_SIZE, 1.5f, 1.5f);
	BspListener listener(testMap);
	bsp.traverseInvertedLevelOrder(&listener, NULL);

	Unit* test = new Unit();
	test->character = '@';
	test->position.x = testMap.spawnX++;
	test->position.y = testMap.spawnY;
	test->goingTo = test->position;
	test->team = 1;
	test->color = TCODColor::cyan;

	Unit* test2 = new Unit();
	test2->character = '@';
	test2->position.x = testMap.spawnX++;
	test2->position.y = testMap.spawnY;
	test2->team = 1;
	test2->goingTo = test2->position;
	test2->color = TCODColor::red;

	Unit* test3 = new Unit();
	test3->character = '@';
	test3->position.x = testMap.spawnX++;
	test3->position.y = testMap.spawnY;
	test3->team = 1;
	test3->goingTo = test3->position;
	test3->color = TCODColor::orange;

	Unit* test4 = new Unit();
	test4->character = '@';
	test4->position.x = testMap.spawnX++;
	test4->position.y = testMap.spawnY;
	test4->team = 1;
	test4->goingTo = test4->position;
	test4->color = TCODColor::blue;

	Unit* testEnemy = new Unit();
	testEnemy->character = TCOD_CHAR_SMILIE;
	testEnemy->position.x = 10;
	testEnemy->position.y = 15;
	testEnemy->team = 2;
	testEnemy->goingTo = testEnemy->position;
	testEnemy->color = TCODColor::azure;
	testEnemy->aiming = test4;

	unitList.push_back(test);
	unitList.push_back(test2);
	unitList.push_back(test3);
	unitList.push_back(test4);

	//Add enemies
	for (Point pt : testMap.enemytospawn) {
		Unit* testEnemy = new Unit();
		testEnemy->character = TCOD_CHAR_SMILIE;
		testEnemy->position.x = pt.x;
		testEnemy->position.y = pt.y;
		testEnemy->team = 2;
		testEnemy->goingTo = testEnemy->position;
		testEnemy->color = TCODColor::azure;
		//t//estEnemy->aiming = test4;
		unitList.push_back(testEnemy);
	}
	for (int x = 0; x < testMap.width; x++) {
		for (int y = 0; y < testMap.height; y++) {
			minimapImg.putPixel(x, y, testMap.getTile(x, y).foreground);
		}
	}
	
	//Set minimap

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
		//Limit scroll
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

	if (key->pressed && (key->c == 'h' || key->c == 'H')) {
		//Toggle attack
		for (Unit* u : selectedUnits) {
			u->shooting = !u->shooting;
			u->shooting = true;
		}
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
			if (u->position.x >= beginningX-viewportX && u->position.x <= (beginningX + width) - viewportX && u->position.y >= beginningY - viewportY && u->position.y <= (beginningY + height) - viewportY && u->team == playerTeam) {
				if (!std::count(selectedUnits.begin(), selectedUnits.end(), u) && u->hp>0) {
					selectedUnits.push_back(u);
				}
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
		if (key->c == 'a' || key->c == 'A') {
			//A move
			for (Unit* u : selectedUnits) {
				if (u->team == playerTeam && u->shooting) {
					//Target closest enemy
					int distance = 2000000;
					for (Unit* enemy : unitList) {
						//If in enemy team, and in view range, and is close
						if (enemy->team != playerTeam && testMap.getTile(enemy->position.x, enemy->position.y).visible && enemy->hp > 0) {
							//Get distance
							int distFromUnit = sqrt(pow(u->position.x - enemy->position.x, 2)*1.0 + pow(u->position.y - enemy->position.y, 2) * 1.0);
							if (distFromUnit < distance) {
								u->aiming = enemy;
								distance = distFromUnit;
								std::cout << u->character << std::endl;
							}
						}
					}
				}
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

		//If clicking on unit, then attack it
		bool moving = true;
		for (Unit* toAttack : unitList) {
			if (worldMouseX == toAttack->position.x && worldMouseY == toAttack->position.y) {
				//Attack it
				moving = false;
				for (Unit* u : selectedUnits) {
					u->aiming = toAttack;
				}
				break;
			}
		}
		if (moving) {
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
					u->path->compute(u->position.x, u->position.y, u->goingTo.x, u->goingTo.y);
				}
				units++;
			}
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
	std::vector<Projectile*> projs;
	int k = 0;
	for (Projectile* projectile : projectiles) {
		
		projectile->positionX -= cos(projectile->degree) * projectile->speed;
		projectile->positionY -= sin(projectile->degree) * projectile->speed;

		//Check for intersection in ai
		for (Unit* u : unitList) {
			if (round(projectile->positionX) == round(u->position.x) && round(projectile->positionY) == round(u->position.y)) {
				u->hp--;
				projs.push_back(projectile);
				break;
			}
		}
		if (!testMap.getTile(round(projectile->positionX), round(projectile->positionY)).walkable) {
			projs.push_back(projectile);
			continue;
		}
		projectile->time++;
		if (projectile->time> 500) {
			projs.push_back(projectile);
		}
		k++;
	}
		

	int b = 0;
	for (Projectile* pro : projs) {
		projectiles.erase(std::remove(projectiles.begin(), projectiles.end(), pro), projectiles.end());
		b++;
	}

	//Clear map
	for (int i = 0; i < testMap.width * testMap.height; i++) {
		testMap.tiles[i].visible = false;
	}
	for (Unit* u : unitList) {
		testMap.map->setProperties(u->position.x, u->position.y, true, false);
		if (u->team == playerTeam && u-> hp > 0) {
			//Compute fov
			testMap.map->computeFov(u->position.x, u->position.y, 40);
			for (int i = 0; i < testMap.width * testMap.height; i++) {
				testMap.tiles[i].visible |= testMap.map->isInFov(i % testMap.width, i / testMap.width);
			}
		}
	}

	for (Unit *u: unitList) {
		if ((u->position.x == u->goingTo.x && u->position.y == u->goingTo.y) || u->path->size() <= 0) {
			u->pathTicker = 0;
			if (!u->actionQueue.empty()) {
				u->goingTo = u->actionQueue.front();
				u->actionQueue.pop_front();
				u->path = new TCODPath(testMap.map);
				u->path->compute(u->position.x, u->position.y, u->goingTo.x, u->goingTo.y);
			}
			else {
				u->goingTo = u->position;
			}
		}
		else {
			u->moveTicker++;
			if (u->moveTicker % u->moveSpeed == 0) {
				if (u->path->size() > u->pathTicker) {
					int x;
					int y;
					testMap.map->setProperties(u->position.x, u->position.y, true, true);

					u->path->get(u->pathTicker, &x, &y);
					u->position.x = x;
					u->position.y = y;
					u->pathTicker++;
				}
			}
		}

		//Enemy targets visible and close enemies
		for (Unit* u : unitList) {
			//If in enemy team, and in view range, and is close
			//If you can see the enemy, the enemy can see you
			u->shooting = true;
			if (u->team != playerTeam && u->shooting && testMap.getTile(u->position.x, u->position.y).visible && u->hp > 0) {
				//Target closest enemy
					int distance = 2000000;

					for (Unit* enemy : unitList) {
						if (enemy->team == playerTeam  && enemy->hp > 0) {
							//Get distance
							int distFromUnit = sqrt(pow(u->position.x - enemy->position.x, 2) * 1.0 + pow(u->position.y - enemy->position.y, 2) * 1.0);
							if (distFromUnit < distance) {
								u->aiming = enemy;
								distance = distFromUnit;
								
							}
						}
					}
				
			}
			else if(u->team != playerTeam && !testMap.getTile(u->position.x, u->position.y).visible) {
				u->shooting = false;
			}
		}

		TCODRandom* rand = TCODRandom::getInstance();
		//And shoot
		if (u->aiming != nullptr && u->shooting && u->hp > 0) {
			if (u->aiming->hp <= 0) {
				//Stop shooting because ded
				u->aiming = nullptr;
			} 
			else if (u->reload % 30 == 0) {
				Projectile* proj = new Projectile();
				proj->positionX = u->position.x;
				proj->positionY = u->position.y;

				proj->degree = std::atan2(-(u->aiming->position.y - u->position.y), -(u->aiming->position.x - u->position.x));
				//proj->degree +=rand->getDouble(-0.05, 0.05);
				proj->speed = 1;
				projectiles.push_back(proj);
				u->reload = 0;
			}
		}

		if (u->hp < 0) {
			//Ded...
			u->reload = 1;
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
			if (testMap.getTile(x, y).visible) {
				TCODConsole::root->setChar(x + viewportX, y + viewportY, testMap.getTile(x, y).character);
				TCODConsole::root->setCharForeground(x + viewportX, y + viewportY, testMap.getTile(x, y).foreground);
				TCODConsole::root->setCharBackground(x + viewportX, y + viewportY, testMap.getTile(x, y).background);
			}
			else {
				TCODConsole::root->setChar(x + viewportX, y + viewportY, testMap.getTile(x, y).character);
				TCODConsole::root->setCharForeground(x + viewportX, y + viewportY, testMap.getTile(x, y).foreground*0.5);
				TCODConsole::root->setCharBackground(x + viewportX, y + viewportY, testMap.getTile(x, y).background*0.5);
			}
		}
	}

	TCODConsole::root->print(0, 0, "FPS: %d", TCODSystem::getFps());
	
	if (!(boxWidth == 0 || boxHeight == 0) && drawBox) {
		TCODConsole::root->printFrame(boxStartX, boxStartY, boxWidth, boxHeight, TCOD_BKGND_NONE);
	}

	for (Unit* u : unitList) {
		//To show or not
		if (u->team == playerTeam || testMap.getTile(u->position.x, u->position.y).visible) {
			if (u->hp > 0) {
				TCODConsole::root->putChar((u->position.x) + viewportX, (u->position.y) + viewportY, u->character);
				TCODConsole::root->setCharForeground((int)((u->position.x) + viewportX), (int)((u->position.y) + viewportY), u->color);
			}
		}
	}

	for (Unit* u : selectedUnits) {
		TCODConsole::root->putChar((u->position.x - 1) + viewportX, (u->position.y - 1) + viewportY, TCOD_CHAR_NW);
		TCODConsole::root->putChar((u->position.x + 1) + viewportX, (u->position.y - 1) + viewportY, TCOD_CHAR_NE);
		TCODConsole::root->putChar((u->position.x - 1) + viewportX, (u->position.y + 1) + viewportY, TCOD_CHAR_SW);
		TCODConsole::root->putChar((u->position.x + 1) + viewportX, (u->position.y + 1) + viewportY, TCOD_CHAR_SE);

		//Draw destination
		if (u->position.x != u->goingTo.x && u->goingTo.y != u->position.y) {
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
		//Select shooting opponent
		if (u->aiming != nullptr && u->aiming->hp > 0) {
			TCODConsole::root->putChar((u->aiming->position.x) + viewportX, (u->aiming->position.y - 1) + viewportY, '|');
			TCODConsole::root->putChar((u->aiming->position.x) + viewportX, (u->aiming->position.y + 1) + viewportY, '|');
			TCODConsole::root->putChar((u->aiming->position.x - 1) + viewportX, (u->aiming->position.y) + viewportY, '-');
			TCODConsole::root->putChar((u->aiming->position.x + 1) + viewportX, (u->aiming->position.y) + viewportY, '-');
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

	//HHitpoint bars
	int k = 3;
	int multipler = 3;
	for (Unit* u : unitList) {
		if (u->hp > 0 && u->team == playerTeam) {
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
				//Remove all text
				for (int j = 0; j < barWidth; j++) {
					TCODConsole::root->setChar(j, k, (char)' ');
				}
			}

			// print text on top of the bar
			TCODConsole::root->setChar(0, k, u->character);
			TCODConsole::root->setCharForeground(0, k, u->color);
			TCODConsole::root->print(2, k, "%d", u->hp);
			k++;
		}
	}
	TCODConsole::root->setDefaultBackground(TCODColor::black);

	//Minimap
	minimapImg.blit(TCODConsole::root, 20, 80, TCOD_BKGND_SET, 0.125, 0.125);

	TCODConsole::flush();
}