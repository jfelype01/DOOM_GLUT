#include <cmath>
#include <GL/glut.h>
#include "player.h"
#include "utils.h"
#include "map.h"
#include "enemy.h"
#include "guns.h"

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)

MOUSE Mouse;
KEYS K;
PLAYER P;

bool checaColisao(int nx, int ny) {
    const double radius = 8.0;

    for (int s = 0; s < numSect; s++) {
        if (S[s].ws < 0 || S[s].we > numWall) continue;

        for (int w = S[s].ws; w < S[s].we; w++) {
            if (w < 0 || w >= numWall) continue;

            double d = pointToSegmentDistance(static_cast<double>(nx),
                static_cast<double>(ny),
                static_cast<double>(W[w].x1),
                static_cast<double>(W[w].y1),
                static_cast<double>(W[w].x2),
                static_cast<double>(W[w].y2));
            if (d < radius) {
                return true; // colisão detectada
            }
        }
    }
    return false;
}

void movePlayer() {
    int dx = M.sin[P.angle] * 10.0;
    int dy = M.cos[P.angle] * 10.0;

    int nx = P.x;
    int ny = P.y;

    if (K.w == 1 && K.move == 0) {
        nx += dx;
        ny += dy;
    }
    if (K.s == 1 && K.move == 0) {
        nx -= dx;
        ny -= dy;
    }
    if (K.a == 1 && K.move == 0) {
        nx -= dy;
        ny += dx;
    }
    if (K.d == 1 && K.move == 0) {
        nx += dy;
        ny -= dx;
    }

    // Modo 'm' (move vertical e look) permanece igual
    if (K.a == 1 && K.move == 1) {
        P.look -= 1;
    }
    if (K.d == 1 && K.move == 1) {
        P.look += 1;
    }
    if (K.w == 1 && K.move == 1) {
        P.z -= 4;
    }
    if (K.s == 1 && K.move == 1) {
        P.z += 4;
    }

    // Checa colisão antes de aplicar movimento
    if (!checaColisao(nx, ny)) {
        P.x = nx;
        P.y = ny;
    }
}

void KeysDown(unsigned char key, int x, int y) {
    if (key == 'w') {
        K.w = 1;
    }
    if (key == 's') {
        K.s = 1;
    }
    if (key == 'a') {
        K.a = 1;
    }
    if (key == 'd') {
        K.d = 1;
    }
    if (key == 'm') {
        K.move = 1;
    }
    if (key == ',') {
        K.strafeLeft = 1;
    }
    if (key == '.') {
        K.strafeRight = 1;
    }
    if (key == '1') {
        gunIndex = 0;
    }
    if (key == '2') {
        gunIndex = 1;
	}
    if (key == 13) {
		EnemyInit(1); // Inicializa o inimigo
    }
}

void KeysUp(unsigned char key, int x, int y) {
    if (key == 'w') {
        K.w = 0;
    }
    if (key == 's') {
        K.s = 0;
    }
    if (key == 'a') {
        K.a = 0;
    }
    if (key == 'd') {
        K.d = 0;
    }
    if (key == 'm') {
        K.move = 0;
    }
    if (key == ',') {
        K.strafeLeft = 0;
    }
    if (key == '.') {
        K.strafeRight = 0;
    }
}

void mouseMotion(int x, int y) {
    const int centerX = GLSW / 2;
    const int centerY = GLSH / 2;
    const int threshold = 10;  // tolerância de movimento para evitar múltiplos warps

    if (Mouse.first) {
        Mouse.lastX = x;
        Mouse.lastY = y;
        Mouse.first = 0;
        return;
    }

    int dx = x - centerX;
    int dy = y - centerY;

    if (abs(dx) > threshold || abs(dy) > threshold) {
        P.angle += dx * Mouse.sensibilidade;
        if (P.angle >= 360) P.angle -= 360;
        if (P.angle < 0) P.angle += 360;

        P.look += dy * Mouse.sensibilidade;
        if (P.look > 90) P.look = 90;
        if (P.look < -90) P.look = -90;

        glutWarpPointer(centerX, centerY);

        Mouse.lastX = centerX;
        Mouse.lastY = centerY;
    }
}