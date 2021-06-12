#include <Windows.h>
#include <GL\freeglut.h>
#include <iostream>
#include "collisionSystem.h"
#include "generateRandom.h"

#define IX(n) double(n)/255.00

using namespace std;

int pix1 = 800; // x axis
int bord = int(pix1/80);
int pix = pix1 - 2*bord;
bool init = true;
bool play = true;
collisionSystem* col;

void border() {
    double x = 1.0;
    double y = 1.0;
    double z = 1.0;
    glBegin(GL_POLYGON);
    glColor3f(x, y, z);
    glVertex2f(0, 0);
    glVertex2f(bord, 0);
    glVertex2f(bord, pix1 - bord);
    glVertex2f(0, pix1 - bord);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(x, y, z);
    glVertex2f(bord, bord);
    glVertex2f(bord, 0);
    glVertex2f(pix1, 0);
    glVertex2f(pix1, bord);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(x, y, z);
    glColor3f(x, y, z);
    glVertex2f(pix1, pix1);
    glVertex2f(pix1, bord);
    glVertex2f(pix1-bord, bord);
    glVertex2f(pix1 - bord, pix1);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(x, y, z);
    glVertex2f(0, pix1);
    glVertex2f(0, pix1 - bord);
    glVertex2f(pix1 - bord, pix1 - bord);
    glVertex2f(pix1 - bord, pix1);
    glEnd();
}

void trail(Particle* p) {
    int m = p->tail.len;
    p->tail.x[p->tail.pos] = p->disc.rx;
    p->tail.y[p->tail.pos] = p->disc.ry;
    p->tail.pos = (p->tail.pos + 1) % m;
    int t = p->tail.pos;
    glPointSize(5.0f);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 1; i < p->tail.len-1; i++) { 
        glBegin(GL_LINES);
        glVertex2f(pix * p->tail.x[(t + i)%m] + bord, pix * p->tail.y[(t + i)%m] + bord);
        glVertex2f(pix * p->tail.x[(t + i+1) % m] + bord, pix * p->tail.y[(t + i+1) % m] + bord);
        glEnd();
    }
}

void drawShape(void) {
    Particle* temp;
    int t = 30;
    border();
    for (int i = 0; i < col->n; i++) {
        temp = col->particles[i];
        glBegin(GL_POLYGON);
        glColor3f(temp->c.x , temp->c.y, temp->c.z);
        for (int i = 0; i <= t; i++) {
            double angle = 2.000 * 3.141857 * double(i) / double(t);
            double x = double(pix) * (temp->disc.rx + temp->disc.radius * cos(angle));
            double y = double(pix) * (temp->disc.ry + temp->disc.radius * sin(angle));
            glVertex2f( x+bord, y+bord);
        }
        glEnd();
        if (temp->tr) trail(temp);
    }

}

void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        play = !play;
}

Particle** initialize(int* n) {
    cout << "Enter no. of particles: ";
    cin >> *n;
    Particle** arr = (Particle**)(calloc(*n, sizeof(Particle*)));
    arr[0] = new Particle(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1);
    for (int i = 1; i < *n; i++) 
        arr[i] = new Particle(0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0); // initializing with zeros
    generate(arr, *n, 0.20); // assigning random positions and velocities
    arr[0]->c.x = 1.0;
    arr[0]->c.y = 1.0;
    arr[0]->c.z = 1.0;
    return arr;
}

void initRendering() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glShadeModel(GL_SMOOTH);
    glutInitWindowSize(pix1, pix1);
    glutCreateWindow("Moving particle");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

}

// called when the window is resized
void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (double)w, 0.0f, (double)h, -1.0f, 1.0f);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawShape();
    glutSwapBuffers();
}

void update(int value) {
    if (play)
        col->simulate();
    glutPostRedisplay();
    glutTimerFunc(10, update, 0);
}


int main(int argc, char** argv) {
    col = new collisionSystem;
    int n;
    Particle** arr = initialize(&n);
    col->construct(arr, n,&init,10000); // constructor for collision class
    glutInit(&argc, argv);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //reducing ALIASING, need to work on this// supersampling
    initRendering();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(10, update, 0);
    glutMainLoop();
    return(0);
}


