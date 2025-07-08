#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <algorithm>

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)
#define numSect     4
#define numWall     16


using namespace std;

typedef struct {
    int frame1, frame2;
}TIME;
TIME T;

typedef struct {
    int w, s, a, d;
    int strafeLeft, strafeRight;
    int move;
}KEYS;
KEYS K;

typedef struct {
    double cos[360];
    double sin[360];
}MATH;
MATH M;

typedef struct {
    int x, y, z;
    int angle;
    int look;
} PLAYER;
PLAYER P;

typedef struct {
    int x1, y1;
    int x2, y2;
    int color;
}WALLS;
WALLS W[30];

typedef struct {
    int ws, we;
    int z1, z2;
    int d;
    int c1, c2;
    int surf[SW];
    int surface;
}SECTORS;
SECTORS S[30];

void pixel(int x, int y, int c) {
    int rgb[3];
    
    switch (c) {
    case 0:
		rgb[0] = 255; rgb[1] = 255; rgb[2] = 0; // Yellow
		break;
    case 1:
		rgb[0] = 160; rgb[1] = 160; rgb[2] = 0; // Yellow Darker
		break;
    case 2:
		rgb[0] = 0; rgb[1] = 255; rgb[2] = 0; // Green
		break;
    case 3:
		rgb[0] = 0; rgb[1] = 160; rgb[2] = 0; // Green Darker
        break;
    case 4:
		rgb[0] = 0; rgb[1] = 255; rgb[2] = 255; // Cyan
        break;
    case 5:
        rgb[0] = 0; rgb[1] = 160; rgb[2] = 160; // Cyan Darker
		break;
	case 6:
        rgb[0] = 160; rgb[1] = 100; rgb[2] = 0; // Brown
		break;
	case 7:
		rgb[0] = 110; rgb[1] = 50; rgb[2] = 0; // Brown Darker
		break;
    case 8:
		rgb[0] = 0; rgb[1] = 60; rgb[2] = 130; // background
        break;
    }
    glColor3ub(rgb[0], rgb[1], rgb[2]);;
	glBegin(GL_POINTS);
    glVertex2i(x*pixelScale+2, y*pixelScale+2);
	glEnd();
}

void movePlayer() {
    // Move the player
    if (K.a == 1 && K.move == 0) {
		P.angle -= 4;
        if (P.angle < 0) {
            P.angle += 360;
		}
    }
	if (K.d == 1 && K.move == 0) {
        P.angle += 4;
        if (P.angle > 359) {
            P.angle -= 360;
		}
    }

    int dx = M.sin[P.angle] * 10.0;
	int dy = M.cos[P.angle] * 10.0;

    if (K.w == 1 && K.move == 0) {
        P.x += dx;
		P.y += dy;
    }
    if (K.s == 1 && K.move == 0) {
        P.x -= dx;
        P.y -= dy;
    }

    // Strafe
    if (K.strafeLeft == 1) {
        P.x += dy;
        P.y += dx;
    }
    if (K.strafeRight == 1) {
        P.x -= dy;
		P.y -= dx;
    }

    // Move and look
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
}

void clearBackground() {
    int x, y;
    for (y = 0; y < SH; y++) {
        for (x = 0; x < SW; x++) {
            pixel(x, y, 8);
        }
    }
}

void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2) {
    double da = *y1;
    double db = y2;
    double d = da - db;
    if (d == 0) {
        d = 1;
    }

    double s = da / d;
    *x1 = *x1 + s * (x2 - (*x1));
    *y1 = *y1 + s * (y2 - (*y1));
    if (*y1 == 0) {
        *y1 = 1;
    }
	*z1 = *z1 + s * (z2 - (*z1));

}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int color, int s) {
    int x, y;
    int dyb = b2 - b1;
    int dyt = t2 - t1;
    int dx = x2 - x1;
    if (dx == 0) {
        dx = 1;
    }
    int xstep = x1;

    if (x1 < 1) {
        x1 = 1;
    }
    if (x2 < 1) {
        x2 = 1;
    }
    if (x1 > SW - 1) {
        x1 = SW - 1;
    }
    if (x2 > SW - 1) {
        x2 = SW - 1;
    }

    for (x = x1; x < x2; x++) {
        int y1 = dyb * (x - xstep + 0.5) / dx + b1;
		int y2 = dyt * (x - xstep + 0.5) / dx + t1;

        if (y1 < 1) {
            y1 = 1;
		}
        if (y2 < 1) {
            y2 = 1;
        }
        if (y1 > SH - 1) {
            y1 = SH - 1;
		}
        if (y2 > SH - 1) {
            y2 = SH - 1;
		}

        if (S[s].surface == 1) {
            S[s].surf[x] = y1;
            continue;
        }
        if (S[s].surface == 2) {
            S[s].surf[x] = y2;
            continue;
		}
        if (S[s].surface == -1) {
            for (y = S[s].surf[x]; y < y1; y++) {
                pixel(x, y, S[s].c2);
            };
        }
        if (S[s].surface == -2) {
            for (y = y2; y < S[s].surf[x]; y++) {
                pixel(x, y, S[s].c2);
            };
        }
        for (y = y1; y < y2; y++) {
            pixel(x, y, color);
        }
    }
}

int dist(int x1, int y1, int x2, int y2) {
    int distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return distance;
}

void draw3D() {
    int s, w, loop, wx[4], wy[4], wz[4];
	double cos1 = M.cos[P.angle], sin1 = M.sin[P.angle];

    sort(S, S + numSect, [](const SECTORS& a, const SECTORS& b) {
        return a.d > b.d;
		});

    for (s = 0; s < numSect; s++) {
        S[s].d = 0;

        if (P.z < S[s].z1) {
            S[s].surface = 1;
        } else if (P.z > S[s].z2) {
            S[s].surface = 2;
        } else {
            S[s].surface = 0;
		}

        for (loop = 0; loop < 2; loop++) {
            for (w = S[s].ws; w < S[s].we; w++) {

                int x1 = W[w].x1 - P.x, y1 = W[w].y1 - P.y;
                int x2 = W[w].x2 - P.x, y2 = W[w].y2 - P.y;

                if (loop == 0) {
                    swap(x1, x2);
                    swap(y1, y2);
                }

                wx[0] = x1 * cos1 - y1 * sin1;
                wx[1] = x2 * cos1 - y2 * sin1;
                wx[2] = wx[0];
                wx[3] = wx[1];

                wy[0] = y1 * cos1 + x1 * sin1;
                wy[1] = y2 * cos1 + x2 * sin1;
                wy[2] = wy[0];
                wy[3] = wy[1];

                S[s].d += dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2);

                wz[0] = S[s].z1 - P.z + ((P.look * wy[0]) / 32.0);
                wz[1] = S[s].z1 - P.z + ((P.look * wy[1]) / 32.0);
                wz[2] = wz[0] + S[s].z2;
                wz[3] = wz[1] + S[s].z2;

                if (wy[0] < 1 && wy[1] < 1) {
                    continue;
                }
                if (wy[0] < 1) {
                    clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]);
                    clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]);
                }
                if (wy[1] < 1) {
                    clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]);
                    clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]);
                }


                wx[0] = wx[0] * 200 / wy[0] + SW2;
                wy[0] = wz[0] * 200 / wy[0] + SH2;

                wx[1] = wx[1] * 200 / wy[1] + SW2;
                wy[1] = wz[1] * 200 / wy[1] + SH2;

                wx[2] = wx[2] * 200 / wy[2] + SW2;
                wy[2] = wz[2] * 200 / wy[2] + SH2;

                wx[3] = wx[3] * 200 / wy[3] + SW2;
                wy[3] = wz[3] * 200 / wy[3] + SH2;

                drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], W[w].color, s);
            }
            S[s].d /= (S[s].we - S[s].ws);
            S[s].surface *= -1;
        }
    }
}

void display() {
    int x, y;
    if (T.frame1 - T.frame2 >= 50) {
		clearBackground();
        movePlayer();
        draw3D();

        T.frame2 = T.frame1;
        glutSwapBuffers();
        glutReshapeWindow(GLSW, GLSH);
    }

    T.frame1 = glutGet(GLUT_ELAPSED_TIME);
    glutPostRedisplay();
}

void KeysDown(unsigned char key, int x, int y) {
    if (key == 'w' == 1) {
        K.w = 1;
    }
    if (key == 's' == 1) {
        K.s = 1;
	}
    if (key == 'a' == 1) {
        K.a = 1;
    }
    if (key == 'd' == 1) {
        K.d = 1;
	}
    if (key == 'm' == 1) {
        K.move = 1;
    }
    if (key == ',' == 1) {
		K.strafeLeft = 1;
    }
    if (key == '.' == 1) {
        K.strafeRight = 1;
    }
}

void KeysUp(unsigned char key, int x, int y) {
    if (key == 'w' == 1) {
        K.w = 0;
    }
    if (key == 's' == 1) {
        K.s = 0;
    }
    if (key == 'a' == 1) {
        K.a = 0;
    }
    if (key == 'd' == 1) {
        K.d = 0;
    }
    if (key == 'm' == 1) {
        K.move = 0;
    }
    if (key == ',' == 1) {
        K.strafeLeft = 0;
    }
    if (key == '.' == 1) {
        K.strafeRight = 0;
    }
}

int loadSectors[] = {//ws, we, z1, z2, bottom color, top color
    0, 4, 0, 40, 2, 3,
    4, 8, 0, 40, 4, 5,
    8, 12, 0, 40, 6, 7,
    12, 16, 0, 40, 0, 1,
};

int loadWalls[] = {//x1, y1, x2, y2, color
    0, 0, 32, 0, 0,
	32, 0, 32, 32, 1,
	32, 32, 0, 32, 0,
	0, 32, 0, 0, 1,

	64, 0, 96, 0, 2,
	96, 0, 96, 32, 3,
	96, 32, 64, 32, 2,
	64, 32, 64, 0, 3,

    64, 64, 96, 64, 4,
	96, 64, 96, 96, 5,
	96, 96, 64, 96, 4,
	64, 96, 64, 64, 5,

	0, 64, 32, 64, 6,
	32, 64, 32, 96, 7,
	32, 96, 0, 96, 6,
	0, 96, 0, 64, 7,
};

void init() {
    int i;
    for (i = 0; i < 360; i++) {
        M.cos[i] = cos(i * M_PI / 180);
        M.sin[i] = sin(i * M_PI / 180);
    }

    P.x = 70;
    P.y = -110;
    P.z = 20;
    P.angle = 0;
    P.look = 0;

    int s, w, v1 = 0, v2 = 0;
    for (s = 0; s < numSect; s++) {
        S[s].ws = loadSectors[v1 + 0];
		S[s].we = loadSectors[v1 + 1];
        S[s].z1 = loadSectors[v1 + 2];
        S[s].z2 = loadSectors[v1 + 3] - loadSectors[v1 + 2];
		S[s].c1 = loadSectors[v1 + 4];
		S[s].c2 = loadSectors[v1 + 5];
        v1 += 6;
        for (w = S[s].ws; w < S[s].we; w++) {
            W[w].x1 = loadWalls[v2 + 0];
			W[w].y1 = loadWalls[v2 + 1];
			W[w].x2 = loadWalls[v2 + 2];
			W[w].y2 = loadWalls[v2 + 3];
            W[w].color = loadWalls[v2 + 4];
			v2 += 5;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(GLSW, GLSH);
    glutCreateWindow("");
    glPointSize(pixelScale);
    gluOrtho2D(0, GLSW, 0, GLSH);
    init();
    glutDisplayFunc(display);
	glutKeyboardFunc(KeysDown);
    glutKeyboardUpFunc(KeysUp);
    glutMainLoop();

    return 0;
}
