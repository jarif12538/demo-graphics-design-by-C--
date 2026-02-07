
#include <GL/glut.h>
#include <math.h>



void bresenhams(int x1, int x2, int y1, int y2) {
    int pk, x, y;
    float m = float(y2 - y1) / float(x2 - x1);


    float xdiff = fabs(x2 - x1);
    float ydiff = fabs(y2 - y1);


    if (m <= 1 && m >= 0)
    {
        pk = (2 * ydiff) - xdiff;
        x = x1, y = y1;


        glBegin(GL_POINTS);


        for (int i = 0; i < xdiff; i++)
        {
            glVertex2f(x, y);


            if (pk < 0)
            {
                x++;
                pk = pk + (2 * ydiff);
            }
            else {
                x++, y++;
                pk = pk + (2 * ydiff) - (2 * xdiff);
            }
        }
        glEnd();
    }
    else if (m > 1)
    {
        pk = (2 * xdiff) - ydiff;
        x = x1, y = y1;


        glBegin(GL_POINTS);


        for (int i = 0; i < ydiff; i++)
        {
            glVertex2f(x, y);


            if (pk < 0)
            {
                y++;
                pk = pk + (2 * xdiff);
            }
            else {
                x++, y++;
                pk = pk + (2 * xdiff) - (2 * ydiff);
            }
        }
        glEnd();
    }
    else if (m >= 1 && m <= 0) {
        pk = (2 * ydiff) - xdiff;
        x = x1, y = y1;


        glBegin(GL_POINTS);


        for (int i = 0; i < xdiff; i++)
        {
            glVertex2f(x, y);


            if (pk < 0)
            {
                x++;
                pk = pk + (2 * ydiff);
            }
            else {
                x++, y--;
                pk = pk + (2 * ydiff) - (2 * xdiff);
            }
        }
        glEnd();
    }
    else {
        pk = (2 * xdiff) - ydiff;
        x = x1, y = y1;


        glBegin(GL_POINTS);


        for (int i = 0; i < ydiff; i++)
        {
            glVertex2f(x, y);


            if (pk < 0)
            {
                y--;
                pk = pk + (2 * xdiff);
            }
            else {
                x++, y--;
                pk = pk + (2 * xdiff) - (2 * ydiff);
            }
        }
        glEnd();
    }
}

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glOrtho(0, 500, 0, 500, 0, 500);
    glClear(GL_COLOR_BUFFER_BIT);


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
    glutCreateWindow("BRESENHAM");
    glutDisplayFunc(display);
    glutMainLoop();


    return 0;
}
