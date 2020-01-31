#include "stdafx.h"
#include "GameField.h"

GameField::GameField(int width, int height):
changeDirection(false),
cacheDirection(STOP)
{
    rotationType = 1;
    this->width = width;
    this->height = height;
	gameFieldSprite = new PolygonSprite();
	gameFieldSprite2 = new PolygonSprite();
	Vector3f pos = {50, 160, 0.0};
	Vector3f pos2 = {768, 0, 0.0};
	gameFieldSprite->setPos(pos);
	gameFieldSprite->setPos(pos2);
	gameFieldSprite->setTexture("images/gamefield.bmp");
	gameFieldSprite2->setTexture("images/gamefield.bmp");

	Head = new CubeSprite();
	Head->setTexture("images/Base.bmp");
	pos.x = 16;
	pos.y = 16;
	pos.z = 15;
	Head->setPos(pos);

	snake = new SnakeContainer();
	snake->Init();

	eDirection = STOP;

	info = new TextSprite("Snake: ");
	Vector3f posInfo = {6, 20, 0};
	info->setPos(posInfo);

	food = new Eat();
	food->setTexture("images/eat.bmp");
	Vector3f zoom = {};
	zoom.x = 15.0;
	zoom.y = 15.0;
	zoom.z = 15.0;
	food->setZoom(zoom);
	food->setPos(snake->getFreeArray());

	radar = new Radar();
    radar->setEatPos(food->getPos());
    Vector3f snakePos = {snake->getHeadPos().x, snake->getHeadPos().y, 0};
    radar->setSnakePos(snakePos);

	this->generateWalls();
}

GameField::~GameField()
{
	delete gameFieldSprite;
	delete gameFieldSprite2;
	delete Head;
	delete info;
	delete radar;
	snake->Release();
	delete snake;
	food->Release();
	delete food;

    for (auto Iter = walls.begin(); Iter < walls.end(); Iter++) {
        delete (*Iter).wall;
    }

    walls.clear();
}

void GameField::Render() {
    float LightPos[4]={0.0f,0.0f,0.0f,0.0f};
    float Ambient[4]={1.0f,0.0f,0.0f,1.0f};
    float Ambient2[4]={0.0f,0.2f,0.9f,10.5f};;
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
	gameFieldSprite->Render();
	gameFieldSprite2->Render();
    glDisable(GL_FOG);

    for (auto Iter = walls.begin(); Iter < walls.end(); Iter++) {
        glBindTexture( GL_TEXTURE_2D, (*Iter).texture);
        (*Iter).wall->Render();
    }

    glEnable(GL_FOG);
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient2);

	snake->Render();

	Vector3f zoom = {};
	zoom.x = 15.0;
	zoom.y = 15.0;
	zoom.z = 15.0;
	Head->setZoom(zoom);

	food->Render();
	radar->render(this->width, this->height);
}

void GameField::Render2D() {
	char buffer[255];
	fString str("Snake: ");
	str.append("posX: ");
	//itoa(Head->getPos().x, buffer, 10);
	sprintf(buffer, "%d", (int)snake->getHeadPos().x);
	str.append(buffer);
	str.append(" posY: ");
	//itoa(Head->getPos().y, buffer, 10);
	sprintf(buffer, "%d", (int)snake->getHeadPos().y);
	str.append(buffer);
	str.append(", Tiles: ");
    sprintf(buffer, "%d", (int)snake->getSize());
    str.append(buffer);
	info->setText(str);
	info->Render(this->width, this->height);
}

void GameField::Proceed(SDL_Event* event) {
	Vector3f pos = {};
	Uint32 now = SDL_GetTicks();

	if (eDirection != PAUSE) {
        if (cacheDirection != STOP && !changeDirection && snake->isChangeDirectionAllowed()) {
            eDirection = cacheDirection;
            cacheDirection = STOP;
        }
        if (snake->isSnakeCrash()) {
            eDirection = CRASH;
        }

        if (now - next_time >= 16) {
            if (eDirection != snake->getHeadDirection()) {
                if (snake->getHeadDirection() == STOP) {
                    if (rotationType == 1) {
                        camera->rotateLeft();
                    } else if (rotationType == 2) {
                        camera->rotateRight();
                    }
                    rotationType = 0;
                } else {
                    if ((eDirection == LEFT && snake->getHeadDirection() == UP) ||
                        (eDirection == UP && snake->getHeadDirection() == RIGHT) ||
                        (eDirection == DOWN && snake->getHeadDirection() == LEFT) ||
                        (eDirection == RIGHT && snake->getHeadDirection() == DOWN)) {
                        camera->rotateLeft();
                    } else {
                        camera->rotateRight();
                    }
                }
            }
            switch (eDirection) {
                case LEFT:
                    pos = Head->getPos();
                    if (pos.x > 16) {
                        pos.x -= UNIT_MOVE;
                    }
                    Head->setPos(pos);
                    if (!snake->isSnakeCrash()) {
                        (*left_right) += 9;
                    }
                    break;
                case RIGHT:
                    pos = Head->getPos();
                    if (pos.x < 1520) { // 752
                        pos.x += UNIT_MOVE;
                    }
                    Head->setPos(pos);
                    if (!snake->isSnakeCrash()) {
                        (*left_right) -= 9;
                    }
                    break;
                case UP:
                    pos = Head->getPos();
                    if (pos.y < 752)
                        pos.y += UNIT_MOVE;
                    Head->setPos(pos);
                    break;
                case DOWN:
                    pos = Head->getPos();
                    if (pos.y > 16)
                        pos.y -= UNIT_MOVE;
                    Head->setPos(pos);
                    break;
                case STOP:
                case PAUSE:
                    break;
                case CRASH:
                    if (!snake->isSnakeVisible()) {
                        this->resetGame();
                    }
                    break;
            }
            next_time = now;
            snake->Move(eDirection);
            changeDirection = false;
        }
    }

    //bool isChangeDirectionAllowed = snake->isChangeDirectionAllowed();

	if (eDirection != CRASH) {
        if (this->camera->getCameraType() == 1) {
            switch (event->type) {
                case SDL_KEYDOWN:
                    switch (event->key.keysym.sym) {
                        case SDLK_SPACE:
                            if (eDirection == PAUSE) {
                                eDirection = cacheDirection;
                            } else {
                                cacheDirection = eDirection;
                                eDirection = PAUSE;
                            }
                            break;
                        case SDLK_j: // left
                            if (eDirection != PAUSE) {
                                 if (eDirection != RIGHT && eDirection != STOP) {
                                    if (eDirection == UP) {
                                        rotationType = 1; // left
                                    } else {
                                        rotationType = 2; // right
                                    }
                                    cacheDirection = LEFT;
                                    changeDirection = true;
                                }
                            }
                            break;
                        case SDLK_l: // right
                            if (eDirection != PAUSE) {
                                 if (eDirection != LEFT) {
                                    if (eDirection != STOP) {
                                        if (eDirection == UP) {
                                            rotationType = 2; // right
                                        } else {
                                            rotationType = 1; // left
                                        }
                                    }
                                    cacheDirection = RIGHT;
                                    changeDirection = true;
                                }
                            }
                            break;
                        case SDLK_i: // up
                            if (eDirection == STOP) {
                                rotationType = 1;
                            }
                            if (eDirection != PAUSE) {
                                 if (eDirection != DOWN) {
                                    if (eDirection == LEFT) {
                                        rotationType = 2; // right
                                    } else {
                                        rotationType = 1; // left
                                    }
                                    cacheDirection = UP;
                                    changeDirection = true;
                                }
                            }
                            break;
                        case SDLK_k: // down
                            if (eDirection == STOP) {
                                rotationType = 1;
                            }
                            if (eDirection != PAUSE) {
                                 if (eDirection != UP) {
                                    if (eDirection == LEFT) {
                                        rotationType = 1; // left
                                    } else {
                                        rotationType = 2; // right
                                    }
                                    cacheDirection = DOWN;
                                    changeDirection = true;
                                }
                            }
                            break;
                    }
                    break;
            }
        } else {
            switch (event->type) {
                case SDL_KEYDOWN:
                    switch (event->key.keysym.sym) {
                        case SDLK_SPACE:
                            if (eDirection == PAUSE) {
                                eDirection = cacheDirection;
                            } else {
                                cacheDirection = eDirection;
                                eDirection = PAUSE;
                            }
                            break;
                        case SDLK_j: // left
                            if (eDirection != PAUSE) {
                                if (eDirection == LEFT) {
                                    cacheDirection = DOWN;
                                } else if (eDirection == UP) {
                                    cacheDirection = LEFT;
                                } else if (eDirection == DOWN) {
                                    cacheDirection = RIGHT;
                                } else {
                                    cacheDirection = UP;
                                }
                                changeDirection = true;
                                rotationType = 1;
                            }
                            break;
                        case SDLK_l: // right
                            if (eDirection != PAUSE) {
                                if (eDirection == LEFT) {
                                    cacheDirection = UP;
                                } else if (eDirection == UP) {
                                    cacheDirection = RIGHT;
                                } else if (eDirection == DOWN) {
                                    cacheDirection = LEFT;
                                } else {
                                    cacheDirection = DOWN;
                                }
                                changeDirection = true;
                                rotationType = 2;
                            }
                            break;
                        case SDLK_i: // up
                            if (eDirection == STOP) {
                                cacheDirection = RIGHT;
                                changeDirection = true;
                                rotationType = 0;
                            }
                            break;
                    }
                    break;
            }
        }

        switch (event->type) {
            case SDL_KEYDOWN:
                switch (event->key.keysym.sym) {
                    case SDLK_m:
                        radar->setVisibility(!radar->isVisibility());
                        break;
                }
        }
    }

	// validate change direction vs head actual direction
	validateDirection();
	// food lifetime process
    foodProcess();

    // update snake into radar
    Vector3f snakePos = {snake->getHeadPos().x, snake->getHeadPos().y, 0};
    radar->setSnakePos(snakePos);
}

void GameField::foodProcess() {
    if (this->snake->getHeadPos().x > this->food->getPos().x - 32 && this->snake->getHeadPos().x < this->food->getPos().x + 32 &&
        this->snake->getHeadPos().y > this->food->getPos().y - 32 && this->snake->getHeadPos().y < this->food->getPos().y + 32) { // check food collision
        Vector2f foodPos = {this->food->getPos().x, this->food->getPos().y};
        int testCounter = 0;
        do { // place new food
            if (testCounter > 3) {
                foodPos = this->snake->getFreeArray();
                if (foodPos.x == -1 && foodPos.y == -1) {
                    this->eDirection = STOP;
                    return;
                }
                else
                    this->food->setPos(foodPos);
                break;
            }
            else {
                this->food->setPos();
                foodPos.x = this->food->getPos().x;
                foodPos.y = this->food->getPos().y;
            }
            testCounter++;
        } while(this->snake->isSnakeInThisPos(foodPos));

        this->snake->AddTile(this->eDirection);
        this->snake->AddTile(this->eDirection);
        radar->setEatPos(food->getPos());
    }
}

void GameField::setLeftRight(float *pos) {
    left_right = pos;
}

Vector2f GameField::getHeadPos() {
    return snake->getHeadPos();
}

void GameField::generateWalls() {
    Vector3f zoom = {15, 15, 15};
    Vector2f wallPos = {-16, -16};
    string path = "images/wall.bmp";
    GLuint texture = Sprite::loadTexture((char*) path.c_str());

    for (int round = 0; round < 2; round++) {
        for (int x = -16; x <= 1552; x += 32) {
            Wall *cube = new Wall();
            //cube->setTexture((char *) path.c_str());
            wallPos.x = (float) x;
            cube->setPos(wallPos);
            cube->setZoom(zoom);

            sWALL aWall{};
            aWall.wall = cube;
            aWall.texture = texture;

            this->walls.push_back(aWall);
        }

        wallPos.x = -16;
        wallPos.y = 784;
    }

    wallPos.x = -16;
    wallPos.y = 16;

    for (int round = 0; round < 2; round++) {
        for (int y = 16; y <= 752; y += 32) {
            Wall *cube = new Wall();
            //cube->setTexture((char *) path.c_str());
            wallPos.y = (float) y;
            cube->setPos(wallPos);
            cube->setZoom(zoom);

            sWALL aWall{};
            aWall.wall = cube;
            aWall.texture = texture;

            this->walls.push_back(aWall);
        }

        wallPos.x = 1552;
        wallPos.y = 16;
    }
}

void GameField::resetGame() {
    camera->reset();
    snake->reset();
    eDirection = cacheDirection = STOP;
}

void GameField::validateDirection() {
    eDIRECTION headDirection = snake->getHeadDirection();

    if (cacheDirection == STOP) {
        if (eDirection == LEFT && headDirection == RIGHT) {
            eDirection = RIGHT;
        } else if (eDirection == RIGHT && headDirection == LEFT) {
            eDirection = LEFT;
        } else if (eDirection == UP && headDirection == DOWN) {
            eDirection = DOWN;
        } else if (eDirection == DOWN && headDirection == UP) {
            eDirection = UP;
        }
    } else {
        if (cacheDirection == LEFT && headDirection == RIGHT) {
            eDirection = RIGHT;
            cacheDirection = STOP;
            rotationType = 0;
        } else if (cacheDirection == RIGHT && headDirection == LEFT) {
            eDirection = LEFT;
            cacheDirection = STOP;
            rotationType = 0;
        } else if (cacheDirection == UP && headDirection == DOWN) {
            eDirection = DOWN;
            cacheDirection = STOP;
            rotationType = 0;
        } else if (cacheDirection == DOWN && headDirection == UP) {
            eDirection = UP;
            cacheDirection = STOP;
            rotationType = 0;
        }
    }
}

void GameField::setCamera(Camera* camera) {
    this->camera = camera;
}

bool GameField::isPauseOrStop() {
    return eDirection == PAUSE || eDirection == STOP;
}
