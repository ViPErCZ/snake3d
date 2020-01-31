#include "Camera.h"

Camera::Camera() {
    posY = 183;
    posX = 117;
    angleZ = 45;
    angleX = -78;
    eyeX = 21;
    eyeY = 70;
    left = right = false;
    cameraType = 1;
    cycle = 0;
    stepLeft = stepRight = 1;
}

void Camera::process(float x, float y) {
    if (cameraType == 2) {
        glTranslatef(posX, posY, 0); // correctly center
        glRotatef(angleX/*-78*/, 1, 0, 0);
        glRotatef(0, 0, 1, 0);
        glRotatef(angleZ, 0, 0, 1);
        glTranslatef(posX, posY, -300); // correctly center


        gluLookAt(x + eyeX,
                  y - eyeY,
                  0.0,
                  x + eyeX,
                  y - eyeY,
                  -300.0,
                  1.0,
                  1.0,
                  1.0);
    }

    Uint32 now = SDL_GetTicks();

    if (cycle == 18) {
        if (left) {
            stepLeft++;
            stepRight = stepLeft - 4;
        } else {
            stepRight--;
            stepLeft = stepRight + 4;
        }
        if (stepLeft == 5) {
            stepLeft = 1;
        }
        if (stepRight == -3) {
            stepRight = 1;
        }

        left = right = false;
        cycle = 0;

        if (!queueRotation.empty()) {
            if ((*queueRotation.begin()) == 1) {
                left = true;
            } else if ((*queueRotation.begin()) == 2) {
                right = true;
            }
            queueRotation.erase(queueRotation.begin());
        }
    }

    if (now - next_time >= 15) {
        if (left) {
            if (angleZ == 0) {
                angleZ = 360;
            }
            angleZ -= 5;
            if (stepLeft == 1) {
                eyeY += 5;
                eyeX += 17;
            } else if (stepLeft == 2) {
                eyeY -= 18;
                eyeX += 5;
            } else if (stepLeft == 3) {
                eyeY -= 7;
                eyeX -= 18;
            } else if (stepLeft == 4) {
                eyeY += 20;
                eyeX -= 4;
            }
            cycle++;
        } else if (right) {
            if (angleZ == 360) {
                angleZ = 0;
            }
            angleZ += 5;
            if (stepRight == 1) {
                eyeY -= 20;
                eyeX += 4;
            } else if (stepRight == 0) {
                eyeY += 7;
                eyeX += 18;
            } else if (stepRight == -1) {
                eyeY += 18;
                eyeX -= 5;
            } else if (stepRight == -2) {
                eyeY -= 5;
                eyeX -= 17;
            }
            cycle++;
        }
        next_time = now;
    }
}

int Camera::getCameraType() const {
    return cameraType;
}

void Camera::setCameraType(int type) {
    this->cameraType = type;
}

void Camera::rotateLeft() {
    if (left || right) {
        queueRotation.push_back(1);
    } else {
        left = true;
    }
}

void Camera::rotateRight() {
    if (right || left) {
        queueRotation.push_back(2);
    } else {
        right = true;
    }
}

void Camera::reset() {
    posY = 183;
    posX = 117;
    angleZ = 45;
    angleX = -78;
    eyeX = 21;
    eyeY = 70;
    left = right = false;
    cycle = 0;
    stepLeft = stepRight = 1;
    queueRotation.clear();
}

void Camera::setPosX(int posX) {
    Camera::posX = posX;
}

void Camera::setPosY(int posY) {
    Camera::posY = posY;
}

void Camera::setEyeX(float eyeX) {
    Camera::eyeX = eyeX;
}

void Camera::setEyeY(float eyeY) {
    Camera::eyeY = eyeY;
}

void Camera::setAngleX(float angleX) {
    Camera::angleX = angleX;
}

void Camera::setAngleZ(float angleZ) {
    Camera::angleZ = angleZ;
}

int Camera::getPosX() const {
    return posX;
}

int Camera::getPosY() const {
    return posY;
}

float Camera::getEyeX() const {
    return eyeX;
}

float Camera::getEyeY() const {
    return eyeY;
}

float Camera::getAngleX() const {
    return angleX;
}

float Camera::getAngleZ() const {
    return angleZ;
}
