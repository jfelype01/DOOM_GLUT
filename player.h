#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x, y, z;
    int angle;
    int look;
    int hp;
} PLAYER;

typedef struct {
    int w, s, a, d;
    int strafeLeft, strafeRight;
    int move;
} KEYS;

typedef struct {
    int lastX, lastY;
    int first;
    float sensibilidade;
    int mouse_dx;
    int mouse_dy;
} MOUSE;

extern MOUSE Mouse;
extern KEYS K;
extern PLAYER P;

void movePlayer();
bool checaColisao(int nx, int ny);
void KeysDown(unsigned char key, int x, int y);
void KeysUp(unsigned char key, int x, int y);
void mouseMotion(int x, int y);



#endif
