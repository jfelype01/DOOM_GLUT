#include <cmath>
#include <iostream>
#include <GL/glut.h>
#include "utils.h"

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)

TIME T;
MATH M;

int dist(int x1, int y1, int x2, int y2) {
    int distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return distance;
}

void drawCrosshair() {
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(GLSW / 2 - 5, GLSH / 2);
    glVertex2f(GLSW / 2 + 5, GLSH / 2);
    glVertex2f(GLSW / 2, GLSH / 2 - 5);
    glVertex2f(GLSW / 2, GLSH / 2 + 5);
    glEnd();
}

double pointToSegmentDistance(double px, double py, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double l2 = dx * dx + dy * dy;
    if (l2 == 0.0) {
        dx = px - x1;
        dy = py - y1;
        return sqrt(dx * dx + dy * dy);
    }

    double t = ((px - x1) * dx + (py - y1) * dy) / l2;
    t = std::max(0.0, std::min(1.0, t));

    double projX = x1 + t * dx;
    double projY = y1 + t * dy;

    dx = px - projX;
    dy = py - projY;

    return sqrt(dx * dx + dy * dy);
}