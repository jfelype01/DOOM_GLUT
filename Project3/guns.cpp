#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "enemy.h"
#include "guns.h"
#include "stb_image.h"
#include "utils.h"

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)

GUN guns[7];
bool isShooting = false;
double shoootColldown = 0.0;
bool mouseHeld = false;
int gunIndex = 0;


GLuint pistolTexture = 0;
GLuint shootTextures[5];
GLuint shootgunTextures[6];

bool isAnimating = false;
int currentFrame = 0;
int frameCounter = 0;
int frameDelay = 5;
int frameDelaySG = 9;

// Declaração do vetor de inimigos
extern std::vector<ENEMY> inimigos;

void GunInit() {
    guns[0].arc = 50;
    guns[0].range = 2000;
    guns[0].damage = 20; // Pistola
    guns[0].shootDelay = 5;

    guns[1].arc = 50;
    guns[1].range = 2000;
    guns[1].damage = 50;
    guns[1].shootDelay = 9; // Rifle
}

void raycasting(int gun, double px, double py, double dx, double dy) {
    float maxDistance = guns[gun].range; // Alcance do tiro

    float step = 1;
    for (float dist = 0.0f; dist < maxDistance; dist += step) {
        float shootX = px + dx * dist;
        float shootY = py + dy * dist;

        // Verifica colisão com inimigo
        for (ENEMY& inimigo : inimigos) {

            if (!inimigo.alive) {
                continue; // Pula inimigos que já terminaram a animação de morte
            }

            double distToEnemy = sqrt((shootX - inimigo.x) * (shootX - inimigo.x) + (shootY - inimigo.y) * (shootY - inimigo.y));

            //std::cout << "Player em: (" << px << ", " << py << "); " << "Tiro em (" << shootX << ", " << shootY << "), inimigo em (" << inimigo.x << ", " << inimigo.y << " Distância: (" << distToEnemy << ")\n";

            if (distToEnemy < guns[gun].arc && inimigo.hp > 0) { // raio de acerto
                inimigo.hp -= guns[gun].damage; // Aplica dano
                std::cout << "Inimigo atingido! HP restante: " << inimigo.hp << std::endl;
                // AQUI ESTÁ A CORREÇÃO:
                // Só inicia a animação de morte se o inimigo ainda não estiver morrendo (state != 3)
                if (inimigo.hp <= 0 && inimigo.state != 3) {
                    inimigo.state = 3; // Mude o estado para morrendo
                    inimigo.deathFrameCount = 0; // Inicia o contador de frames da morte
                }
                return;
            }
        }
    }
}

void shoot(int gun) {
    double px = P.x;
    double py = P.y;
    int angle = (int)P.angle;
    int size = 0;
	double dx[5];
	double dy[5];

    if (gun == 0) {
        size = 1;
        dx[0] = M.sin[angle];
		dy[0] = M.cos[angle];
    }

    if (gun == 1) {
		size = 5;
        dx[0] = M.sin[angle];
        dx[1] = M.sin[angle + 3];
        dx[2] = M.sin[angle + 5];
        dx[3] = M.sin[angle - 3];
        dx[4] = M.sin[angle - 5];

        dy[0] = M.cos[angle];
        dy[1] = M.cos[angle + 3];
        dy[2] = M.cos[angle + 5];
        dy[3] = M.cos[angle - 3];
        dy[4] = M.cos[angle - 5];
    }

	//std::cout << "Disparando arma " << gun << " de (" << px << ", " << py << ") na direção (" << dx << ", " << dy << ")\n";
	//std::cout << "Inimigo: " << inimigos[0].x << ", " << inimigos[0].y << std::endl;

    for (int i = 0; i < size; ++i) {
        raycasting(gun, px, py, dx[i], dy[i]);
	}
}

void loadShootTextures() {
    for (int i = 0; i < 5; ++i) {
        std::string filename = "assets/shoot" + std::to_string(i + 1) + ".png";
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            glGenTextures(1, &shootTextures[i]);
            glBindTexture(GL_TEXTURE_2D, shootTextures[i]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLenum format = GL_RGBA;
            if (nrChannels == 3) format = GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else {
            printf("Erro ao carregar a imagem: %s\n", filename.c_str());
            printf("Razao: %s\n", stbi_failure_reason());
        }
        stbi_image_free(data);
    }
}

void animateGun() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, GLSW, 0, GLSH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (isAnimating) {
        frameCounter++;
        if (frameCounter > frameDelay) {
            currentFrame++;
            if (currentFrame >= 5) {
                currentFrame = 0;
                isAnimating = false;
            }
            frameCounter = 0;
        }
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shootTextures[currentFrame]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1, 1, 1);

    float scale = 2.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(GLSW / 2 - 32 * scale, 0);
    glTexCoord2f(1, 1); glVertex2f(GLSW / 2 + 32 * scale, 0);
    glTexCoord2f(1, 0); glVertex2f(GLSW / 2 + 32 * scale, 64 * scale);
    glTexCoord2f(0, 0); glVertex2f(GLSW / 2 - 32 * scale, 64 * scale);
    glEnd();

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void loadShootgunTextures() {
    for (int i = 0; i < 6; ++i) {
        std::string filename = "assets/shootgun" + std::to_string(i + 1) + ".png";
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            glGenTextures(1, &shootgunTextures[i]);
            glBindTexture(GL_TEXTURE_2D, shootgunTextures[i]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLenum format = GL_RGBA;
            if (nrChannels == 3) format = GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else {
            printf("Erro ao carregar a imagem: %s\n", filename.c_str());
            printf("Razao: %s\n", stbi_failure_reason());
        }
        stbi_image_free(data);
    }
}

void animateshootgun() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, GLSW, 0, GLSH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    if (isAnimating) {
        frameCounter++;
        if (frameCounter > frameDelaySG) {
            currentFrame++;
            if (currentFrame >= 6) {
                currentFrame = 0;
                isAnimating = false;
            }
            frameCounter = 0;
        }
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shootgunTextures[currentFrame]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1, 1, 1);

    float scale = 3.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(GLSW / 2 - 32 * scale, 0);
    glTexCoord2f(1, 1); glVertex2f(GLSW / 2 + 32 * scale, 0);
    glTexCoord2f(1, 0); glVertex2f(GLSW / 2 + 32 * scale, 64 * scale);
    glTexCoord2f(0, 0); glVertex2f(GLSW / 2 - 32 * scale, 64 * scale);
    glEnd();

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void animateShoot() {
    if (shoootColldown <= 0) {
        if (!isAnimating) {
            isAnimating = true;
            currentFrame = 0;
            glutPostRedisplay();
        }
        shoootColldown = guns[gunIndex].shootDelay;

        shoot(gunIndex); // Exemplo: dispara arma 0
    }
}

void MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseHeld = true;
            animateShoot();
        }
        if (state == GLUT_UP) {
            mouseHeld = false;
        }
    }

}
