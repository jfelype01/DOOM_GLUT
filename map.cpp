#include <iostream>
#include <algorithm>
#include <GL/glut.h>
#include <cmath>
#include "player.h"
#include "map.h"
#include "utils.h"

//textures
#include "textures/T_wall.h"
#include "textures/T_floor.h"
#include "textures/T_ceiling.h"
#include "textures/T_NUMBERS.h"
#include "textures/T_VIEW2D.h"

TexureMaps Textures[64];
WALLS W[660];
SECTORS S[128];
int numText = 3;                          //number of textures
int numSect = 0;                          //number of sectors
int numWall = 0;                          //number of walls

void initTextures() {
    Textures[0].name = _T_FLOOR; Textures[0].h = _T_FLOOR_HEIGHT; Textures[0].w = _T_FLOOR_WIDTH;
    Textures[1].name = _T_WALL; Textures[1].h = _T_WALL_HEIGHT; Textures[1].w = _T_WALL_WIDTH;
    Textures[2].name = T_ceiling; Textures[2].h = T_CEILING_HEIGHT; Textures[2].w = T_CEILING_WIDTH;
}

void drawPixel(int x, int y, int r, int g, int b) {
    glColor3ub(r, g, b);;
    glBegin(GL_POINTS);
    glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
    glEnd();
}

void clearBackground() {
    int x, y;
    for (y = 0; y < SH; y++) {
        for (x = 0; x < SW; x++) {
            drawPixel(x, y, 0, 60, 130);
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

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int s, int w, int frontBack) {
    int wt = W[w].wt;

    double ht = 0, ht_step = (double)Textures[wt].w * W[w].u / (double)(x2 - x1);

    int dx = x2 - x1;
    if (dx == 0) dx = 1;
    int dyb = b2 - b1;
    int dyt = t2 - t1;
    int xstep = x1;

    if (x1 < 0) {
        ht -= ht_step * x1;
    }

    x1 = std::max(0, std::min(x1, SW));
    x2 = std::max(0, std::min(x2, SW));

    for (int x = x1; x < x2; x++) {
        int y1 = dyb * (x - xstep + 0.5) / dx + b1;
        int y2 = dyt * (x - xstep + 0.5) / dx + t1;

        double vt = 0, vt_step = (double)Textures[wt].h * W[w].v / (double)(y2 - y1);

        if (y1 < 0) {
            vt -= vt_step * y1;
        }
        y1 = std::max(0, std::min(y1, SH));
        y2 = std::max(0, std::min(y2, SH));

        if (frontBack == 0) {
            if (S[s].surface == 1) {
                S[s].surf[x] = y1;
            }
            if (S[s].surface == 2) {
                S[s].surf[x] = y2;
            }
            for (int y = y1; y < y2; y++) {
                int pixel = (int)(Textures[wt].h - (int)vt % Textures[wt].h - 1) * 3 * Textures[wt].w + ((int)ht % Textures[wt].w) * 3;

                int r = Textures[wt].name[pixel + 0] - W[w].shade / 2;
                int g = Textures[wt].name[pixel + 1] - W[w].shade / 2;
                int b = Textures[wt].name[pixel + 2] - W[w].shade / 2;

                if (r < 0) r = 0;
                if (g < 0) g = 0;
                if (b < 0) b = 0;

                drawPixel(x, y, r, g, b);
                vt += vt_step;
            }
            ht += ht_step;
        }
        if (frontBack == 1) {
            //for (int y = y1; y < y2; y++) drawPixel(x, y, 255, 0, 0);


            int xo = SW / 2;
            int yo = SH / 2;

            double fov = 200;
            int x2 = x - xo;
            int wo;
            double tile = S[s].ss * 7;

            if (S[s].surface == 1) {
                y2 = S[s].surf[x];
                wo = S[s].z1;
            }
            if (S[s].surface == 2) {
                y1 = S[s].surf[x];
                wo = S[s].z2;
            }

            double lookUpDown = -P.look * 6.2;

            if (lookUpDown > SH) {
                lookUpDown = SH;
            }

            double moveUpDown = (double)(P.z - wo) / (double)yo;

            if (moveUpDown == 0) {
                moveUpDown == 0.001;
            }

            int ys = y1 - yo, ye = y2 - yo;

            for (int y = ys; y < ye; y++) {
                float z = y + lookUpDown;
                if (z == 0) {
                    z = 0.0001;
                }
                double fx = x2 / z * moveUpDown * tile;
                double fy = fov / z * moveUpDown * tile;

                double rx = fx * M.sin[P.angle] - fy * M.cos[P.angle] + (P.y / 60.0 * tile);
                double ry = fx * M.cos[P.angle] + fy * M.sin[P.angle] - (P.x / 60.0) * tile;

                if (rx < 0) rx = -rx + 1;
                if (ry < 0) ry = -ry + 1;

                int st = S[s].st;
                int pixel = (int)(Textures[st].h - (int)ry % Textures[st].h - 1) * 3 * Textures[st].w + ((int)rx % Textures[st].w) * 3;
                int r = Textures[st].name[pixel + 0];
                int g = Textures[st].name[pixel + 1];
                int b = Textures[st].name[pixel + 2];

                drawPixel(x2 + xo, y + yo, r, g, b);
            }
        }
    }
}

void draw3D() {
    int x, s, w, frontBack, cycles, wx[4], wy[4], wz[4];
    double cos1 = M.cos[P.angle], sin1 = M.sin[P.angle];

    std::sort(S, S + numSect, [](const SECTORS& a, const SECTORS& b) {
        return a.d > b.d;
        });

    for (s = 0; s < numSect; s++) {
        S[s].d = 0;

        if (P.z < S[s].z1) {
            S[s].surface = 1;
            cycles = 2;
            for (x = 0; x < SW; x++) {
                S[s].surf[x] = SH;
            }
        }
        else if (P.z > S[s].z2) {
            S[s].surface = 2;
            cycles = 2;
            for (x = 0; x < SW; x++) {
                S[s].surf[x] = 0;
            }
        }
        else {
            S[s].surface = 0;
            cycles = 1;
        }

        for (frontBack = 0; frontBack < cycles; frontBack++) {
            for (w = S[s].ws; w < S[s].we; w++) {

                int x1 = W[w].x1 - P.x, y1 = W[w].y1 - P.y;
                int x2 = W[w].x2 - P.x, y2 = W[w].y2 - P.y;

                if (frontBack == 1) {
                    std::swap(x1, x2);
                    std::swap(y1, y2);
                }

                wx[0] = x1 * cos1 - y1 * sin1;
                wx[1] = x2 * cos1 - y2 * sin1;
                wx[2] = wx[0];
                wx[3] = wx[1];

                wy[0] = y1 * cos1 + x1 * sin1;
                wy[1] = y2 * cos1 + x2 * sin1;
                wy[2] = wy[0];
                wy[3] = wy[1];

                W[w].d = dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2);
                S[s].d += W[w].d;

                wz[0] = S[s].z1 - P.z + ((P.look * wy[0]) / 32.0);
                wz[1] = S[s].z1 - P.z + ((P.look * wy[1]) / 32.0);
                wz[2] = S[s].z2 - P.z + ((P.look * wy[0]) / 32.0);
                wz[3] = S[s].z2 - P.z + ((P.look * wy[1]) / 32.0);

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


                //std::cout << "ola" << std::endl;

                drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], s, w, frontBack);
            }
            S[s].d /= (S[s].we - S[s].ws);
        }
    }
}

void floors() {
    int x, y;

    int xo = SW / 2;
    int yo = SH / 2;

    double fov = 200;
    double lookUpDown = -P.look * 4;

    if (lookUpDown > SH) {
        lookUpDown = SH;
    }

    double moveUpDown = P.z / 16.0;

    if (moveUpDown == 0) {
        moveUpDown == 0.001;
    }

    int ys = -yo, ye = -lookUpDown;

    if (moveUpDown < 0) {
        ys = -lookUpDown;
        ye = yo + lookUpDown;
    }

    for (int y = ys; y < ye; y++) {
        for (x = -xo; x < xo; x++) {
            float z = y + lookUpDown;
            if (z == 0) {
                z = 0.0001;
            }
            double fx = x / z * moveUpDown;
            double fy = fov / z * moveUpDown;

            double rx = fx * M.sin[P.angle] - fy * M.cos[P.angle] + (P.y / 30.0);
            double ry = fx * M.cos[P.angle] + fy * M.sin[P.angle] - (P.x / 30.0);

            if (rx < 0) rx = -rx + 1;
            if (ry < 0) ry = -ry + 1;

            if (fx < 0) fx = -fx + 1;
            if (fy < 0) fy = -fy + 1;

            if (rx <= 0 || ry <= 0 || rx >= 5 || ry >= 5) { continue; }

            if ((int)fx % 2 == (int)fy % 2) {
                drawPixel(x + xo, y + yo, 255, 0, 0);
            }
            else {
                drawPixel(x + xo, y + yo, 0, 255, 0);
            }
        }
    }
}

void load()
{
    FILE* fp = NULL;
    if (fopen_s(&fp, "level.h", "r") != 0 || fp == NULL) { printf("Error opening level.h"); return; }
    int s, w;

    if (fscanf_s(fp, "%i", &numSect) != 1) { fclose(fp); return; }   //number of sectors 

    for (s = 0; s < numSect; s++)      //load all sectors
    {
        if (fscanf_s(fp, "%i", &S[s].ws) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &S[s].we) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &S[s].z1) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &S[s].z2) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &S[s].st) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &S[s].ss) != 1) { fclose(fp); return; }
    }
    if (fscanf_s(fp, "%i", &numWall) != 1) { fclose(fp); return; }   //number of walls 
    for (s = 0; s < numWall; s++)      //load all walls
    {
        if (fscanf_s(fp, "%i", &W[s].x1) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].y1) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].x2) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].y2) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].wt) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].u) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].v) != 1) { fclose(fp); return; }
        if (fscanf_s(fp, "%i", &W[s].shade) != 1) { fclose(fp); return; }
    }
    if (fscanf_s(fp, "%i %i %i %i %i", &P.x, &P.y, &P.z, &P.angle, &P.look) != 5) { fclose(fp); return; } //player position, angle, look direction 
    fclose(fp);
}