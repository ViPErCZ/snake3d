#include <vector>
#include "stdafx.h"

#ifndef SNAKE_CAMERA_H
#define SNAKE_CAMERA_H

using namespace std;

class Camera {
public:
    Camera();

    void process(float posX, float posY);
    int getCameraType() const;
    void setCameraType(int type);
    void rotateLeft();
    void rotateRight();
    void setPosX(int posX);
    void setPosY(int posY);
    void setEyeX(float eyeX);
    void setEyeY(float eyeY);
    void setAngleX(float angleX);
    void setAngleZ(float angleZ);
    int getPosX() const;
    int getPosY() const;
    float getEyeX() const;
    float getEyeY() const;
    float getAngleX() const;
    float getAngleZ() const;
    int stepLeft;
    int stepRight;
    void reset();

protected:
    int posX;
    int posY;
    float eyeX;
    float eyeY;
    float angleX;
    float angleZ;
    bool left;
    bool right;
    int cameraType;
    int cycle;
    Uint32 next_time{};
    vector<int> queueRotation;
};


#endif //SNAKE_CAMERA_H
