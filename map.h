#ifndef MAP_H
#define MAP_H

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)

typedef struct {
    int x1, y1;
    int x2, y2;
    int color;
    int d;
    int wt, u, v;            //wall texture and u/v tile
    int shade;             //shade of the wall
}WALLS;

typedef struct {
    int ws, we;
    int z1, z2;
    int d;
    int c1, c2;
    int st, ss;             //surface texture, surface scale 
    int surf[SW];
    int surface;
}SECTORS;

typedef struct
{
    int w, h;                             //texture width/height
    const unsigned char* name;           //texture name
}TexureMaps;


extern TexureMaps Textures[64];
extern WALLS W[660];
extern SECTORS S[128];
extern int numText;                          //number of textures
extern int numSect;                          //number of sectors
extern int numWall;                          //number of walls

void initTextures();
void clearBackground();
void drawPixel(int x, int y, int r, int g, int b);
void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2);
void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int s, int w, int frontBack);
void draw3D();
void floors();
void load();

#endif
