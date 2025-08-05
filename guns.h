#ifndef GUNS_H
#define GUNS_H

typedef struct {
    double arc;
    double range;
    double damage;
    double shootDelay;
}GUN;

extern double shoootColldown;
extern bool isShooting;
extern GUN guns[7];
extern bool mouseHeld;
extern int gunIndex;

void GunInit();
void shoot(int gun);
void MouseClick(int button, int state, int x, int y);
void loadShootTextures();
void animateGun();
void animateShoot();
void animateshootgun();
void loadShootgunTextures();


#endif