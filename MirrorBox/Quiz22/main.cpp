/**
	Quiz3 
	Preston Goulet
	Taylor Greeff
	https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#include <stdlib.h>
#include <GL/glut.h>
#include <fstream>
#include "mesh.h"
#include "texture.h"
#include "render.h"
#include "controls.h"

 // global
Mesh *mesh1, *mesh2, *mesh3, *mesh4, *mesh5, *mesh6;
GLuint display1, display2, display3, display4, display5, display6;
GLuint textures[6];

// init
void init() {
	// mesh
	mesh1 = createPlane(2000, 2000, 200);
	mesh2 = createCubeMissing(10);
	mesh3 = createCube(1);
	mesh4 = createCube(1);
	mesh5 = createSkyBox(6000);
	mesh6 = createCubeFace(10);

	// normals
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerFace(mesh5);
	calculateNormalPerFace(mesh6);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	calculateNormalPerVertex(mesh5);
	calculateNormalPerVertex(mesh6);

	// textures
	loadBMP_custom(textures, "../BMP_files/brick.bmp", 0);
	//loadBMP_custom(textures, "../BMP_files/mirror.bmp", 1);
	codedTexture(textures, 2, 0); //Sky texture - noise multiscale. Type=0
	codedTexture(textures, 3, 1); //Marble texture - noise marble. Type=1
	loadBMP_custom(textures, "../BMP_files/cubesky.bmp", 4);
	loadBMP_custom(textures, "../BMP_files/reflection.bmp", 5);

	// display lists
	display1 = meshToDisplayList(mesh1, 1, textures[0]);
	display2 = meshToDisplayList(mesh2, 2, textures[1]);
	display3 = meshToDisplayList(mesh3, 3, textures[2]);
	display4 = meshToDisplayList(mesh4, 4, textures[3]);
	display5 = meshToDisplayList(mesh5, 5, textures[4]);
	display6 = meshToDisplayList(mesh6, 6, textures[5]);

	// configuration
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	// light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// STENCIL|STEP 2. NEW LINES
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearStencil(0); //define the value to use as clean.

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.75);
	glFogf(GL_FOG_START, 10.0);
	glFogf(GL_FOG_END, 3000);
}

// reshape
void reshape(int w, int h) {
	window_width = w;
	window_height = h;
}

// text
void renderBitmapString(float x, float y, float z, const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

// display
void display(void) {
	Vec3f mirrorPos;
	mirrorPos.x = 50;
	mirrorPos.y = -500;
	mirrorPos.z = -2500;

	// STENCIL-STEP 3. enable and configure
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);
	gluPerspective(45, window_ratio, 10, 100000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// lookAt
	// gluLookAt(0.0f, 40.0f, 320.0,	0.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f);

	gluLookAt(camera_x, camera_y, camera_z, camera_viewing_x, camera_viewing_y, camera_viewing_z, 0.0f, 1.0f, 0.0f);
	// camera
	//glScalef(scale, scale, scale);
	//glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	//glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, 0.0f);

	//=====================================
	//		Stencil
	//=====================================

	glEnable(GL_STENCIL_TEST); //Start using the stencil
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //Disable writing colors in frame buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); //Place a 1 where rendered
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); 	//Replace where rendered
														// PLAIN for the stencil
	

	glPushMatrix();	
	glTranslatef(mirrorPos.x, mirrorPos.y, mirrorPos.z);
	glCallList(display6); //mirror plane
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //Reenable color
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Keep the pixel

	// Trying to get the box to reflect with this. I moved the box 420 units in front of of the camera hoping that it would show up in the mirror
	// but something is wrong. Still working on it.
	glPushMatrix();
	glScalef(1.0, 1.0, -1.0);
	glTranslatef(camera_x - 50, camera_y - 150, camera_z + 1500);
	glCallList(display4); //mirrored box
	glPopMatrix();

	// STENCIL-STEP 4. disable it
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.7, 0.0, 0.0, 0.3);
	glColor4f(1.0, 1.0, 1.0, 0.3);

	// box 1
	glPushMatrix();
	glTranslatef(mirrorPos.x, mirrorPos.y, mirrorPos.z);
	glCallList(display6); //mirror
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	// Enable Culling
	glEnable(GL_CULL_FACE);

	// box 2
	glPushMatrix();
	glTranslatef(mirrorPos.x, mirrorPos.y, mirrorPos.z);
	glCallList(display2); //add rest of the mirror 
	glPopMatrix();

	// box 3
	glPushMatrix();
	glTranslatef(camera_x - 50, camera_y - 150, camera_z);
	glCallList(display4); //Box under camera
	glPopMatrix();


	//plane
	//glPushMatrix();
	//glTranslatef(-1000, 200, -1000);
	//glCallList(display1);
	//glPopMatrix();

	// end
	// skybox
	glPushMatrix();
	glTranslatef(-3000, -3000, -2000);
	glCallList(display5);
	glPopMatrix();
	// end
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// texto
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, window_height - 13.0f, 0.0f, "Use [Arrows] to move in plain");
	renderBitmapString(0.0, window_height - 26.0f, 0.0f, "Use [W and S] to look up and down");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDisable(GL_CULL_FACE);
	glutSwapBuffers();
}
// rotate what the user see
void rotate_point(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	// translate point back to origin:
	camera_viewing_x -= camera_x;
	camera_viewing_z -= camera_z;
	// rotate point
	float xnew = camera_viewing_x * c - camera_viewing_z * s;
	float znew = camera_viewing_x * s + camera_viewing_z * c;
	// translate point back:
	camera_viewing_x = xnew + camera_x;
	camera_viewing_z = znew + camera_z;
}

// callback function for keyboard (alfanumeric keys)
void callbackKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': case 'W':
		camera_viewing_y += (10);
		break;
	case 's': case 'S':
		camera_viewing_y -= (10);
		break;
	}
}

// callback function for arrows
void specialkeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		//total_moving_angle += -0.01;
		//rotate_point(-0.01);

		camera_x -= 10;
		camera_viewing_x -= 10;
	}
	else if (key == GLUT_KEY_RIGHT) {
		//total_moving_angle += 0.01;
		//rotate_point(0.01);
		camera_x += 10;
		camera_viewing_x += 10;
	}
	else if (key == GLUT_KEY_DOWN) {
		//printf("Down key is pressed\n");
		camera_z += 10;
		camera_viewing_z += 10;
		//camera_x += (-10) * sin(total_moving_angle);//*0.1;
		//camera_z += (-10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y -= 10;
		//camera_viewing_x += (-10) * sin(total_moving_angle);//*0.1;
		//camera_viewing_z += (-10) * -cos(total_moving_angle);//*0.1;
	}
	else if (key == GLUT_KEY_UP) {
		//printf("Up key is pressed\n");
		camera_z -= 10;
		camera_viewing_z -= 10;
		//camera_x += (10) * sin(total_moving_angle);//*0.1;
		//camera_z += (10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_x += (10) * sin(total_moving_angle);//*0.1;
		//camera_viewing_z += (10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y += 10;
	}
}

// main
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Sky Box");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
	return 0;
}