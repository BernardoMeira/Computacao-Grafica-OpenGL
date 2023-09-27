#include <windows.h>
#include <GL/glut.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#define _CRT_SECURE_NO_WARNINGS

#define DEG_TO_RADIANS 0.017453292519943295769236907684886f
#define EN_SIZE 20
#define rotationRate 0.035
#define angleBackFrac 0.6
#define camBackFrac 0.01
#define velocidadeBackFrac 0.2

#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44
#define VK_UP 0x26
#define VK_DOWN 0x28

#define GRASSDIM 256
#define RUNWAYDIM 512
#define grass "grass2.ppm"
#define runway "830.ppm"

GLuint grassTexture;
GLuint roadTexture;

using namespace std;

const GLfloat light_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 50.0f };

const double PI = 3.141592654;
int interval = 1000 / 60;
float posX = 1;
float posY = 1;
float posZ = 1;
int angulo = 0;
float camX = 0;
float camZ = 0;

//float velocidade = 0.06;

float velocidade = 0.000f;
const float drag = 0.00225f;

float zoom = 4;
float rotX = 0;
float rotY = 0;
float rotZ = 0;
int TIME = 0;
bool START = false;
bool rot = false;

void loadTextureFile(const char * filename)
{
	FILE* pFile;
	pFile= fopen(filename, "rb");
	if (!pFile)
	{
		printf("Falha para abrir: %s\n",filename);
		return;
	}
	double w,h,d;
	if(strcmp(filename, runway) == 0)
	{
		w=RUNWAYDIM;
		h=RUNWAYDIM;
	}
	else
	{
		w = GRASSDIM;
		h = GRASSDIM;
	}
	unsigned char* imagedata;
	imagedata=(unsigned char*)malloc(sizeof(unsigned char)*w*h*4);
	fread(imagedata,sizeof(unsigned char),h*w*4,pFile);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imagedata );
	delete [] imagedata;
	fclose(pFile);

}

void bindTextures( const char * filename1, const char * filename2)
{

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &grassTexture);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	loadTextureFile(filename1);
	glGenTextures(1, &roadTexture);
	glBindTexture(GL_TEXTURE_2D, roadTexture);
	loadTextureFile(filename2);
	printf("Texturas carregadas\n");

}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void fan(){
    //glColor3d(0.4,0.4,0.4);
    glColor3d(0.4,0.4,0.8);
    glPushMatrix();
        glTranslated(0,0,-0.2);
        glScaled(1.7,1.7,0.4);
        glutSolidSphere(0.8,30,30);
    glPopMatrix();

    glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0,0,0);
        glRotated(1,0,1,0);
        glScaled(0.6,17,0.05);
        glutSolidSphere(1,30,30);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,0,0);
        glRotated(-1,0,1,0);
        glRotated(90,0,0,1);
        glScaled(0.6,17,0.05);
        glutSolidSphere(1,30,30);
    glPopMatrix();

}

void tail_fan(){
    //glColor3d(0.4,0.4,0.4);
    glColor3d(0.4,0.4,0.8);
    glPushMatrix();
        glTranslated(0,0,-0.37);
        glScaled(0.4,0.4,0.2);
        glutSolidSphere(0.8,30,30);
    glPopMatrix();

    glColor3d(0.4,0.4,0.4);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(0.57,0.57,0.2);
        glutSolidSphere(0.8,30,30);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,0,0);
        glRotated(-1,0,1,0);
        glRotated(90,0,0,1);
        glScaled(0.3,2.6,0.05);
        glutSolidSphere(1,30,30);
    glPopMatrix();
}

void plane(){
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;

    /// Main body
    glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(1.5,0.6,0.55);
        glutSolidSphere(1,30,30);
    glPopMatrix();

    glColor3d(0.5,0.7,1.0);
    glPushMatrix();
        glTranslated(0.6,0,0);
        glScaled(1,0.55,0.45);
        glutSolidSphere(1,30,30);
    glPopMatrix();

    glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0.96,0.03,0);
        glScaled(0.2,0.635,0.43);
        glRotated(90,0,1,0);
        glRotated(60,1,0,0);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

        glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0.82,0,0);
        glScaled(0.79,0.2,0.43);
        glRotated(90,0,1,0);
        glRotated(90,1,0,0);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

    ///Motor
        glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(-0.2,0.52,0);
        glRotated(-4,0,0,1);
        glScaled(1.8,0.5,0.2);
        glRotated(18,0,0,1);
        glRotated(0,0,1,0);
        glutSolidCube(1);
    glPopMatrix();

    ///Tail

    glColor3d(0.4,0.4,0.4);
    glPushMatrix();
        glTranslated(-2,0.35,0);
        glRotated(-2,0,0,1);
        glScaled(3,0.24,0.12);
        glRotated(5,0,0,1);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0.1,0.1,0.1);
    glPushMatrix();
        glTranslated(-2,0,0);
        glRotated(-14,0,0,1);
        glScaled(2.88,0.08,0.08);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0.4,0.4,0.8);
    glPushMatrix();
        glTranslated(-3.39,0.6,0);
        glRotated(-30,0,0,1);
        glScaled(0.48,0.27,0.1);
        glRotated(30,0,0,1);
        glutSolidCube(1);
    glPopMatrix();

    ///Tail_fan

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(-4.2,0.38,0.9);
        glRotated(-90,1,0,0);
    glPushMatrix();
        glTranslated(0.9,1,0);
        glRotated(90,1,0,0);
        glRotated(13*a,0,0,1);
        glScaled(0.2,0.2,0.2);
        tail_fan();
    glPopMatrix();

        glTranslated(0.9,0.8,0);
        glRotated(90,1,0,0);
        glScaled(0.09,0.09,0.6);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

    /// Left Leg
    glColor3d(0.4,0.4,0.4);
    glPushMatrix();
        glTranslated(-0.1,-0.7,-0.5);
        glScaled(1.8,0.07,0.18);
        glutSolidCube(1);
    glPopMatrix();

        glColor3d(0.2,0.2,0.2);
    glPushMatrix();
        glTranslated(-0.8,-0.35,-0.32);
        glRotated(30,1,0,0);
        glScaled(0.1,0.8,0.1);
        glutSolidCube(1);
    glPopMatrix();
        glColor3d(0.2,0.2,0.2);
    glPushMatrix();
        glTranslated(0.6,-0.35,-0.32);
        glRotated(30,1,0,0);
        glScaled(0.1,0.8,0.1);
        glutSolidCube(1);
    glPopMatrix();

    /// Right Leg
    glColor3d(0.4,0.4,0.4);
    glPushMatrix();
        glTranslated(-0.1,-0.7,0.5);
        glScaled(1.8,0.07,0.18);
        glutSolidCube(1);
    glPopMatrix();

        glColor3d(0.2,0.2,0.2);
    glPushMatrix();
        glTranslated(-0.8,-0.35,0.32);
        glRotated(-30,1,0,0);
        glScaled(0.1,0.8,0.1);
        glutSolidCube(1);
    glPopMatrix();
        glColor3d(0.2,0.2,0.2);
    glPushMatrix();
        glTranslated(0.6,-0.35,0.32);
        glRotated(-30,1,0,0);
        glScaled(0.1,0.8,0.1);
        glutSolidCube(1);
    glPopMatrix();

    /// Rotor
    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(0.2,-0.15,0.9);
        glRotated(90,0,1,0);
    glPushMatrix();
        glTranslated(0.9,1,0);
        glRotated(90,1,0,0);
        glRotated(13*a,0,0,1);
        glScaled(0.2,0.2,0.2);
        fan();
    glPopMatrix();

        glTranslated(0.9,0.8,0);
        glRotated(90,1,0,0);
        glScaled(0.09,0.09,0.6);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

        glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0.2,0.65,-0.17);
        glRotated(90,0,1,0);
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

        glColor3d(0.6,0.6,0.6);
    glPushMatrix();
        glTranslated(0.2,0.65,0.17);
        glRotated(90,0,1,0);
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();
}


void ground(){
    /// Ground
    glColor3d(0,0.5,0.1);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(EN_SIZE*2,0.3,EN_SIZE*2);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-8,-2.7,-5);
        glRotated(65,0,1,0);
        //glRotated(15,0,1,0);
        glScaled(2,2,2);
    glPopMatrix();

    glPushMatrix();
        glTranslated(8,-2.7,-5);
        glRotated(-65,0,1,0);
        //glRotated(15,0,1,0);
        glScaled(2,2,2);
    glPopMatrix();
}

void tree(){
    ///Trees
	glPushMatrix ();
        glColor3f(0.91,0.76,0.65);
        glRotatef(-90,1,0,0);
        glScaled(0.2,0.2,1.4);
        glutSolidTorus(0.5,0.5,10,10);
    glPopMatrix ();

    glPushMatrix ();
        glColor3f(0.184314,0.309804,0.184314);
        glRotatef(-90,1,0,0);
        glScaled(0.2,0.2,0.4);
        glutSolidCone( 3.5, 8, 32, 8 );
    glPopMatrix ();
}


void environment(){

    /// Ground

    //glColor3d(0,0.5,0.1);
    //glPushMatrix();
        //glTranslated(0,0,0);
        //glScaled(EN_SIZE*2,0.3,EN_SIZE*2);
        //glutSolidCube(1);
    //glPopMatrix();

   // glColor3d(0,1,0.7);

//    glPushMatrix();
//        glRotated(angle,0,1,0);
//        glPushMatrix();
//            glTranslated(tX,tY,tZ);
//            glScaled(1,1,2);
//            //glRotated(90,1,0,0);
//            glutSolidCube(1);
//        glPopMatrix();
//    glPopMatrix();

    double terrainSize = 400.0;
    double roadSize = 400.0;
	double textureSize = 320.0;
	double roadTextureSize = 30.0;
	double roadTextureWidth = 1.0;
	glColor3f(1.0,1.0,1.0);
	glPolygonMode(GL_BACK, GL_FILL);
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, grassTexture);
	glPushMatrix ();
        glRotatef(300,0.f,1.f,0.f);
        glRotatef(180,1,0,0);
        glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); glVertex3d(-terrainSize,1.0,-terrainSize);
        glTexCoord2d(textureSize,0.0);   glVertex3d(terrainSize,1.0,-terrainSize);
        glTexCoord2d(textureSize,textureSize); glVertex3d(terrainSize,1.0,terrainSize);
        glTexCoord2d(0.0,textureSize); glVertex3d(-terrainSize,1.0,terrainSize);
        glEnd();
	glPopMatrix ();
	glDisable (GL_TEXTURE_2D);

    glColor3f(1.0,1.0,1.0);
	glPolygonMode(GL_BACK, GL_FILL);
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, roadTexture);
	glPushMatrix ();
        glTranslatef(0,0.1,0);
        //glRotatef(300,0.f,1.f,0.f);
        glRotatef(180,1,0,0);
        glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); glVertex3d(-3,1.0,-roadSize);
        glTexCoord2d(roadTextureWidth,0.0);   glVertex3d(3,1.0,-roadSize);
        glTexCoord2d(roadTextureWidth,roadTextureSize); glVertex3d(3,1.0,roadSize);
        glTexCoord2d(0.0,roadTextureSize); glVertex3d(-3,1.0,roadSize);
        glEnd();
	glPopMatrix ();
	glDisable (GL_TEXTURE_2D);
}

void draw(){
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;

    TIME = t;

    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


    ///Environment

    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslated(0,0.5,0);
        environment();
    glPopMatrix();

    glEnable(GL_LIGHTING);
    ///Trees Right
    glPushMatrix();
    int treeX = 0;
    int treeZ = 0;
    int treeDistance =0;
    glTranslated(17,0,-300);
    glRotated(-11,0,1,0);
        for (int I = 0; I < 20; I++) {
            treeDistance += 50;
            treeX = 10 * (I/2);
            treeZ = 10 + treeDistance/2;
            for (int J = 0; J < 10; J++) {
                glPushMatrix();
                    glTranslated(treeX,0,treeZ);
                    treeX += 30;
                    treeZ += 30;
                    tree();
                glPopMatrix();
            }
        }
    glPopMatrix();

    glPushMatrix();
    treeX = 0;
    treeZ = 0;
    treeDistance =0;
    glTranslated(11,0,-300);
    glRotated(-44,0,1,0);
        for (int I = 0; I < 20; I++) {
            treeDistance += 50;
            treeX = 10 * (I/2);
            treeZ = 10 + treeDistance/2;
            for (int J = 0; J < 10; J++) {
                glPushMatrix();
                    glTranslated(treeX,0,treeZ);
                    treeX += 30;
                    treeZ += 30;
                    tree();
                glPopMatrix();
            }
        }
    glPopMatrix();

    ///Plane
    if(rotX>11)rotX=11;
    if(rotX<-11)rotX=-11;
    if(rotZ>10)rotZ=10;
    if(rotZ<-15)rotZ=-15;

    glPushMatrix();
        glTranslatef(posX,posY,posZ);
        glRotatef(angulo, 0.0f, 1.0f, 0.0f);
        glRotated(rotX,1,0,0);
        glRotated(rotY,0,1,0);
        glRotated(rotZ,0,0,1);
        glScalef(0.4,0.4,0.4);
        glRotatef(90,0,1,0);
        plane();

    if(rotX>0)rotX-=velocidadeBackFrac;
    if(rotX<0)rotX+=velocidadeBackFrac;
    if(rotY>0)rotY-=velocidadeBackFrac;
    if(rotY<0)rotY+=velocidadeBackFrac;
    if(rotZ>0)rotZ-=velocidadeBackFrac;
    if(rotZ<0)rotZ+=velocidadeBackFrac;
	glPopMatrix();
}


void drawBitmapText(char *str,float x,float y,float z)
{
	char *c;
	glRasterPos3f(x,y+8,z);

	for (c=str; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

void drawStrokeText(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x-4, y+8,z);
	  glScalef(0.007f,0.007f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}

void drawStrokeText2(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.005f,0.005f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}
void drawStrokeChar(char c,float x,float y,float z)
{
	  glPushMatrix();
          glTranslatef(x, y+8,z);
          glScalef(0.002f,0.002f,z);
          glutStrokeCharacter(GLUT_STROKE_ROMAN , c);
	  glPopMatrix();
}

void teclado() {
    float rotFrac = 1;
    if (GetAsyncKeyState(VK_W)){
            //posX -= sin(M_PI * angulo / 180) * velocidade;
            //posZ -= cos(M_PI * angulo / 180) * velocidade;
            velocidade += drag*2;
            //rotX -= rotFrac/2;

    }
    if (GetAsyncKeyState(VK_S)){
    		//posX += sin(M_PI * angulo / 180) * velocidade;
            //posZ += cos(M_PI * angulo / 180) * velocidade;
            velocidade -= drag*2;
            //rotX += rotFrac/2;
    }
    if (GetAsyncKeyState(VK_A)){;
            angulo += 1.0f;
            if (angulo >= -360)angulo %= 360;
            //rotX -= rotFrac/2;
            //rotZ += rotFrac/2;
            rotX -= velocidade;
            rotZ += velocidade;
            camX -= 0.01;
            camZ -= 0.01;
    }
    if (GetAsyncKeyState(VK_D)){
            angulo -= 1.0f;
            if (angulo <= 360)angulo %= 360;
            //rotX -= rotFrac/2;
            //rotZ -= rotFrac/2;
            rotX -= velocidade;
            rotZ -= velocidade;
            camX += 0.01;
            camZ += 0.01;
    }
    if (GetAsyncKeyState(VK_UP)){
        posY += 0.02f;
    }
    if (GetAsyncKeyState(VK_DOWN)){
        if(posY > 0.0f){
            posY -= 0.02f;
        } else {
            posY = 0.0f;
        }
    }

}

void updateCamera(){
    if(camX>1)camX=1;
    if(camX<-1)camX=-1;
    if(camZ>1)camZ=1;
    if(camZ<-1)camZ=-1;

    gluLookAt(posX+sin(angulo*M_PI/180),posY+0.05,posZ+cos(angulo*M_PI/180),
    // center x
    posX,
    // center y
    posY+0.1,
    // center z
    posZ,
    0.0,1.0,0.0);

    if(camX>0)camX-=angleBackFrac;
    if(camX<0)camX+=angleBackFrac;
    if(camZ>0)camZ-=angleBackFrac;
    if(camZ<0)camZ+=angleBackFrac;
}

void updateVelocidade(){
    if (velocidade > 0.0f){
        posX -= sin(M_PI * angulo / 180) * velocidade;
        posZ -= cos(M_PI * angulo / 180) * velocidade;
        velocidade -= drag;
    }
    else if (velocidade < 0.0f){
        posX -= sin(M_PI * angulo / 180) * velocidade;
        posZ -= cos(M_PI * angulo / 180) * velocidade;
        velocidade += drag;
    }
    else if (velocidade < 0.01 || velocidade > 0.01){
        velocidade = 0.0f;
    }
    rotX -= velocidade*2;
    if(velocidade > 0.3f)velocidade = 0.3f;
    if(velocidade < -0.3f)velocidade = -0.3f;
    printf("velocidade: %d\n",velocidade);
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;
    double aa=a;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);

    if(!rot){
        a=0;
    }

    glLoadIdentity();

    gluLookAt(	0.0, 4.5, 10.0,
                0, 4, 0,
                0, 1.0f, 0.0f);

    if(START){
        glClearColor(0.8,1.0,1.0,1);
        teclado();
        glPushMatrix();
            glTranslated(0,0,0);
            glScaled(zoom,zoom,zoom);
            glRotated(a,0,1,0);
            updateCamera();
            draw();
        glPopMatrix();
    }

    else{
        glClearColor(1.0,1.0,1.0,1);
        glPushMatrix();
            glTranslated(0,3,0);
            glRotated(aa/1.4,0,1,0);
            glScaled(1.5,1.5,1.5);
            plane();
        glPopMatrix();

        drawStrokeText("Aperte G para comecar",-1,-1,0);
    }

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    float frac = 0.3;
    float rotFrac = 1;
    switch (key)
    {
        case 27 :
            exit(0);
            break;
        case 'g':
            START=true;
            break;
        case 'm':
            START=false;
            break;
    }

    glutPostRedisplay();
}

void update(int value) {
   teclado();
   updateVelocidade();
   updateCamera();

   glutTimerFunc(interval, update, 0);

   glutPostRedisplay();
}

void inicializar (void){
    glClearColor(0.8,1.0,1.0,1);
    glClearDepth( 1.0f );

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LESS);

    bindTextures(grass, runway);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat globalAmbient[] = { 0.2, 0.2, 0.2, 1.0 };

    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );

}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,720);
    glutInitDisplayMode (GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("TESTE");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutTimerFunc(interval, update, 0);
    inicializar();
    glutMainLoop();

    return EXIT_SUCCESS;
}
