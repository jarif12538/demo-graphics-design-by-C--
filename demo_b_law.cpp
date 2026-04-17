#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

void bresenhams(int x1, int x2, int y1, int y2) {

    int pk, x, y;

    float m = (float)(y2 - y1) / (float)(x2 - x1);

    int xdiff = abs(x2 - x1);
    int ydiff = abs(y2 - y1);

    // 0 <= slope <= 1
    if (m >= 0 && m <= 1) {

        pk = (2 * ydiff) - xdiff;
        x = x1;
        y = y1;

        glBegin(GL_POINTS);

        for (int i = 0; i <= xdiff; i++) {

            glVertex2i(x, y);

            if (pk < 0) {
                x++;
                pk = pk + (2 * ydiff);
            } else {
                x++;
                y++;
                pk = pk + (2 * ydiff) - (2 * xdiff);
            }
        }

        glEnd();
    }

    // slope > 1
    else if (m > 1) {

        pk = (2 * xdiff) - ydiff;
        x = x1;
        y = y1;

        glBegin(GL_POINTS);

        for (int i = 0; i <= ydiff; i++) {

            glVertex2i(x, y);

            if (pk < 0) {
                y++;
                pk = pk + (2 * xdiff);
            } else {
                x++;
                y++;
                pk = pk + (2 * xdiff) - (2 * ydiff);
            }
        }

        glEnd();
    }

    // -1 <= slope < 0
    else if (m >= -1 && m < 0) {

        pk = (2 * ydiff) - xdiff;
        x = x1;
        y = y1;

        glBegin(GL_POINTS);

        for (int i = 0; i <= xdiff; i++) {

            glVertex2i(x, y);

            if (pk < 0) {
                x++;
                pk = pk + (2 * ydiff);
            } else {
                x++;
                y--;
                pk = pk + (2 * ydiff) - (2 * xdiff);
            }
        }

        glEnd();
    }

    // slope < -1
    else {

        pk = (2 * xdiff) - ydiff;
        x = x1;
        y = y1;

        glBegin(GL_POINTS);

        for (int i = 0; i <= ydiff; i++) {

            glVertex2i(x, y);

            if (pk < 0) {
                y--;
                pk = pk + (2 * xdiff);
            } else {
                x++;
                y--;
                pk = pk + (2 * xdiff) - (2 * ydiff);
            }
        }

        glEnd();
    }
}

void display() {

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glOrtho(0, 500, 0, 500, 0, 500);

    glColor3f(0.4f, 0.2f, 0.0f);

    bresenhams(200, 250, 200, 300);
    bresenhams(250, 300, 100, 200);
    bresenhams(200, 250, 200, 100);
    bresenhams(250, 300, 300, 200);

    glFlush();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Bresenham Line Algorithm");

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}