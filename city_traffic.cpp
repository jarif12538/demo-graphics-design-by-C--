/*
 * ============================================================
 *  CITY TRAFFIC SCENE  –  OpenGL + GLUT
 *  Computer Graphics Project
 * ============================================================
 *  Features implemented
 *  ─────────────────────────────────────────────────────────
 *  Graphics Primitives  : points, lines, polygons, circles
 *  Algorithms           : DDA Line, Bresenham Line,
 *                         Midpoint Circle
 *  2D Transformations   : translation, rotation, scaling,
 *                         reflection, shear
 *  Animated Objects     : moving cars (left & right lanes),
 *                         cycling traffic light,
 *                         moving sun + drifting clouds
 *  Theme                : Day-time city street scene
 * ============================================================
 *  Build (Linux / macOS)
 *    g++ city_traffic.cpp -o city_traffic -lGL -lGLU -lglut -lm
 *  Build (Windows – MinGW)
 *    g++ city_traffic.cpp -o city_traffic -lopengl32 -lglu32 -lfreeglut -lm
 * ============================================================
 */

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>

// ──────────────────────────────────────────────
//  Window dimensions
// ──────────────────────────────────────────────
const int W = 900, H = 600;

// ──────────────────────────────────────────────
//  Global animation state
// ──────────────────────────────────────────────
float carX1      = -250.0f;   // car going right (lane 1)
float carX2      =  250.0f;   // car going left  (lane 2)
float sunAngle   =  30.0f;    // degrees from horizon
float cloud1X    = -350.0f;
float cloud2X    =  100.0f;
int   tlPhase    =  0;        // 0=red 1=yellow 2=green
float tlTimer    =  0.0f;
const float TL_RED_DUR    = 3.0f;
const float TL_YELLOW_DUR = 1.0f;
const float TL_GREEN_DUR  = 3.0f;

// ──────────────────────────────────────────────
//  Colour helpers
// ──────────────────────────────────────────────
void setColor(float r, float g, float b){ glColor3f(r,g,b); }

// ══════════════════════════════════════════════
//  ALGORITHM 1 – DDA Line
//  Used for road lane markings
// ══════════════════════════════════════════════
void ddaLine(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1, dy = y2 - y1;
    float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);
    float xInc = dx / steps, yInc = dy / steps;
    float x = x1, y = y1;

    glBegin(GL_POINTS);
    for(int i = 0; i <= (int)steps; ++i){
        glVertex2f(x, y);
        x += xInc; y += yInc;
    }
    glEnd();
}

// ══════════════════════════════════════════════
//  ALGORITHM 2 – Bresenham Line
//  Used for building outlines / window grids
// ══════════════════════════════════════════════
void bresenhamLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1<x2)?1:-1, sy = (y1<y2)?1:-1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while(true){
        glVertex2i(x1, y1);
        if(x1==x2 && y1==y2) break;
        int e2 = 2*err;
        if(e2 > -dy){ err -= dy; x1 += sx; }
        if(e2 <  dx){ err += dx; y1 += sy; }
    }
    glEnd();
}

// ══════════════════════════════════════════════
//  ALGORITHM 3 – Midpoint Circle
//  Used for wheels, sun, traffic-light bulbs
// ══════════════════════════════════════════════
void midpointCircle(float cx, float cy, float r, bool filled=false)
{
    if(filled){
        // filled via triangle fan
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for(int a=0; a<=360; a+=2)
            glVertex2f(cx + r*cosf(a*M_PI/180.0f),
                       cy + r*sinf(a*M_PI/180.0f));
        glEnd();
        return;
    }
    int x = (int)r, y = 0;
    int p = 1 - (int)r;
    glBegin(GL_POINTS);
    auto plot8 = [&](int px, int py){
        glVertex2f(cx+px, cy+py); glVertex2f(cx-px, cy+py);
        glVertex2f(cx+px, cy-py); glVertex2f(cx-px, cy-py);
        glVertex2f(cx+py, cy+px); glVertex2f(cx-py, cy+px);
        glVertex2f(cx+py, cy-px); glVertex2f(cx-py, cy-px);
    };
    plot8(x,y);
    while(x > y){
        ++y;
        if(p <= 0) p += 2*y+1;
        else { --x; p += 2*(y-x)+1; }
        plot8(x,y);
    }
    glEnd();
}

// ──────────────────────────────────────────────
//  Solid polygon helper
// ──────────────────────────────────────────────
void drawRect(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
    glVertex2f(x,   y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x,   y+h);
    glEnd();
}

// ══════════════════════════════════════════════
//  2D TRANSFORMATION UTILITIES
// ══════════════════════════════════════════════

// Translation – push/pop matrix + translate
void applyTranslation(float tx, float ty)
{
    glTranslatef(tx, ty, 0.0f);
}

// Rotation around arbitrary point
void applyRotationAround(float angle, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glRotatef(angle, 0,0,1);
    glTranslatef(-cx,-cy,0);
}

// Uniform scaling around a centre
void applyScalingAround(float sx, float sy, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(sx, sy, 1);
    glTranslatef(-cx,-cy,0);
}

// Reflection about Y-axis  (scale x by -1)
void applyReflectionY(float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(-1, 1, 1);
    glTranslatef(-cx,-cy,0);
}

// Shear in X direction
void applyShearX(float shx)
{
    float m[16] = {
        1,0,0,0,
        shx,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    glMultMatrixf(m);
}

// ══════════════════════════════════════════════
//  SCENE COMPONENTS
// ══════════════════════════════════════════════

// ── Sky gradient (two quads blended) ──────────
void drawSky()
{
    glBegin(GL_QUADS);
      glColor3f(0.53f, 0.81f, 0.98f); glVertex2f(-450,-20);
      glColor3f(0.53f, 0.81f, 0.98f); glVertex2f( 450,-20);
      glColor3f(0.18f, 0.55f, 0.90f); glVertex2f( 450, 300);
      glColor3f(0.18f, 0.55f, 0.90f); glVertex2f(-450, 300);
    glEnd();
}

// ── Sun (Midpoint circle + rays via DDA) ──────
void drawSun()
{
    // Sun position derived from angle
    float rad = sunAngle * M_PI / 180.0f;
    float sx  = -300.0f + 320.0f * cosf(rad);
    float sy  =  -10.0f + 200.0f * sinf(rad);

    // Glow
    setColor(1.0f, 0.95f, 0.5f);
    midpointCircle(sx, sy, 28, true);
    // Core
    setColor(1.0f, 0.88f, 0.2f);
    midpointCircle(sx, sy, 20, true);
    // Outline
    setColor(1.0f, 0.7f, 0.0f);
    glPointSize(2.0f);
    midpointCircle(sx, sy, 20, false);

    // Rays using DDA lines
    setColor(1.0f, 0.9f, 0.3f);
    glPointSize(1.5f);
    for(int a=0; a<360; a+=45){
        float ar = a*M_PI/180.0f;
        ddaLine(sx + 22*cosf(ar), sy + 22*sinf(ar),
                sx + 36*cosf(ar), sy + 36*sinf(ar));
    }
}

// ── Cloud (group of circles) ──────────────────
void drawCloud(float cx, float cy, float scale)
{
    glPushMatrix();
    applyScalingAround(scale, scale, cx, cy);
    setColor(1,1,1);
    midpointCircle(cx,      cy,     22, true);
    midpointCircle(cx+25,   cy+5,   18, true);
    midpointCircle(cx-22,   cy+5,   16, true);
    midpointCircle(cx+10,   cy+18,  15, true);
    midpointCircle(cx-10,   cy+15,  14, true);
    glPopMatrix();
}

// ── Ground / road ─────────────────────────────
void drawRoad()
{
    // Grass / ground
    setColor(0.3f, 0.6f, 0.2f);
    drawRect(-450, -300, 900, 200);   // bottom strip

    // Road surface
    setColor(0.22f, 0.22f, 0.22f);
    drawRect(-450, -220, 900, 170);

    // Pavement kerbs
    setColor(0.75f, 0.75f, 0.75f);
    drawRect(-450, -225, 900, 8);
    drawRect(-450, -55,  900, 8);

    // Centre divider (yellow solid line – Bresenham)
    setColor(1.0f, 0.85f, 0.0f);
    glPointSize(3.0f);
    bresenhamLine(-450, -138, 450, -138);

    // Dashed white lane markings using DDA
    setColor(1,1,1);
    glPointSize(2.5f);
    for(int x = -440; x < 450; x += 60){
        ddaLine((float)x,       -175.0f,
                (float)(x+35),  -175.0f);
        ddaLine((float)x,       -100.0f,
                (float)(x+35),  -100.0f);
    }
}

// ── Building (with Bresenham window grid) ─────
void drawBuilding(float x, float y, float w, float h,
                  float r, float g, float b,
                  int wCols, int wRows,
                  bool sheared=false)
{
    glPushMatrix();

    // Optional shear transform (demonstrates shear)
    if(sheared){
        glTranslatef(x, y, 0);
        applyShearX(0.08f);
        glTranslatef(-x, -y, 0);
    }

    // Building body
    setColor(r,g,b);
    drawRect(x, y, w, h);

    // Roof edge
    setColor(r*0.7f, g*0.7f, b*0.7f);
    drawRect(x-3, y+h, w+6, 6);

    // Windows – Bresenham outlines
    float ww = w / (wCols*2.0f+1.0f);
    float wh = h / (wRows*2.5f+1.0f);
    for(int row=0; row<wRows; row++){
        for(int col=0; col<wCols; col++){
            float wx = x + ww*(1+col*2);
            float wy = y + wh*(1+row*2);
            // Lit window fill
            setColor(1.0f, 0.95f, 0.7f);
            drawRect(wx, wy, ww, wh);
            // Bresenham border
            setColor(0.3f,0.3f,0.3f);
            glPointSize(1.5f);
            bresenhamLine((int)wx,      (int)wy,
                          (int)(wx+ww), (int)wy);
            bresenhamLine((int)(wx+ww), (int)wy,
                          (int)(wx+ww), (int)(wy+wh));
            bresenhamLine((int)(wx+ww), (int)(wy+wh),
                          (int)wx,      (int)(wy+wh));
            bresenhamLine((int)wx,      (int)(wy+wh),
                          (int)wx,      (int)wy);
        }
    }

    glPopMatrix();
}

// ── Traffic light ─────────────────────────────
void drawTrafficLight(float x, float y)
{
    // Pole
    setColor(0.3f,0.3f,0.3f);
    drawRect(x-3, y, 6, 80);

    // Housing box
    setColor(0.15f,0.15f,0.15f);
    drawRect(x-12, y+80, 24, 65);

    // Bulbs – Midpoint circles
    glPointSize(2.0f);
    // Red
    setColor(tlPhase==0 ? 1.0f:0.3f, 0.0f, 0.0f);
    midpointCircle(x, y+130, 8, true);
    // Yellow
    setColor(tlPhase==1 ? 1.0f:0.3f,
             tlPhase==1 ? 0.8f:0.3f, 0.0f);
    midpointCircle(x, y+112, 8, true);
    // Green
    setColor(0.0f, tlPhase==2 ? 1.0f:0.3f, 0.0f);
    midpointCircle(x, y+94,  8, true);
}

// ── Car body (demonstrates translation transform) ─
void drawCar(float cx, float cy, float r, float g, float b, bool facingLeft=false)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    // Flip if facing left (reflection)
    if(facingLeft)
        applyReflectionY(0, 0);   // reflect about the car's local Y axis

    // Body
    setColor(r, g, b);
    drawRect(-35, -12, 70, 22);

    // Cabin (with slight rotation for 2D perspective effect)
    glPushMatrix();
    setColor(r*0.85f, g*0.85f, b*0.85f);
    drawRect(-22, 10, 44, 18);
    glPopMatrix();

    // Windshields (Bresenham)
    setColor(0.6f, 0.85f, 1.0f);
    drawRect(-21,11, 18,15);
    drawRect(  3,11, 18,15);

    // Wheels – Midpoint circle
    setColor(0.1f,0.1f,0.1f);
    midpointCircle(-20,-12, 10, true);
    midpointCircle( 20,-12, 10, true);
    setColor(0.5f,0.5f,0.5f);
    midpointCircle(-20,-12,  5, true);
    midpointCircle( 20,-12,  5, true);

    // Head/tail lights
    setColor(1.0f, 1.0f, 0.6f);
    drawRect( 33, -6, 4, 8);   // headlight
    setColor(0.9f, 0.1f, 0.1f);
    drawRect(-37, -6, 4, 8);   // tail light

    glPopMatrix();
}

// ── Pavement trees ────────────────────────────
void drawTree(float x, float y, float scale)
{
    glPushMatrix();
    applyTranslation(x, y);
    applyScalingAround(scale, scale, 0, 0);

    // Trunk
    setColor(0.45f, 0.30f, 0.10f);
    drawRect(-5, 0, 10, 40);

    // Foliage – three filled circles
    setColor(0.13f, 0.55f, 0.13f);
    midpointCircle(0,  60, 28, true);
    midpointCircle(-18,48, 22, true);
    midpointCircle( 18,48, 22, true);

    glPopMatrix();
}

// ── Rotated decorative star / signage ─────────
void drawRotatedSign(float x, float y, float angle)
{
    glPushMatrix();
    applyTranslation(x, y);
    // Rotation transform
    glRotatef(angle, 0, 0, 1);

    // Sign board
    setColor(0.9f, 0.2f, 0.1f);
    drawRect(-18, -8, 36, 16);
    setColor(1,1,1);
    // Simple cross on sign via DDA
    glPointSize(2.0f);
    ddaLine(-12, 0, 12, 0);
    ddaLine(0, -6, 0,  6);

    glPopMatrix();
}

// ══════════════════════════════════════════════
//  RENDER – assemble full scene
// ══════════════════════════════════════════════
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // ── Sky & atmosphere ──
    drawSky();
    drawSun();

    // ── Clouds ────────────
    drawCloud(cloud1X,  180, 1.0f);
    drawCloud(cloud2X,  220, 0.85f);

    // ── Buildings (background) ────────────────
    // Sheared building (demonstrates shear)
    drawBuilding(-340, -50, 80, 230, 0.55f,0.60f,0.70f, 2,5, true);
    drawBuilding(-240, -50, 70, 200, 0.65f,0.55f,0.60f, 2,4);
    drawBuilding(-150, -50, 90, 260, 0.50f,0.55f,0.65f, 3,6);
    drawBuilding( -40, -50, 60, 180, 0.60f,0.62f,0.58f, 2,4);
    drawBuilding(  50, -50, 85, 240, 0.58f,0.60f,0.68f, 3,5);
    drawBuilding( 170, -50, 75, 210, 0.62f,0.58f,0.55f, 2,5);
    drawBuilding( 270, -50, 95, 280, 0.53f,0.58f,0.67f, 3,6);
    drawBuilding( 385, -50, 65, 195, 0.60f,0.65f,0.60f, 2,4);

    // ── Road ──────────────
    drawRoad();

    // ── Trees on pavement ─
    drawTree(-380, -220, 0.9f);
    drawTree(-200, -220, 1.0f);
    drawTree(  20, -220, 0.95f);
    drawTree( 200, -220, 1.0f);
    drawTree( 380, -220, 0.9f);
    drawTree( 400, -220, 0.9f);

    // ── Traffic lights ────
    drawTrafficLight(-100, -220);
    drawTrafficLight( 100, -220);

    // ── Rotating sign ─────
    // Angle slowly oscillates – using static so it persists
    static float signAngle = 0.0f;
    signAngle += 0.3f;
    drawRotatedSign(-370, -45, signAngle);
    drawRotatedSign( 370, -45, -signAngle);

    // ── Cars ──────────────
    // Car 1 – moving right, upper lane
    drawCar(carX1, -100, 0.85f, 0.15f, 0.15f, false);
    // Car 2 – moving left,  lower lane  (reflected)
    drawCar(carX2, -170, 0.15f, 0.35f, 0.75f, true);
    // Parked car (scaled down – scaling transform)
    glPushMatrix();
    applyScalingAround(0.8f, 0.8f, -250, -185);
    drawCar(-250, -185, 0.2f, 0.65f, 0.2f, false);
    glPopMatrix();
}

// ══════════════════════════════════════════════
//  UPDATE – advance animation state
// ══════════════════════════════════════════════
void update(float dt)
{
    // Cars
    carX1 += 80.0f * dt;
    if(carX1 > 500.0f) carX1 = -500.0f;

    carX2 -= 60.0f * dt;
    if(carX2 < -500.0f) carX2 = 500.0f;

    // Sun arc
    sunAngle += 2.0f * dt;
    if(sunAngle > 150.0f) sunAngle = 10.0f;

    // Clouds drift
    cloud1X += 15.0f * dt;
    if(cloud1X > 500.0f) cloud1X = -500.0f;
    cloud2X += 10.0f * dt;
    if(cloud2X > 500.0f) cloud2X = -500.0f;

    // Traffic light cycle
    tlTimer += dt;
    float dur = (tlPhase==0) ? TL_RED_DUR :
                (tlPhase==1) ? TL_YELLOW_DUR : TL_GREEN_DUR;
    if(tlTimer >= dur){
        tlTimer = 0.0f;
        tlPhase = (tlPhase + 1) % 3;
    }
}

// ══════════════════════════════════════════════
//  GLUT CALLBACKS
// ══════════════════════════════════════════════

// Fixed timestep: ~60 fps  (16 ms per tick)
const int TIMER_MS = 16;
const float DT     = TIMER_MS / 1000.0f;

void display()
{
    render();
    glutSwapBuffers();
}

void timer(int /*value*/)
{
    update(DT);
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timer, 0);
}

void reshape(int w, int h)
{
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-450, 450, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    if(key == 27) exit(0);   // ESC to quit
}

// ══════════════════════════════════════════════
//  MAIN
// ══════════════════════════════════════════════
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("City Traffic or Street Scene");

    // OpenGL state
    glPointSize(2.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set projection once
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-450, 450, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}