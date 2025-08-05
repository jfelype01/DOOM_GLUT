#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "./stb_image.h"

//Arquivos do projeto
#include "player.h"
#include "utils.h"
#include "map.h"
#include "enemy.h"
#include "guns.h"

const std::string ASSETS_PATH = "assets/";

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)

using namespace std;

void display() {
    int x, y;
    double currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Lógica para spawn de inimigos
    if (currentTime - lastSpawnTime > spawnInterval) {
        SpawnEnemy();
        lastSpawnTime = currentTime;
    }

    // Lógica para a dificuldade dinâmica
	const double DIFFICULTY_INTERVAL = 30000.0; // tempo para diminuir o intervalo de spawn (25 segundos)
	const double DECREASE_AMOUNT = 3000.0;     // tempo diminuído a cada intervalo (3 segundos)
	const double MIN_SPAWN_INTERVAL = 9000.0;   // limite mínimo do intervalo de spawn (6 segundos)

    if (currentTime - lastDifficultyIncreaseTime > DIFFICULTY_INTERVAL) {
        if (spawnInterval > MIN_SPAWN_INTERVAL) {
            spawnInterval -= DECREASE_AMOUNT;
            if (spawnInterval < MIN_SPAWN_INTERVAL) {
                spawnInterval = MIN_SPAWN_INTERVAL;
            }
            std::cout << "A dificuldade aumentou! Novo intervalo de spawn: " << spawnInterval / 1000.0 << " segundos." << std::endl;
        }
        lastDifficultyIncreaseTime = currentTime; // Reinicia o temporizador
    }

    if (T.frame1 - T.frame2 >= 16) {
        clearBackground();
        if (Mouse.mouse_dx != 0 || Mouse.mouse_dy != 0) {
            P.angle += Mouse.mouse_dx * Mouse.sensibilidade * 0.5f;
            if (P.angle >= 360) P.angle -= 360;
            if (P.angle < 0)    P.angle += 360;

            P.look += Mouse.mouse_dy * Mouse.sensibilidade * 0.5f;
            if (P.look > 90)    P.look = 90;
            if (P.look < -90)  P.look = -90;

            Mouse.mouse_dx = 0;
            Mouse.mouse_dy = 0;
        }
        if (shoootColldown > 0) {
            shoootColldown -= 0.1;
        }
        else if (mouseHeld == true) {
            animateShoot();
        }

        EnemyUpdate();
        movePlayer();

        glDisable(GL_TEXTURE_2D);
        draw3D();
        drawEnemySprite();

        if (gunIndex == 0 ) {
            animateGun();
        } else if (gunIndex == 1) {
            animateshootgun();
		}

        drawCrosshair();
        T.frame2 = T.frame1;
        glutSwapBuffers();
    }

    T.frame1 = glutGet(GLUT_ELAPSED_TIME);
    glutPostRedisplay();
}

void init() {

    GunInit();
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
    P.hp = 100;
    glEnable(GL_TEXTURE_2D);

    Mouse.lastX = GLSW / 2;
    Mouse.lastY = GLSH / 2;
    Mouse.first = 1;
    Mouse.sensibilidade = 0.1f;

    srand(time(NULL));
    initTextures();
    load();
    glutSetCursor(GLUT_CURSOR_NONE);
	EnemyInit(1); //quantidade de inimigos
	lastSpawnTime = glutGet(GLUT_ELAPSED_TIME);
    lastDifficultyIncreaseTime = glutGet(GLUT_ELAPSED_TIME);

    loadEnemyAnimationWalk();
    loadEnemyAttackTextures();
    loadEnemyDeathTextures();
    loadShootTextures();
    loadShootgunTextures();

    glEnable(GL_TEXTURE_2D);

    GLuint pistolTexture = loadTextureEnemy((ASSETS_PATH + "gun1.png").c_str());
    printf("ID da textura pistol: %u\n", pistolTexture);

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(GLSW, GLSH);
    glutCreateWindow("");
    glPointSize(pixelScale);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    gluOrtho2D(0, GLSW, 0, GLSH);
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(KeysDown);
    glutKeyboardUpFunc(KeysUp);
    glutMouseFunc(MouseClick);
    glutMainLoop();

    return 0;
}