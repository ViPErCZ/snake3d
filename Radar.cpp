#include <string>
#include "Radar.h"

Radar::Radar() {
    visibility = true;
    Vector3f pos = {5, 28, 0.0};
    Vector3f zoom = {2, 2, 2};

    sprite = new PolygonSprite();
    string screenPath = "images/radar/red_screen.bmp";
    sprite->setTexture((char *) screenPath.c_str());
    sprite->setPos(pos);

    Vector3f pos2 = {20, (float) (sprite->getSize().height) + 28, 0.0};
    snake = new PolygonSprite();
    string snakeTexture = "images/radar/snake.bmp";
    snake->setTexture((char *) snakeTexture.c_str());
    snake->setPos(pos2);
    snake->setZoom(zoom);

    Vector3f pos3 = {(float) (sprite->getSize().width * 2) - 20, (float) (sprite->getSize().height) + 28, 0.0};
    eat = new PolygonSprite();
    string eatTexture = "images/radar/eat.bmp";
    eat->setTexture((char *) eatTexture.c_str());
    eat->setPos(pos3);
    eat->setZoom(zoom);

    sprite->setZoom(zoom);
}

Radar::~Radar() {
    sprite->Release();
    snake->Release();
    eat->Release();

    delete sprite;
    delete snake;
    delete eat;
}

void Radar::render(int width, int height) {
    updatePositions();

    if (this->isVisibility()) {
        // switch to projection mode
        glMatrixMode(GL_PROJECTION);
        // save previous matrix which contains the
        //settings for the perspective projection
        glPushMatrix();

        // reset matrix
        glLoadIdentity();

        // set a 2D orthographic projection
        glOrtho(0.0F, width, height, 0.0F, 0.0F, 1.0F);

        // switch back to modelview mode
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        glLoadIdentity();

        // ------------------------------------------------------
        float LightPos[4] = {-5.0f, 5.0f, 10.0f, 1.0f};
        float Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float Ambient2[4] = {0.0f, 0.2f, 0.9f, 10.5f};;
        glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);

        // render radar

        // ...
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_BLEND);
        glColor4f(0.3f, 0.0f, 0.0f, 0.0f);
        this->sprite->Render();
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
        this->snake->Render();
        this->eat->Render();

        glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient2);

        glPopAttrib();
        glPopMatrix();

        // restore
        // --------------------------------
        glMatrixMode(GL_PROJECTION);
        // restore previous projection matrix
        glPopMatrix();

        // get back to modelview mode
        glMatrixMode(GL_MODELVIEW);
    }
}

bool Radar::isVisibility() const {
    return visibility;
}

void Radar::setVisibility(bool visibility) {
    Radar::visibility = visibility;
}

const Vector3f &Radar::getEatPos() const {
    return eatPos;
}

void Radar::setEatPos(const Vector3f &eatPos) {
    Radar::eatPos = eatPos;
}

void Radar::updatePositions() {
    this->eat->setPos(computeCoords(eatPos));
    this->snake->setPos(computeCoords(snakePos));
}

Vector3f Radar::computeCoords(const Vector3f &object) const {
    Vector3f pos = {};
    pos.x = (((float) (this->sprite->getSize().width - SCREEN_EDGE) * 2 / 1520 /* gameField width */) *
            object.x) +
            SCREEN_EDGE; // fix 0px init moving - no need
    pos.y = (((float) (this->sprite->getSize().height - SCREEN_EDGE) * 2 / 752 /* gameField height */) *
             (752 - object.y)) + 28 +
            SCREEN_EDGE; // fix 28px init moving
    pos.z = 0;

    return pos;
}

void Radar::setSnakePos(const Vector3f &snakePos) {
    Radar::snakePos = snakePos;
}
