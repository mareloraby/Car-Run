#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include "Vector3f.h"
#include "Camera.h" 

#define GLUT_KEY_ESCAPE 27
#define LEFT_LANE -2
#define CENTER_LANE 0
#define RIGHT_LANE 2
#define GROUND_LENGTH 10000
#define RESPAWN_POSITION 200

using namespace std;

int lanes[3] = { LEFT_LANE,CENTER_LANE,RIGHT_LANE };

struct Shape;
float GAME_SPEED = 0.7;
float GAME_SPEED_OLD = 0.7;

int WIDTH = 1280;
int HEIGHT = 720;
GLuint tex;
char title[] = "Car Finite Run";

int timeSlowSpeedFail = 0;
float groundTransform = 0;
int wheel_rotation_angle;
int player_lane = 1;
int score = -1;
int score_pos = -30;
int lives_pos  = -30;
int stop = 1;
int lives = 3;
bool level2 = false;
bool level3 = false;
int timeElapsed = -1;
int level1time = 20;
int level2time = 30;
int level3time = 30;
bool gameWon = false;
bool gameLost = false;
bool hitObstacle = false;
bool hitPowerUP = false;
bool gainedPowerUp = false;
int timePowerFail = 0;
int cameraZoom = 0;

vector<Shape> obstacles;
vector<Shape> wheels;

struct Shape {
	float x;
	int lane;

	Shape(float x, int lane) {
		this->x = x, this->lane = lane;
	};
};

Camera camera = Camera(0.5f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//*******************************************************************************************//
// EYE (ex, ey, ez): defines the location of the camera.									 //
// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
//*******************************************************************************************//

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1280 / 720, 0.001, 200);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

// Model Variables
Model_3DS model_house;
Model_3DS model_car;
Model_3DS stone_model;
Model_3DS wheel_model;
Model_3DS palm_model;
Model_3DS house_model;
Model_3DS tree_model;
Model_3DS box_model;
Model_3DS boat_model;

// Textures
GLTexture tex_ground;
GLTexture tex_surface;
GLTexture tex_wood;

void print(int x, int y, char *string)
{
	int len, i;
	glColor3f(0, 0, 0);	
	if (gameWon) {
		glColor3f(1, 1, 1);
	}
	if (gameLost) {
		glColor3f(1, 0, 0);
	}
	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}
//=======================================================================
// Lighting Configuration Function
//=======================================================================

float lposx =2.0f;
float lposy =3.0f;
float lposz =2.0f;

float lambientr = 0.1f;
float lambientg = 0.1f;
float lambientb = 0.1f;

bool cueRedSunset = false;
bool cueDarkerSunset = false;
bool cueDarkness = false;


int random(int lower, int upper)
{

	return (rand() % (upper - lower + 1)) + lower;
}



void InitLightSource()
{
	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0Ambient[] = { lambientr, lambientg, lambientb, 0.0f };
	GLfloat l0Position[] = { lposx, lposy, lposz, 0};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);

}

void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with tescorexture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -3);
	glTexCoord2f(200, 0);
	glVertex3f(GROUND_LENGTH, 0, -3);
	glTexCoord2f(200, 3);
	glVertex3f(GROUND_LENGTH, 0, 3);
	glTexCoord2f(0, 3);
	glVertex3f(-20, 0, 3);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderSurface()
{
	if(!cueDarkerSunset || level2) glDisable(GL_LIGHTING);	// Disable lighting
	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	glBindTexture(GL_TEXTURE_2D, tex_surface.texture[0]);	// Bind the ground texture
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-200, 0, -200);
	glTexCoord2f(100, 0);
	glVertex3f(GROUND_LENGTH, 0, -200);
	glTexCoord2f(100, 10);
	glVertex3f(GROUND_LENGTH, 0, 200);
	glTexCoord2f(0, 10);
	glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();
	if (!cueDarkerSunset || level2) glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderSkyBox() {
	//sky box
//	glDisable(GL_LIGHTING);	// Disable lighting 
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();
//	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.


}

void renderWheel(float x, float lane) {

	glDisable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	//Draw Wheels
	glPushMatrix();
	glTranslatef(x + 5, 1, lane);
	glScalef(0.01, 0.01, 0.01);
	glRotatef(wheel_rotation_angle, 0, 1, 0);
	wheel_model.Draw();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.

}

void renderObstacle(float x, float lane)
{


	glDisable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
    


	if (!level2 && !level3) {
		glPushMatrix();
		glTranslatef(x + 5, 0.35, lane);
		glScalef(3, 3.5, 2);
		stone_model.Draw();
		glPopMatrix();


		// draw palm trees
		glPushMatrix();
		glTranslatef(x + 5, 0.35, lane + 10);
		glScalef(0.01, 0.01, 0.01);
		palm_model.Draw();
		glPopMatrix();

		// draw palm trees
		glPushMatrix();
		glTranslatef(x + 5, 0.35, lane - 15);
		glScalef(0.01, 0.01, 0.01);
		palm_model.Draw();
		glPopMatrix();
	}
	
	else if(level2 && !level3){

		glPushMatrix();
		glTranslatef(x + 5, 1, lane);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(90, 0, 1, 0);
		box_model.Draw();
		glPopMatrix();
		
		glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
		glPushMatrix();
		glTranslatef(x + 5, 3.7, lane + 15);
		glScalef(0.0009, 0.0009, 0.0009);
		house_model.Draw();
		glPopMatrix();
		glDisable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.


		// draw trees
		glPushMatrix();
		glTranslatef(x + 5, 3.6, lane - 15);
		glScalef(0.001, 0.001, 0.001);
		glRotatef(180, 0, 1, 0);
		tree_model.Draw();
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glTranslatef(x + 5, 0.35, lane);
		glScalef(3, 3.5, 2);
		stone_model.Draw();
		glPopMatrix();


		// draw palm trees
		glPushMatrix();
		glTranslatef(x + 5, 1.9, lane + 12);
		glScalef(0.03, 0.03, 0.03);
		glRotatef(90, 1, 0, 0);
		boat_model.Draw();
		glPopMatrix();

		// draw palm trees
		glPushMatrix();
		glTranslatef(x + 5, 1.9, lane - 15);
		glScalef(0.03, 0.03, 0.03);
		glRotatef(90, 1, 0, 0);
		boat_model.Draw();
		glPopMatrix();


	}

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
	
}

void addObstacle(int lane)
{
	obstacles.push_back(Shape(RESPAWN_POSITION, lane));
}// adds an obstacle behind the skybox

void addWheel(int lane)
{
	wheels.push_back(Shape(RESPAWN_POSITION, lane));
}

void destroyAtIndex(int index, vector<Shape> &shapes)
{
	// Swap this element with the last one to pop from the vector
	Shape tmp = shapes[shapes.size() - 1];
	shapes[shapes.size() - 1] = shapes[index];
	shapes[index] = tmp;
	shapes.pop_back();
}

void onObstacleCollision()
{
	if (!gainedPowerUp) {
		if (!hitObstacle) {
			PlaySound(TEXT("audios/Crash.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			hitObstacle = false;
		}

		lives--;
		score = score - 10;
	}
	if (lives == 0)
	{
		//if lives are equal to zero 
		gameLost = true;
		//prnt game over
		glFlush();
	}

}

void onWheelCollision(int i)
{
	glFlush();
	glutSwapBuffers();
	if (!hitPowerUP) {
		PlaySound(TEXT("audios/powerup.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
		hitPowerUP = false;
	}

	int randomPwr = random(1,3);
	cout << randomPwr << endl;

	switch (randomPwr) {
	case 1: gainedPowerUp = true; timePowerFail = timeElapsed + 5; break;
	case 2: if (lives < 3) lives++; break;
	case 3: GAME_SPEED = 0.5; timeSlowSpeedFail = timeElapsed + 5; break;

	}
}

//=======================================================================
// Display Function
//=======================================================================

void myDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupCamera();
	InitLightSource();
	InitMaterial();

	// Display Score
	char* strScore[20];
	char* strLives[20];
	char* gameLostStr[20];


	// score display
	glPushMatrix();
	if (player_lane == 0)
	{
		glTranslatef(-10, score_pos, 10);
	}
	else if (player_lane == 1)
	{

		glTranslatef(-10, score_pos, 12);

	}
	else {
		glTranslatef(-10, score_pos, 14);

	}

	glDisable(GL_LIGHTING);	// Disable lighting 
	sprintf((char *)strScore, "Score : %d", score);
	print(50, 50, (char *)strScore);
	glEnable(GL_LIGHTING);

	glPopMatrix();


	// lives display
	glPushMatrix();
	if (player_lane == 0)
	{
		glTranslatef(-10, lives_pos, -20);
	}
	else if (player_lane == 1)
	{

		glTranslatef(-10, lives_pos, -18);

	}
	else {
		glTranslatef(-10, lives_pos, -16);

	}
	
	int k = 0;
	char livesString[] = "Lives = %d/%d";
	glDisable(GL_LIGHTING);	// Disable lighting 
	sprintf((char*)strLives, livesString,lives,3);
	print(50, 50, (char*)strLives);
	glEnable(GL_LIGHTING);
	/*for (int i = 0; i < lives; i++)
	{
		glTranslatef(-10, lives_pos, -16+k);
		print(60, 50, "1");
		k += 5;
	}*/
	glPopMatrix();


	glPushMatrix();
	if (player_lane == 0)
	{
		glTranslatef(-10, lives_pos, -3);
	}
	else if (player_lane == 1)
	{

		glTranslatef(-10, lives_pos, -1);

	}
	else {
		glTranslatef(-10, lives_pos, 1);

	}

	if (gameLost) {
		glDisable(GL_LIGHTING);	// Disable lighting 
		sprintf((char*)gameLostStr, "Game Over");
		print(50, 50, (char*)gameLostStr);
		glEnable(GL_LIGHTING);
	}
	if (gameWon) {
		glDisable(GL_LIGHTING);	// Disable lighting 
		sprintf((char*)gameLostStr, "You Won!!");
		print(50, 50, (char*)gameLostStr);
		glEnable(GL_LIGHTING);
	}

	if (!level2 && !level3 && !gameWon && !gameLost) {
		glDisable(GL_LIGHTING);	// Disable lighting 
		sprintf((char*)gameLostStr, "Level 1");
		print(50, 50, (char*)gameLostStr);
		glEnable(GL_LIGHTING);
	}
	if (level2 && !gameWon && !gameLost) {
		glDisable(GL_LIGHTING);	// Disable lighting 
		sprintf((char*)gameLostStr, "Level 2");
		print(50, 50, (char*)gameLostStr);
		glEnable(GL_LIGHTING);
	}
	if (level3 && !gameWon && !gameLost) {
		glDisable(GL_LIGHTING);	// Disable lighting 
		sprintf((char*)gameLostStr, "Level 3");
		print(50, 50, (char*)gameLostStr);
		glEnable(GL_LIGHTING);
	}





	glPopMatrix();



	// Display Level
	glPushMatrix();
	glTranslated(groundTransform * 0.2, 0, 0);
	RenderSurface();
	glPopMatrix();

	// Draw Street
	glPushMatrix();
	glTranslated(groundTransform, 0.5, 0);
	RenderGround();
	glPopMatrix();


	// Draw Player
	glPushMatrix();
	glTranslatef(0, 0.9, lanes[player_lane]);
	glScalef(0.02, 0.02, 0.02);
	glRotatef(90.f, 1, 0, 0);
	glRotatef(-90.f, 0, 0, 1);
	glDisable(GL_LIGHTING);	// Disable lighting 
	model_car.Draw();
	glEnable(GL_LIGHTING);	// Disable lighting 
	glPopMatrix();

	//Draw all Wheels(powerup)
	for (unsigned i = 0; i < wheels.size(); i++)
	{
		renderWheel(wheels[i].x, lanes[wheels[i].lane]);
	}

	// Draw all obstacles
	for (unsigned i = 0; i < obstacles.size(); i++)
	{
		renderObstacle(obstacles[i].x, lanes[obstacles[i].lane]);
	}

	
	if (timeElapsed == level1time && lives != 0) { //go to level 2
		glutSwapBuffers();
		tex_surface.Load("Textures/grasstext.bmp");
		level2 = true;

		GAME_SPEED = 0.9; //game speed increases
		GAME_SPEED_OLD = 0.9;

		for (int i = 0; i < obstacles.size(); i++)
		{
			obstacles[i].x -= 200;
		}
		for (int i = 0; i < wheels.size(); i++)
		{
			wheels[i].x -= 200;
		}
	}
	else if (timeElapsed == (level1time + level2time) && lives != 0) { // go to level 3
		
		//glutSwapBuffers();
		 GAME_SPEED = 0.8;
		 GAME_SPEED_OLD = 0.8;
		tex_surface.Load("Textures/sea.bmp");
		
		level2 = false;
		level3 = true;

		for (int i = 0; i < obstacles.size(); i++)
		{
			obstacles[i].x -= 200;
		}
		for (int i = 0; i < wheels.size(); i++)
		{
			wheels[i].x -= 200;
		}

	}
	else if (timeElapsed == (level1time + level2time+ level3time) && lives != 0)
	{
		stop = 0;
		gameWon = true;
		//exit(EXIT_SUCCESS);
	}
	//drawRain();

	RenderSkyBox();
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.

	glFlush();
}
//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_car.Load("Models/car3/Car.3ds");
	stone_model.Load("Models/stone/Stone 2.3DS");
	wheel_model.Load("Models/wheel/wheel.3DS");
	palm_model.Load("Models/palmTree/palm.3DS");
	house_model.Load("Models/House9/House/House.3DS");
	tree_model.Load("Models/tree1/tree.3DS");
	boat_model.Load("Models/yacht1/Boat.3DS");
	box_model.Load("Models/waste/Bin Polymer trash bin BM-5000 N080818.3DS");


	// Loading texture files
	
	tex_ground.Load("Textures/ground.bmp");
	
	tex_surface.Load("Textures/surface.bmp");

	tex_wood.Load("Textures/wall.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}
//=======================================================================
// Animation Function
//=======================================================================
void anim()

{

	if ( !gameLost && !gameWon) {
		score += 1;
	
		//light anim

		if (timeElapsed == level1time/5) {
			cueRedSunset = true;
		}
		if (timeElapsed == level1time * (2/3)) {
			cueDarkerSunset = true;
		}
		if (timeElapsed == (level1time))
		{
			cueDarkness = true;
		}


		if (cueRedSunset) {
			if (lposx > 0.5) lposx = lposx - 0.0015;
			if (lambientr < 0.25) lambientr = lambientr + 0.002;
			else {
				cueRedSunset = false;
			}
		}

		if (cueDarkerSunset) {
			if (lposx > -0.2) lposx = lposx - 0.0005;
			if (lposz > -3) lposz = lposz - 0.005;
			if (lposy > 0.5) lposy = lposy - 0.005;
			if (lambientb < 0.2) lambientb = lambientb + 0.0001;
		

		}

		if (cueDarkness) {
			if (lambientb < 0.2) lambientb = lambientb + 0.0004;
			if (lambientr > 0.05) lambientr = lambientr - 0.0005;
			lposx = lposx - 0.005;
			if (lposy > -4) lposy = lposy - 0.005;	
			if (lposz > -5) lposz = lposz - 0.005;
	
		}
	
		wheel_rotation_angle += 5 * stop;
		for (int i = 0; i < obstacles.size(); i++)
		{
			obstacles[i].x -= GAME_SPEED * stop;

			// If player collided with obstacle
			if (obstacles[i].lane == player_lane &&
				obstacles[i].x <= 1 && obstacles[i].x >= 0)
			{
				onObstacleCollision();
			}

			// If the obstacle is way behind the player
			if (obstacles[i].x < -20)
				destroyAtIndex(i--, obstacles);
		}



		for (int i = 0; i < wheels.size(); i++)
		{
			wheels[i].x -= GAME_SPEED * stop;

			// If player collided with wheel
			if (wheels[i].lane == player_lane &&
				wheels[i].x <= 0.9 && wheels[i].x >= 0)
			{
				onWheelCollision(i);
				destroyAtIndex(i--, wheels);
			}
		}

		for (int i = 0; i < wheels.size(); i++)
		{
			// If the wheel is way behind the player
			if (wheels[i].x < -20 && wheels.size() > 0)
				destroyAtIndex(i--, wheels);
		}

		groundTransform -= GAME_SPEED * stop;

		for (int i = 0; i < 1e7; i++);
		glutPostRedisplay();

	}
	
}

bool top = false;
bool front = true;

void actM(int button, int state, int x, int y)//mouse function takes 4 parameters: button: which button has been clicked (GLUT_RIGHT_BUTTON or GLUT_LEFT_BUTTON),
											//state wether the button is clicked or released (GLUT_UP or GLUT_DOWN)
											// x and y are the position of the mouse cursor
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//if the left button has been clicked then translate the square to the mouse position
	{
		//top
		top = true;
		front = false;
		score_pos = -48.5;
		lives_pos = -48.5;
		camera = Camera(-8.0f, 7.0f, lanes[player_lane], -1.0f, 2.7f, lanes[player_lane], 0.0f, 1.0f, 0.0f);
	
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//if the right button has been clicked, translate  the square to the origin (0,0)
	{ //front
		top = false;
		front = true;
		score_pos = -30;
		lives_pos = -30;
		camera = Camera(0.5f, 2.0f, lanes[player_lane], 1.0f, 2.0f, lanes[player_lane], 0.0f, 1.0f, 0.0f);;
	}

	glutPostRedisplay();//redisplay to update the screen with the new paraeters
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.8;
	float x_car_cam = 2;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'd':
		if (player_lane < 2)
		{
			player_lane++;
			camera.moveX(-x_car_cam);
		}
		break;
	case 'a':
		if (player_lane > 0)
		{
			player_lane--;
			camera.moveX(x_car_cam);
		}
		break;
	case 'q':
		if(!front) camera.moveZ(d);
		break;
	case 'e':
		if (!front) camera.moveZ(-d);
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
// camera controls
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void dropStone(int v)
{
	boolean dropAllowed = random(0, 100) < 70;

	if (dropAllowed)
	{
		int lane = random(0, 2);
		addObstacle(lane);
	}
	glutTimerFunc(750, dropStone, 0);
}

void dropWheel(int v)
{
	boolean dropAllowed = random(0, 100) < 70;

	if (dropAllowed)
	{
		int lane = random(0, 2);
		addWheel(lane);
	}
	glutTimerFunc(4000, dropWheel, 0);
}

void Timers(int value) {

	timeElapsed +=1;

	if (timeElapsed > timePowerFail) {
		
		gainedPowerUp = false;

	}

	if (timeElapsed > timeSlowSpeedFail) {
	
		GAME_SPEED = GAME_SPEED_OLD;
	}
	//cout << timeElapsed << endl;
	glutTimerFunc(900, Timers, 0);
	glutPostRedisplay();
}
//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 150);
	glutCreateWindow(title);
	glutDisplayFunc(myDisplay);
	glutIdleFunc(anim);
	glutTimerFunc(0, dropStone, 0);
	glutTimerFunc(0, dropWheel, 0);
	glutTimerFunc(0, Timers, 0);


	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutMouseFunc(actM);			//call the mouse function
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}


