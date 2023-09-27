
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string>
#include <sstream>

#define VK_W 0x57
#define VK_S 0x53

int width = 500;
int height = 300;
int interval = 1000 / 60;

int parede_width = 6;
int parede_height = 65;
int parede_speed = 2;
int parede_speedAI = 2;

float parede_left_x = 10.0f;
float parede_left_y = 50.0f;
float parede_left_center = parede_height / 2;

float parede_right_x = width - parede_width - 10;
float parede_right_y = 50;
float parede_right_center = parede_height / 2;

float bola_pos_x = width / 2;
float bola_pos_y = height / 2;
float bola_dir_x = -1.0f;
float bola_dir_y = 0.0f;
int bola_size = 10;
int bola_speed = 4;

int score_left = 0;
int score_right = 0;

void inicializar(int width, int height) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

std::string int2str(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str( );
}

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}


void desenharObjeto(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}

void desenhar() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.0f, 0.9f, 0.0f);
    desenharObjeto(parede_left_x, parede_left_y, parede_width, parede_height);
    desenharObjeto(parede_right_x, parede_right_y, parede_width, parede_height);
    drawText(width / 2 - 20, height - 25, int2str(score_left) + "|" + int2str(score_right));

    desenharObjeto(bola_pos_x - bola_size / 2, bola_pos_y - bola_size / 2, bola_size, bola_size);

    glutSwapBuffers();
}

void teclado() {
    if (GetAsyncKeyState(VK_W)) parede_left_y += parede_speed;
    if (GetAsyncKeyState(VK_S)) parede_left_y -= parede_speed;
}

void vec2_norm(float& x, float &y) {
        float length = sqrt((x * x) + (y * y));
        if (length != 0.0f) {
            length = 1.0f / length;
            x *= length;
            y *= length;
        }
    }

void updateBola() {
    bola_pos_x += bola_dir_x * bola_speed;
    bola_pos_y += bola_dir_y * bola_speed;

    if (bola_pos_x < parede_left_x + parede_width &&
        bola_pos_x > parede_left_x &&
        bola_pos_y < parede_left_y + parede_height &&
        bola_pos_y > parede_left_y) {

        float t = ((bola_pos_y - parede_left_y) / parede_height) - 0.5f;
        bola_dir_x = fabs(bola_dir_x);
        bola_dir_y = t;
    }

    if (bola_pos_x > parede_right_x &&
        bola_pos_x < parede_right_x + parede_width &&
        bola_pos_y < parede_right_y + parede_height &&
        bola_pos_y > parede_right_y) {
        float t = ((bola_pos_y - parede_right_y) / parede_height) - 0.5f;
        bola_dir_x = -fabs(bola_dir_x);
        bola_dir_y = t;
    }

    if (bola_pos_x < 0) {
        score_right++;
        bola_pos_x = width / 2;
        bola_pos_y = height / 2;
        bola_dir_x = fabs(bola_dir_x);
        bola_dir_y = 0;
    }

    if (bola_pos_x > width) {
        score_left++;
        bola_pos_x = width / 2;
        bola_pos_y = height / 2;
        bola_dir_x = -fabs(bola_dir_x);
        bola_dir_y = 0;
    }

    if (bola_pos_y > height) {
        bola_dir_y = -fabs(bola_dir_y);
    }

    if (bola_pos_y < 0) {
        bola_dir_y = fabs(bola_dir_y);
    }

    vec2_norm(bola_dir_x, bola_dir_y);
}

void updateAI_Right(){
    if (bola_pos_y < parede_right_y + parede_right_center){
        parede_right_y = parede_right_y - parede_speedAI;
    }
    if (bola_pos_y > parede_right_y + parede_right_center){
        parede_right_y = parede_right_y + parede_speedAI;
    }
}

void updateAI_Left(){
    if (bola_pos_y < parede_left_y + parede_left_center){
        parede_left_y = parede_left_y - parede_speedAI;
    }
    if (bola_pos_y > parede_left_y + parede_left_center){
        parede_left_y = parede_left_y + parede_speedAI;
    }
}

void update(int value) {
   teclado();

   updateBola();
   updateAI_Right();
   //updateAI_Left();

   glutTimerFunc(interval, update, 0);

   glutPostRedisplay();
}

void janelaRedimensionada(GLsizei w, GLsizei h){
	if(h == 0) h = 1;
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (w <= h)
            gluOrtho2D (0.0f, 500.0f, 0.0f, 500.0f*h/w);
        else
            gluOrtho2D (0.0f, 500.0f*w/h, 0.0f, 500.0f);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("TESTE");
    //glutReshapeFunc(janelaRedimensionada);
    glutDisplayFunc(desenhar);
    glutTimerFunc(interval, update, 0);
    inicializar(width, height);
    glutMainLoop();
    return 0;
}

