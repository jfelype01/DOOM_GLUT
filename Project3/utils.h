#ifndef UTILS_H
#define UTILS_H

typedef struct {
    double cos[360];
    double sin[360];
}MATH;


typedef struct {
    int frame1, frame2;
}TIME;

extern TIME T;
extern MATH M;

int dist(int x1, int y1, int x2, int y2);
double pointToSegmentDistance(double px, double py, double x1, double y1, double x2, double y2);
void drawCrosshair();

#endif