#include "stdafx.h"
#include "Engine.h"

Engine::Engine(SDL_Renderer * renderer, int aWidth, int aHeight):
mouseDownIsAction(false)
{
	width = aWidth;
	height = aHeight;
	info = new TextSprite("informace");
	info->setTexture((char*)"images/tile.bmp");
	Vector3f pos = {0, 50, 0};
	info->setPos(pos);
	last_time = 0;
	fps = 0.0f;
	angle = 41.0f;
	left_right = -54;
	up_down = -147;
	zoom = -450;
	this->renderer = renderer;
	camera = new Camera();
	camera->setCameraType(2);
    gameField = new GameField(width, height);
    gameField->renderer = renderer;
    gameField->setCamera(camera);
//    angleZ = 45;
    angleX = -78;
//    eyeX = 21;
//    eyeY = 70;

    setup_opengl(true);
}

Engine::~Engine()
{
	delete gameField;
	delete info;
	delete camera;
}

Uint32 Engine::TimeLeft(void)
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

void Engine::setup_opengl(bool freeD)
{
	if (freeD) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(40, (GLfloat)width/height, 0.5f, 1500.0f);
		//glTranslatef(left_right, up_down, zoom); // correctly center
		if (camera->getCameraType() == 1) {
            glTranslatef(-54, up_down, zoom); // correctly center
            //info->setText(std::to_string(left_right));
            glRotatef(-60, 1, 0, 0);
            glRotatef(angle, 0, 0, 1);

            gluLookAt(gameField->getHeadPos().x - 154,
                      gameField->getHeadPos().y + 0.0025 * std::abs(sin(1 * 3.14 / 180)) + up_down,
                      0.0,
                      gameField->getHeadPos().x - 154,
                      gameField->getHeadPos().y + 0.0025 * std::abs(sin(1 * 3.14 / 180)) + up_down,
                      -1.0,
                      0.0,
                      1.0,
                      0.0);
        } else {
                camera->process(gameField->getHeadPos().x, gameField->getHeadPos().y);
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glShadeModel(GL_SMOOTH);
		glClearDepth(1.0f);						    // Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
        //glDepthFunc(GL_LESS);
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		glEnable(GL_TEXTURE_2D);
		GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f };// Okolní svìtlo
		glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
		glEnable(GL_LIGHT1);
		glEnable(GL_TEXTURE_2D);
		glClearStencil(0);
	}
	else {
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (0, width, height, 0, 0, 1);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);
	}

    glClearColor(.0, .0, .0, 0);
}

void Engine::Run(SDL_Window* window) {
    setup_opengl(true);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float LightPos[4]={5.0f,5.0f,00.0f,1.0f};
    float Ambient[4]={0.0f,0.2f,0.9f,1.0f};
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);

    GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };// Tøi typy mlhy
    GLuint fogfilter = 2;// Která mlha se používá
    GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};// Barva mlhy
    glFogi(GL_FOG_MODE, fogMode[fogfilter]);// Mód mlhy
    glFogfv(GL_FOG_COLOR, fogColor);// Barva mlhy
    glFogf(GL_FOG_DENSITY, 0.05f);// Hustota mlhy
    glHint(GL_FOG_HINT, GL_DONT_CARE);// Kvalita mlhy
    glFogf(GL_FOG_START, 1.0f);// Zaèátek mlhy - v hloubce (osa z)
    glFogf(GL_FOG_END, 120.0f);// Konec mlhy - v hloubce (osa z)
    //glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);
    glEnable(GL_FOG);// Zapne mlhu

    do {
		calculateFPS();

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity( );
        //glClearColor(0.5f,0.5f,0.5f,1.0f);// Šedá barva pozadí (stejná, jako má mlha)

        glClear(GL_COLOR_BUFFER_BIT);

		Render();

		SDL_GL_SwapWindow(window);
		//SDL_GL_SwapBuffers( );
		//SDL_Delay(TimeLeft());
		glLoadIdentity( );
		SDL_PollEvent(&event);
		gameField->Proceed(&event);
		switch (event.type) {
			case SDL_MOUSEBUTTONUP:
				mouseDownIsAction = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				    case SDLK_x:
				        angleX -= 1;
				        break;
				    case SDLK_c:
                        camera->setAngleZ(camera->getAngleZ()-1);
				        break;
				    case SDLK_v:
				        //angleZ += 1;
				        camera->setAngleZ(camera->getAngleZ()+1);
				        break;
                    case SDLK_UP:
                        camera->setPosY(camera->getPosY()+1);
                        break;
                    case SDLK_DOWN:
                        camera->setPosY(camera->getPosY()-1);
                        break;
					case SDLK_LEFT:
						angle -= 0.1;
//                        testX -= 1;
                        camera->setPosX(camera->getPosX()-1);
						break;
					case SDLK_RIGHT:
//                        testX += 1;
                        camera->setPosX(camera->getPosX()+1);
						angle += 0.1;
						break;
					case SDLK_a:
						//left_right -= 1.0f;
                        camera->setEyeY(camera->getEyeY()-1);
						break;
					case SDLK_d:
						//left_right += 1.0f;
                        camera->setEyeY(camera->getEyeY()+1);
						break;
					case SDLK_s:
						up_down -= 1.0f;
                        camera->setEyeX(camera->getEyeX()-1);
						break;
					case SDLK_w:
						up_down += 1.0f;
                        camera->setEyeX(camera->getEyeX()+1);
						break;
                    case SDLK_q:
                        zoom += 0.9f;
                        break;
					case SDLK_z:
						zoom -= 0.9f;
						break;
					case SDLK_t:
						zoom += 0.8f;
						up_down -= 1.0f;
						break;
					case SDLK_g:
						zoom -= 1.0f;
						up_down += 0.8f;
						break;
					case SDLK_b:
						//zoom *= -1;
						//up_down *= 1;
						angle += 180;
						break;
				    case SDLK_F4:
                        camera->setCameraType(1);
                        break;
				    case SDLK_F5:
                        camera->setCameraType(2);
                        break;
				}
				break;
			case SDL_WINDOWEVENT_RESIZED:
				width = event.window.data1;
				height = event.window.data2;
				glViewport(0, 0, width, height);
				setup_opengl(true);
				// Vymazání okna
                glClear(GL_COLOR_BUFFER_BIT);
				glFlush();
				break;
			case SDL_QUIT:
				return;
				break;
		}
	}
	while(true);
}

void Engine::calculateFPS() {
	frame++;
	current_time = (unsigned int)glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = current_time - last_time;
	if(timeInterval > 1000) {
		fps = frame / (timeInterval / 1000.0f);
        last_time = current_time;
        frame = 0;
	}
}

void Engine::Render() {
	/*setup_opengl();*/
	char buffer[255];
	fString str("informace: ");
	str.append("posY: ");
    sprintf(buffer, "%d", camera->getPosY());
    str.append(buffer);
    str.append(", posX: ");
    sprintf(buffer, "%d", camera->getPosX());
	str.append(buffer);
	str.append(", eyeX: ");
    sprintf(buffer, "%f", camera->getEyeX());
	str.append(buffer);
    str.append(", eyeY: ");
    sprintf(buffer, "%f", camera->getEyeY());
    str.append(buffer);
    str.append(", angleZ: ");
    sprintf(buffer, "%f", camera->getAngleZ());
    str.append(buffer);
	/*str.append(" posX: ");
	itoa(left_right, buffer, 10);
	str.append(buffer);
	str.append(" posY: ");
	itoa(up_down, buffer, 10);
	str.append(buffer);*/
	info->setText((char*)str.getString());
	info->Render(width, height);
	gameField->Render2D();
	gameField->setLeftRight(&left_right);
	setup_opengl(true);
    gameField->Render();
    //info->Render();
}