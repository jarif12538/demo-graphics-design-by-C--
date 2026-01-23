#include <GL/glut.h>

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glOrtho(-4, 8, -4, 8, -4, 8);
    glClear(GL_COLOR_BUFFER_BIT);
    // House base
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-2.0f, -2.0f);
    glVertex2f(2.0f, -2.0f);
    glVertex2f(2.0f, 2.0f);
    glVertex2f(-2.0f, 2.0f);
    glEnd();
    // Roof
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-2.5f, 2.0f);
    glVertex2f(2.5f, 2.0f);
    glVertex2f(0.0f, 4.0f);
    glEnd();
    // Door
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.4f, -2.0f);
    glVertex2f(0.4f, -2.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(-0.4f, 0.0f);
    glEnd();
    // Left window
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-1.5f, 0.5f);
    glVertex2f(-0.7f, 0.5f);
    glVertex2f(-0.7f, 1.3f);
    glVertex2f(-1.5f, 1.3f);
    glEnd();
    // Right window
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.7f, 0.5f);
    glVertex2f(1.5f, 0.5f);
    glVertex2f(1.5f, 1.3f);
    glVertex2f(0.7f, 1.3f);
    glEnd();

    glFlush();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(350, 40);
    glutCreateWindow("Simple Triangle");
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}