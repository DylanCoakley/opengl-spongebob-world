#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <windows.h>
    #include <GL/glut.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "rgbimage.h"

#define PI 3.14159

#define GROUND_WIDTH 100
#define GROUND_HEIGHT 0.5
#define ROAD_HEIGHT 0.5

static GLfloat theta[] = {0.0, 0.0, 0.0};
static GLint axis = 0;
GLUquadricObj *q;

// Attributes to modify the view
GLfloat viewTheta = 0.5, viewPhi = 2.0;
GLfloat viewRadius = 0.75;
GLfloat origX = 1.0;
GLfloat origY = 0.0;
GLfloat origZ = 0.0;
GLfloat pointX, pointY, pointZ;
bool onTrajectory = false;

float snailX = 7.0;
float snailZ = 1.0;

GLfloat black[] = {0.0, 0.0, 0.0, 0.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightYellow[] = {0.8, 0.8, 0.0};

/*
    Note use of push/pop to return modelview matrix
    to its state before functions were entered and use
    rotation, translation, and scaling to create instances
    of symbols
*/

// Read a texture map from a BMP bitmap file.
void loadTextureFromFile(char *filename)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	RgbImage theTexMap( filename );

	// Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
	//    Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,theTexMap.GetNumCols(), theTexMap.GetNumRows(),
					 GL_RGB, GL_UNSIGNED_BYTE, theTexMap.ImageData() );

}

// Draws the sandy ground of the world
void sandFloor()
{
    //set properties of the sandy ground
    GLfloat sandAmbient[] = {1.0, 0.85, 0.5, 1.0};
    GLfloat sandDiffuse[] = {1.0, 0.85, 0.5, 1.0};
    GLfloat sandSpecular[] = {1.0, 0.85, 0.5, 1.0};
    GLfloat sandShininess[] = {20.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, sandAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sandDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sandSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, sandShininess);

    // draw the ground and map a sandy texture
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glScalef(1.25, 1.0, 1.25);
    glBegin(GL_QUADS);
        // Top face
        glTexCoord2f(0.0, 0.0);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glTexCoord2f(0.0, 1.0);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(20.0, 0.0, -20.0);
        glTexCoord2f(1.0, 1.0);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(20.0, 0.0, 20.0);
        glTexCoord2f(1.0, 0.0);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-20.0, 0.0, 20.0);

        // Bottom face
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-20.0, -20.0, -20.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(20.0, -20.0, -20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(20.0, -20.0, 20.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(-20.0, -20.0, 20.0);

        // Front face
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-20.0, -20.0, 20.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-20.0, 0.0, 20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(20.0, 0.0, 20.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(20.0, -20.0, 20.0);

        // Back face
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-20.0, -20.0, -20.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(20.0, 0.0, -20.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(20.0, -20.0, -20.0);

        // Left face
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-20.0, -20.0, 20.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-20.0, 0.0, 20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(-20.0, 0.0, -20.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(-20.0, -20.0, -20.0);

        // Right face
        glTexCoord2f(0.0, 0.0);
        glVertex3f(20.0, -20.0, -20.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(20.0, 0.0, -20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(20.0, 0.0, 20.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(20.0, -20.0, 20.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

}

// Draw the main road
void roadway()
{
    //set properties of the road
    GLfloat roadAmbient[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat roadDiffuse[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat roadSpecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat roadShininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, roadAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, roadDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, roadSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, roadShininess);

    // draw the road
    glPushMatrix();
    glTranslatef(0.0, 0.1, 10.0);
    glScalef(GROUND_WIDTH, 1.0, 10.0);
    glutSolidCube(GROUND_HEIGHT);
    glPopMatrix();
}

// Draw the walkway to the left house
void leftWalkway()
{
    glPushMatrix();
    glTranslatef(-10.0, 0.1, 2.5);
    glScalef(4.0, 1.0, 20.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glutSolidCube(ROAD_HEIGHT);
    glPopMatrix();
}

// Draw the walkway to the middle house
void middleWalkway()
{
    //set properties of wood
    GLfloat woodAmbient[] = {0.3, 0.2, 0.0, 1.0};
    GLfloat woodDiffuse[] = {0.3, 0.2, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, woodAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, woodDiffuse);

    glPushMatrix();
    glTranslatef(0.0, 0.1, -1.5);
    glScalef(5.0, 1.0, 1.5);

    for(int i = 0; i < 4; i++)
    {
        glutSolidCube(ROAD_HEIGHT);
        glTranslatef(0.0, 0.0, 1.5);
    }

    glPopMatrix();
}

// Draw the walkway to the right house
void rightWalkway()
{
    glPushMatrix();
    glTranslatef(10.0, 0.1, 2.5);
    glScalef(4.0, 1.0, 20.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glutSolidCube(ROAD_HEIGHT);
    glPopMatrix();
}

// Draw the house on the left
void rockHouse()
{
    glPushMatrix();
    glTranslatef(-10.0, 0.5, -5.5);
    glScalef(3.0, 5.0, 3.0);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();
}

// Draw the middle house
void faceHouse()
{
    //set properties of the face house
    GLfloat faceAmbient[] = {0.0, 0.0, 0.3, 1.0};
    GLfloat faceDiffuse[] = {0.0, 0.2, 0.4, 1.0};
    GLfloat faceSpecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat faceShininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, faceAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, faceDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, faceSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, faceShininess);

    // Base of the house
    glPushMatrix();
    glTranslatef(0.0, 0.0, -5.5);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(q, 2.5, 2.0, 12.0, 20.0, 20.0);
    glPopMatrix();

    // Nose
    glPushMatrix();
    glTranslatef(0.0, 6.5, -3.5);
    glScalef(1.0, 4.0, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Eyebrow
    glPushMatrix();
    glTranslatef(0.0, 8.5, -3.5);
    glScalef(3.0, 1.0, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left ear
    glPushMatrix();
    glTranslatef(-2.25, 6.5, -5.5);
    glScalef(1.0, 4.0, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right ear
    glPushMatrix();
    glTranslatef(2.25, 6.5, -5.5);
    glScalef(1.0, 4.0, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Roof
    glPushMatrix();
    glTranslatef(0.0, 12.0, -5.5);
    glScalef(1.0, 0.2, 1.0);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();

    //set properties of wood
    GLfloat woodAmbient[] = {0.3, 0.2, 0.0, 1.0};
    GLfloat woodDiffuse[] = {0.3, 0.2, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, woodAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, woodDiffuse);

    // Door
    glPushMatrix();
    glTranslatef(0.0, 1.5, -3.15);
    glScalef(0.8, 2.5, 0.2);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    GLfloat knobAmbient[] = {1.0, 1.0, 0.0, 1.0};
    GLfloat knobDiffuse[] = {1.0, 1.0, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, knobAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, knobDiffuse);

    // Doorknob
    glPushMatrix();
    glTranslatef(0.5, 1.75, -3.0);
    glScalef(0.5, 1.0, 0.5);
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    // set properties of metal
    GLfloat metalAmbient[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat metalDiffuse[] = {0.7, 0.7, 0.7, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, metalAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metalDiffuse);

    // Left window border
    glPushMatrix();
    glTranslatef(1.0, 7.25, -3.4);
    glScalef(1.0, 1.5, 1.2);
    glutSolidTorus(0.1, 0.4, 20, 20);
    glPopMatrix();

    // Right window border
    glPushMatrix();
    glTranslatef(-1.0, 7.25, -3.4);
    glScalef(1.0, 1.5, 1.2);
    glutSolidTorus(0.1, 0.4, 20, 20);
    glPopMatrix();

    // set properties of window
    GLfloat windowAmbient[] = {0.7, 0.9, 1.0, 1.0};
    GLfloat windowDiffuse[] = {0.7, 0.9, 1.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, windowAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, windowDiffuse);

    // Left window
    glPushMatrix();
    glTranslatef(1.0, 7.25, -3.4);
    glScalef(3.5, 4.5, 1.0);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // Right window
    glPushMatrix();
    glTranslatef(-1.0, 7.25, -3.4);
    glScalef(3.5, 4.5, 1.0);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
}

// Draw the house on the right
void pineappleHouse()
{
    //set properties of the pineapple house
    GLfloat pineAmbient[] = {1.0, 0.6, 0.0, 1.0};
    GLfloat pineDiffuse[] = {1.0, 0.6, 0.0, 1.0};
    GLfloat pineSpecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pineShininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, pineAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pineDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pineSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, pineShininess);

    // Base of the house
    glPushMatrix();
    glTranslatef(10.0, 0.5, -5.5);
    glScalef(3.0, 10.0, 3.0);
    glutSolidSphere(1.0, 35.0, 30.0);
    glPopMatrix();

    // set properties of metal
    GLfloat metalAmbient[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat metalDiffuse[] = {0.7, 0.7, 0.7, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, metalAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metalDiffuse);

    // Door
    glPushMatrix();
    glTranslatef(10.0, 1.5, -2.6);
    glScalef(0.8, 2.5, 0.2);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    // Left window border
    glPushMatrix();
    glTranslatef(8.8, 6.5, -3.5);
    glRotatef(-10, 1.0, 0.0, 0.0);
    glRotatef(-30, 0.0, 1.0, 0.0);
    glScalef(1.0, 1.5, 1.2);
    glutSolidTorus(0.1, 0.4, 20, 20);
    glPopMatrix();

    // Right window border
    glPushMatrix();
    glTranslatef(12.0, 3.0, -3.4);
    glRotatef(35, 0.0, 1.0, 0.0);
    glScalef(1.0, 1.5, 1.2);
    glutSolidTorus(0.1, 0.4, 20, 20);
    glPopMatrix();

    // set properties of window
    GLfloat windowAmbient[] = {0.7, 0.9, 1.0, 1.0};
    GLfloat windowDiffuse[] = {0.7, 0.9, 1.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, windowAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, windowDiffuse);

    // Left window
    glPushMatrix();
    glTranslatef(8.75, 6.55, -3.45);
    glRotatef(-10, 1.0, 0.0, 0.0);
    glRotatef(-30, 0.0, 1.0, 0.0);
    glScalef(3.5, 5.0, 1.0);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // Right window
    glPushMatrix();
    glTranslatef(12.0, 3.0, -3.4);
    glRotatef(35, 0.0, 1.0, 0.0);
    glScalef(3.5, 5.0, 1.0);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // set properties of the leaves
    GLfloat leafAmbient[] = {0.0, 0.4, 0.0, 1.0};
    GLfloat leafDiffuse[] = {0.0, 0.4, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, leafAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leafDiffuse);

    // Create bottom leaves using polar coordinates
    for(double angle = 0.0; angle < 2 * PI; angle += PI / 3.0)
    {
        glPushMatrix();
        glTranslatef(1.2 * cos(angle), 0.0, 1.2 * sin(angle));
        glTranslatef(10.0, 9.5, -5.5);
        glRotatef(270 - (angle * 180 / PI), 0.0, 1.0, 0.0);
        glRotatef(255, 1.0, 0.0, 0.0);
        glScalef(0.75, 0.25, 1.5);
        gluCylinder(q, 1.0, 0.01, 3.0, 20.0, 20.0);
        glPopMatrix();
    }

    // Create middle leaves using polar coordinates
    for(double angle = 0.0; angle < 2 * PI; angle += PI / 6.0)
    {
        glPushMatrix();
        glTranslatef(1.2 * cos(angle), 0.0, 1.2 * sin(angle));
        glTranslatef(10.0, 10.0, -5.5);
        glRotatef(270 - (angle * 180 / PI), 0.0, 1.0, 0.0);
        glRotatef(255, 1.0, 0.0, 0.0);
        glScalef(0.75, 0.25, 1.5);
        gluCylinder(q, 0.75, 0.01, 3.5, 20.0, 20.0);
        glPopMatrix();
    }

    // Create top leaves using polar coordinates
    for(double angle = 0.0; angle < 2 * PI; angle += PI / 12.0)
    {
        glPushMatrix();
        glTranslatef(0.8 * cos(angle), 0.0, 0.8 * sin(angle));
        glTranslatef(10.0, 10.0, -5.5);
        glRotatef(270 - (angle * 180 / PI), 0.0, 1.0, 0.0);
        glRotatef(255, 1.0, 0.0, 0.0);
        glScalef(0.75, 0.25, 1.5);
        gluCylinder(q, 0.75, 0.01, 3.5, 20.0, 20.0);
        glPopMatrix();
    }
}

// draw the moving snail
void snail()
{
    //set properties of the snail body
    GLfloat bodyAmbient[] = {0.0, 0.8, 1.0, 1.0};
    GLfloat bodyDiffuse[] = {0.6, 0.6, 1.0, 1.0};
    GLfloat bodySpecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat bodyShininess[] = {20.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, bodyAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bodySpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, bodyShininess);

    // Bottom part of body
    glPushMatrix();
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glScalef(2.5, 1.0, 1.0);
    glutSolidTorus(0.1, 0.2, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glScalef(2.5, 1.0, 1.0);
    glutSolidSphere(0.15, 10, 10);
    glPopMatrix();

    // Eye stems
    glPushMatrix();
    glTranslatef(-0.4, 1.8, -0.1);
    glScalef(1.0, 18.0, 1.0);
    glutSolidCube(0.08);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.4, 1.8, 0.1);
    glScalef(1.0, 18.0, 1.0);
    glutSolidCube(0.08);
    glPopMatrix();

    // set white colour for eyes
    glMaterialfv(GL_FRONT, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);

    // Right eye
    glPushMatrix();
    glTranslatef(-0.4, 2.5, -0.15);
    glutSolidSphere(0.175, 10, 10);
    glPopMatrix();

    // Left eye
    glPushMatrix();
    glTranslatef(-0.4, 2.5, 0.15);
    glutSolidSphere(0.175, 10, 10);
    glPopMatrix();

    // set black colour for pupils
    glMaterialfv(GL_FRONT, GL_AMBIENT, black);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, black);

    // Right pupil
    glPushMatrix();
    glTranslatef(-0.5, 2.55, -0.15);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();

    // Left pupil
    glPushMatrix();
    glTranslatef(-0.5, 2.55, 0.15);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();

    // set pink colour for shell
    GLfloat shellAmbient[] = {1.0, 0.42, 0.8, 1.0};
    GLfloat shellDiffuse[] = {1.0, 0.42, 0.8, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, shellAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, shellDiffuse);

    // Shell
    glPushMatrix();
    glTranslatef(0.2, 1.2, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glScalef(1.75, 1.0, 1.0);
    glutSolidTorus(0.1, 0.2, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 2.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glScalef(2.5, 2.0, 5.0);
    glutSolidSphere(0.2, 10, 10);
    glPopMatrix();
}

// Idle function used to rotate camera
void idle()
{
    viewPhi += 0.001;

    glutPostRedisplay();
}

void display()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20.0, 20.0, -20.0, 20.0, -20.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    pointX = viewRadius * cos(viewTheta) * cos(viewPhi);
    pointY = viewRadius * sin(viewTheta);
    pointZ = -viewRadius * cos(viewTheta) * sin(viewPhi);
/*
    gluLookAt(
          1.0, 2.5, 2.0, // current position of camera
          0.0, 0.0, 0.0, // look-at point
          0.0, 1.0, 0.0     // up vector
    );
    */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(
        origX, origX, origZ,  // location of eye
        origX + pointX, origY + pointY, origZ + pointZ, // look-at point
        0.0, 1.0, 0.0   // up vector
    );

    /*
    // Adjust lighting
    GLfloat lightX = origX;
    GLfloat lightY = origY;
    GLfloat lightZ = origZ;
    GLfloat lightPosition[] = {lightX, lightY, lightZ, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
*/

    // draw the sandy ground
    sandFloor();

    // draw the road and walkways
    roadway();
    leftWalkway();
    rightWalkway();
    middleWalkway();

    // draw the houses
    rockHouse();
    pineappleHouse();
    faceHouse();

    // draw the movable snail
    glTranslatef(snailX, 0.0, snailZ);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    snail();
/*
    // Make blue balloon
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(0.0, MAX_HEIGHT, 0.0);
    glRotatef(theta[0], 0.0, 1.0, 0.0);
    balloon();

    // Draw lip of the balloon
    glTranslatef(0.0, -BALLOON_HEIGHT, 0.0);
    glRotatef(theta[0], 0.0, 1.0, 0.0);
    lip();

    // Make black string
    glColor3f(0.0, 0.0, 0.0);

    // Draw upper part of string
    glRotatef(theta[1], 1.0, 0.0, 0.0);
    upperString();

    // Draw lower part of string
    glTranslatef(0.0, -UPPER_STRING_HEIGHT, 0.0);
    glRotatef(theta[2], 1.0, 0.0, 0.0);
    lowerString();
*/
    glFlush();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    // Move camera around central point
    if(key == 'M' or key == 'm')
    {
        if(onTrajectory)
        {
            glutIdleFunc(NULL);
            onTrajectory = false;
        }
        else
        {
            glutIdleFunc(idle);
            onTrajectory = true;
        }
    }

    // Move snail forward
    if(key == 'W' or key == 'w')
    {
        if(axis == 1)
        {
            snailX += 0.05 * sin((theta[1] - 90) * PI / 180.0);
            snailZ += 0.05 * cos((theta[1] - 90) * PI / 180.0);
        }
    }

    // Move snail backwards
    if(key == 'S' or key == 's')
    {
       if(axis == 1)
        {
            snailX -= 0.05 * sin((theta[1] - 90) * PI / 180.0);
            snailZ -= 0.05 * cos((theta[1] - 90) * PI / 180.0);
        }
    }
    // Turn snail to the left
    if(key == 'A' or key == 'a')
    {
        theta[axis] += 5.0;
        if(theta[axis] > 360.0)
            theta[axis] -= 360.0;
    }

    // Turn snail to the right
    if(key == 'D' or key == 'd')
    {
        theta[axis] -= 5.0;
        if(theta[axis] < 360.0)
            theta[axis] += 360.0;
    }

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    switch(key)
    {
        // Camera left
        case GLUT_KEY_LEFT:
            viewPhi += 0.05;
        break;
        // Camera right
        case GLUT_KEY_RIGHT:
            viewPhi -= 0.05;
        break;
        // Move camera forward
        case GLUT_KEY_UP:
            origX += pointX;
            origZ += pointZ;
        break;
        // Move camera backwards
        case GLUT_KEY_DOWN:
            origX -= pointX;
            origZ -= pointZ;
        break;
        // Camera look up
        case GLUT_KEY_PAGE_UP:
            if(viewTheta < ((PI / 2.0) - 0.1))
                viewTheta += 0.05;
        break;
        // Camera look down
        case GLUT_KEY_PAGE_DOWN:
            if(viewTheta > ((-PI / 2.0) + 0.1))
                viewTheta -= 0.05;
        break;
    }

    glutPostRedisplay();
}

void menu(int id)
{
    // Move the eyes of the snail
    if(id == 1)
        axis = 0;
    // Move the body of the snail
    if(id == 2)
        axis = 1;
    // Exit the program
    if(id == 3)
        exit(0);
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-10.0, 10.0, -5.0 * (GLfloat) h / (GLfloat) w,
            15.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);
    else
        glOrtho(-10.0 * (GLfloat) w / (GLfloat) h,
            10.0 * (GLfloat) w / (GLfloat) h, -5.0, 15.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myInit()
{
    // blue ocean background colour
    glClearColor(0.2f, 0.6f, 1.0f, 1.0f);

    // set world dimensions
    glOrtho(-20.0, 20.0, -20.0, 20.0, -20.0, 20.0);

    // initialize quadric object
    q = gluNewQuadric();
}

void myMenu()
{
    glutCreateMenu(menu);
    glutAddMenuEntry("Adjust Snail Eyes", 1);
    glutAddMenuEntry("Walk Snail", 2);
    glutAddMenuEntry("Quit", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Lab Project");

    char* filename = "H:\\CSCI 364\\CSCI 345\\LabProject\\sand.bmp";
    loadTextureFromFile(filename);

    myInit();
    myMenu();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutMainLoop();
}
