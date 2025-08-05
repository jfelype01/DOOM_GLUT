#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector> // Inclua o cabeçalho para usar std::vector
#include "./stb_image.h"
#include "utils.h"
#include "enemy.h"
#include "player.h"

const std::string ASSETS_PATH = "assets/";

#define res         1
#define SW          160*res
#define SH          120*res
#define SW2         (SW/2)
#define SH2         (SH/2)
#define pixelScale  4/res
#define GLSW        (SW*pixelScale)
#define GLSH        (SH*pixelScale)


std::vector<ENEMY> inimigos;
GLuint enemyTexture[8][4];
GLuint enemyAttackTexture[8][3];
GLint enemyDeathTexture[5];

double lastSpawnTime = 0.0; // Variável para controlar o tempo de spawn
double spawnInterval = 15000.0; // Intervalo de spawn em segundos
double lastDifficultyIncreaseTime = 0.0;

void atackPlayer(ENEMY& inimigo) {
    if (inimigo.state == 2) {
        if (inimigo.hp > 0) {
            P.hp -= 10; // Exemplo: reduz 10 pontos de vida do jogador
			std::cout << "Inimigo atacou o jogador! Vida do jogador: " << P.hp << std::endl;
            if (P.hp <= 0) exit(1); // Garante que a vida não fique negativa
        }
    }
}

void EnemyInit(int numEnemies) {
    const int spawnRange = 1000; // Define o tamanho da área de spawn (por exemplo, 500x500)
    const int spawnCenterX = 1000;
    const int spawnCenterY = 1000;

    const int minX = spawnCenterX - spawnRange / 2;
    const int minY = spawnCenterY - spawnRange / 2;

    for (int i = 0; i < numEnemies; ++i) {
        ENEMY novoInimigo;

        // Gere coordenadas aleatórias dentro do intervalo centralizado
        novoInimigo.x = (rand() % spawnRange) + minX;
        novoInimigo.y = (rand() % spawnRange) + minY;

        novoInimigo.z = 0;
        novoInimigo.hp = 100;
        novoInimigo.alive = true;
        novoInimigo.spriteFrame = 0;
        novoInimigo.enemycount = 0;
        novoInimigo.speed = 3.0f;
        novoInimigo.attackFrameCount = 0;
        novoInimigo.state = 1;
        novoInimigo.deathFrameCount = 0;
        novoInimigo.radius = 15.0f;
        inimigos.push_back(novoInimigo);
    }
}

void SpawnEnemy() {
    const int spawnRange = 500;
    const int spawnCenterX = 1000;
    const int spawnCenterY = 1000;
    const int minX = spawnCenterX - spawnRange / 2;
    const int minY = spawnCenterY - spawnRange / 2;

    const int inimigosNaOnda = 5; // Define o número de inimigos por onda

    for (int i = 0; i < inimigosNaOnda; ++i) {
        ENEMY novoInimigo;

        novoInimigo.x = (rand() % spawnRange) + minX;
        novoInimigo.y = (rand() % spawnRange) + minY;
        novoInimigo.z = 0;
        novoInimigo.hp = 100;
        novoInimigo.alive = true;
        novoInimigo.spriteFrame = 0;
        novoInimigo.enemycount = 0;
        novoInimigo.speed = 1.5f;
        novoInimigo.attackFrameCount = 0;
        novoInimigo.state = 1;
        novoInimigo.deathFrameCount = 0;
        novoInimigo.radius = 15.0f;

        inimigos.push_back(novoInimigo);
    }
}

void EnemyUpdate() {
    for (size_t i = 0; i < inimigos.size(); ++i) {
        for (size_t j = i + 1; j < inimigos.size(); ++j) {
            ENEMY& inimigo1 = inimigos[i];
            ENEMY& inimigo2 = inimigos[j];

            if (!inimigo1.alive || !inimigo2.alive) continue;

            float dx = inimigo2.x - inimigo1.x;
            float dy = inimigo2.y - inimigo1.y;
            float dist = sqrt(dx * dx + dy * dy);

            float minDistance = inimigo1.radius + inimigo2.radius;

            if (dist < minDistance && dist > 0) {
                // Colisão detectada, empurre os inimigos
                float overlap = minDistance - dist;
                float nx = dx / dist; // vetor normalizado x
                float ny = dy / dist; // vetor normalizado y

                inimigo1.x -= nx * overlap * 0.5f;
                inimigo1.y -= ny * overlap * 0.5f;
                inimigo2.x += nx * overlap * 0.5f;
                inimigo2.y += ny * overlap * 0.5f;
            }
        }
    }
    // Itera por todos os inimigos
    for (ENEMY& inimigo : inimigos) {
        // Se o inimigo não estiver vivo, não faça nada.
        // Apenas continue se o estado for de morte (3) para concluir a animação.
        if (!inimigo.alive && inimigo.state != 3) {
            continue;
        }

        float target_dx = P.x - inimigo.x;
        float target_dy = P.y - inimigo.y;
        float dist = sqrt(target_dx * target_dx + target_dy * target_dy);

        const float ATTACK_DISTANCE = 50.0f;
        const float ATTACK_ANIMATION_SPEED = 10.0f;
        const float ATTACK_COOLDOWN_TIME = 15.0f;

        float angleToPlayerRad = atan2(target_dy, target_dx);
        float angleToPlayerDeg = angleToPlayerRad * 180.0f / M_PI;
        if (angleToPlayerDeg < 0) {
            angleToPlayerDeg += 360.0f;
        }

        int desiredDirection;
        if (angleToPlayerDeg >= 30.0f && angleToPlayerDeg < 150.0f) {
            desiredDirection = DIR_N;
        }
        else if (angleToPlayerDeg >= 300.0f || angleToPlayerDeg < 30.0f) {
            desiredDirection = DIR_NE;
        }
        else {
            desiredDirection = DIR_NW;
        }
        inimigo.currentDirection = desiredDirection;

        switch (inimigo.state) {
        case 2:
        {
            inimigo.attackFrameCount++;
            inimigo.spriteFrame = (inimigo.attackFrameCount / (int)ATTACK_ANIMATION_SPEED);

            if (inimigo.spriteFrame >= 3) {
				atackPlayer(inimigo);
                inimigo.state = 0;
                inimigo.spriteFrame = 0;
                inimigo.attackFrameCount = 0;
                inimigo.enemycount = 0;
            }
            break;
        }

        case 0:
        {
            inimigo.spriteFrame = 0;
            inimigo.enemycount++;

            if (inimigo.enemycount >= ATTACK_COOLDOWN_TIME && dist < ATTACK_DISTANCE) {
                inimigo.state = 2;
                inimigo.attackFrameCount = 0;
                inimigo.spriteFrame = 0;
            }
            else if (inimigo.enemycount >= ATTACK_COOLDOWN_TIME && dist >= ATTACK_DISTANCE) {
                inimigo.state = 1;
                inimigo.enemycount = 0;
            }
            break;
        }

        case 1:
        {
            if (dist < ATTACK_DISTANCE) {
                inimigo.state = 0;
                inimigo.enemycount = 0;
                inimigo.spriteFrame = 0;
                break;
            }

            float normalized_move_dx = target_dx / dist;
            float normalized_move_dy = target_dy / dist;
            inimigo.x += normalized_move_dx * inimigo.speed;
            inimigo.y += normalized_move_dy * inimigo.speed;

            inimigo.enemycount++;
            if (inimigo.enemycount >= 10) {
                inimigo.spriteFrame = (inimigo.spriteFrame + 1) % 4;
                inimigo.enemycount = 0;
            }
            break;
        }

        case 3: // Estado de morte
        {
            const float DEATH_ANIMATION_SPEED = 20.0f;

            inimigo.deathFrameCount++;
            inimigo.spriteFrame = (inimigo.deathFrameCount / (int)DEATH_ANIMATION_SPEED);

            // A animação de morte terminou
            if (inimigo.spriteFrame >= 5) {
                inimigo.alive = false; // O inimigo é marcado como morto, e ele irá sumir no próximo frame
                inimigo.state = 4; // Mude o estado para um valor que não seja 3, para que a verificação inicial do loop de atualização possa ignorá-lo no futuro.
            }
            break;
        }
        }
    }
}

void drawEnemySprite() {
    for (ENEMY& inimigo : inimigos) {
        // Se o inimigo não estiver vivo, pule o desenho
        if (!inimigo.alive) {
            continue;
        }

        double cos1 = M.cos[(int)P.angle % 360], sin1 = M.sin[(int)P.angle % 360];

        int dx = inimigo.x - P.x;
        int dy = inimigo.y - P.y;

        double wx = dx * cos1 - dy * sin1;
        double wy = dy * cos1 + dx * sin1;

        if (wy <= 1) continue;

        double wz = (inimigo.z - P.z) + ((P.look * wy) / 32.0);

        int screenX = wx * 200 / wy + SW2;
        int screenY = wz * 200 / wy + SH2;

        int spriteSize = 30 * 300 / wy;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);

        if (inimigo.state == 2) {
            if (inimigo.currentDirection >= 0 && inimigo.currentDirection < 8 &&
                inimigo.spriteFrame >= 0 && inimigo.spriteFrame < 3) {
                glBindTexture(GL_TEXTURE_2D, enemyAttackTexture[inimigo.currentDirection][inimigo.spriteFrame]);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, enemyAttackTexture[DIR_N][0]);
            }
        }
        else if (inimigo.state == 3) {
            if (inimigo.spriteFrame >= 0 && inimigo.spriteFrame < 5) {
                glBindTexture(GL_TEXTURE_2D, enemyDeathTexture[inimigo.spriteFrame]);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, enemyDeathTexture[3]);
            }
        }
        else {
            if (inimigo.currentDirection >= 0 && inimigo.currentDirection < 8 &&
                inimigo.spriteFrame >= 0 && inimigo.spriteFrame < 4) {
                glBindTexture(GL_TEXTURE_2D, enemyTexture[inimigo.currentDirection][inimigo.spriteFrame]);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, enemyTexture[DIR_N][0]);
            }
        }
        glColor3ub(255, 255, 255);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2i((screenX - spriteSize) * pixelScale, (screenY + spriteSize) * pixelScale);
        glTexCoord2f(1.0f, 0.0f); glVertex2i((screenX + spriteSize) * pixelScale, (screenY + spriteSize) * pixelScale);
        glTexCoord2f(1.0f, 1.0f); glVertex2i((screenX + spriteSize) * pixelScale, (screenY - spriteSize) * pixelScale);
        glTexCoord2f(0.0f, 1.0f); glVertex2i((screenX - spriteSize) * pixelScale, (screenY - spriteSize) * pixelScale);
        glEnd();

        glDisable(GL_BLEND);
    }
}

GLuint loadTextureEnemy(const char* filename) {
    int width, height, nrChannels;

    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Erro STB: Não foi possível carregar a textura " << filename << ": " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else {
        std::cerr << "Formato de canal não suportado para " << filename << std::endl;
        stbi_image_free(data);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return textureID;
}

void loadEnemyAnimationWalk() {
    const char* directionNames[] = {
        "E",
        "FDE",
        "F",
        "FDD",
        "D",
        "TDD",
        "T",
        "TDE"
    };

    for (int dir = 0; dir < 8; ++dir) {
        for (int frame = 0; frame < 4; ++frame) {
            std::string filename = "A" + std::string(directionNames[dir]) + "0" + std::to_string(frame + 1) + ".png";
            std::string fullPath = ASSETS_PATH + filename;

            enemyTexture[dir][frame] = loadTextureEnemy(fullPath.c_str());

            if (enemyTexture[dir][frame] == 0) {
                std::cerr << "Falha ao carregar a textura: " << filename << std::endl;
            }
        }
    }
}

void loadEnemyAttackTextures() {
    const char* frontDirectionPrefix = "F";
    GLuint tempFrontAttackTextures[3];

    for (int frame = 0; frame < 3; ++frame) {
        std::string filename = "T" + std::string(frontDirectionPrefix) + "0" + std::to_string(frame + 1) + ".png";
        std::string fullPath = ASSETS_PATH + filename;

        tempFrontAttackTextures[frame] = loadTextureEnemy(fullPath.c_str());

        if (tempFrontAttackTextures[frame] == 0) {
            std::cerr << "Falha ao carregar a textura de ataque frontal: " << filename << std::endl;
        }
    }
    for (int dir = 0; dir < 8; ++dir) {
        for (int frame = 0; frame < 3; ++frame) {
            enemyAttackTexture[dir][frame] = tempFrontAttackTextures[frame];
        }
    }
}

void loadEnemyDeathTextures() {
    for (int frame = 0; frame < 5; ++frame) {
        std::string filename = "M0" + std::to_string(frame + 1) + ".png";
        std::string fullPath = ASSETS_PATH + filename;

        enemyDeathTexture[frame] = loadTextureEnemy(fullPath.c_str());

        if (enemyDeathTexture[frame] == 0) {
            std::cerr << "Falha ao carregar a textura de morte: " << filename << std::endl;
        }
    }
}