#ifndef ENEMY_H
#define ENEMY_H

#include <GL/glut.h>
#include <vector>

enum EnemyDirection {
    DIR_E = 0,
    DIR_NE = 1,
    DIR_N = 2,
    DIR_NW = 3,
    DIR_W = 4,
    DIR_SW = 5,
    DIR_S = 6,
    DIR_SE = 7
};

typedef struct {
    int x, y, z;
    int hp;
    bool alive;
    int spriteFrame;
    int enemycount;
    float speed;
    int currentDirection;
    int state;
    int attackFrameCount;
    int deathFrameCount;
    float radius;
} ENEMY;

extern GLuint enemyTexture[8][4];
extern GLuint enemyAttackTexture[8][3];
extern std::vector<ENEMY> inimigos;
extern double lastSpawnTime;
extern double spawnInterval; // Time in seconds between enemy spawns
extern double lastDifficultyIncreaseTime;

void EnemyInit(int numEnemies);
void EnemyUpdate();
void drawEnemySprite();
GLuint loadTextureEnemy(const char* filename);
void loadEnemyAnimationWalk();
void loadEnemyAttackTextures();
void loadEnemyDeathTextures();
void SpawnEnemy();

#endif