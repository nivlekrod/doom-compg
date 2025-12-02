#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "particle.h"
#include "scene.h"

extern GLuint texFogo;

struct Particula
{
    float x, y, z;
    float vx, vy, vz;
    float vida;
    float vidaMax;
    float tamanho;
};

static std::vector<Particula> particulas;
static bool iniciou = false;

static float frand(float a, float b)
{
    return a + (rand() / (float)RAND_MAX) * (b - a);
}

static void respawn(Particula &p)
{
    // Amostra dentro de um losango (octaedro) de altura ~1.5 usado na cena
    const float h = 0.75f;   // metade da altura
    const float base = 0.5f; // raio na base

    p.y = frand(-h, h);
    float crossScale = 1.0f - fabsf(p.y) / h;      // afunila até as pontas
    float r = crossScale * base * frand(0.0f, 1.0f); // raio aleatório no nível atual
    float ang = frand(0.0f, 2.0f * M_PI);
    p.x = cosf(ang) * r;
    p.z = sinf(ang) * r;

    p.vx = frand(-0.25f, 0.25f);
    p.vz = frand(-0.25f, 0.25f);
    p.vy = frand(2.0f, 4.5f);

    p.vidaMax = frand(0.7f, 1.2f);
    p.vida = p.vidaMax;
    p.tamanho = frand(0.3f, 0.55f);
}

void iniciaParticulasFogo(int maxParticulas)
{
    if (iniciou)
        return;

    srand((unsigned)time(NULL));
    particulas.resize(maxParticulas);
    for (size_t i = 0; i < particulas.size(); i++)
    {
        respawn(particulas[i]);
        particulas[i].vida *= frand(0.0f, 1.0f); // fase inicial aleatória
    }
    iniciou = true;
}

void atualizaParticulasFogo(float dt)
{
    if (!iniciou)
        return;

    for (size_t i = 0; i < particulas.size(); i++)
    {
        Particula &p = particulas[i];
        p.vida -= dt;
        if (p.vida <= 0.0f)
        {
            respawn(p);
            continue;
        }

        // leve turbulência horizontal
        float wiggle = sinf(p.x * 10.0f + p.z * 10.0f + p.vida * 15.0f);
        p.vx += wiggle * 0.02f;
        p.vz += wiggle * 0.02f;

        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        // sobe mais rápido que espalha
        p.vy += 1.2f * dt;
        p.vx *= 0.98f;
        p.vz *= 0.98f;
    }
}

void desenhaParticulasFogo()
{
    if (!iniciou)
        return;

    glBindTexture(GL_TEXTURE_2D, texFogo);

    // billboarding simples: roda no Y para encarar a câmera
    glPushMatrix();
    glRotatef(-yaw, 0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    for (size_t i = 0; i < particulas.size(); i++)
    {
        const Particula &p = particulas[i];
        float t = p.vida / p.vidaMax; // 1 = recém-nascido, 0 = morrendo

        float size = p.tamanho * (0.5f + 0.5f * t);
        float half = size * 0.5f;

        // cor: centro claro, borda alaranjada
        float r = 1.0f;
        float g = 0.5f + 0.5f * t;
        float b = 0.2f;
        float a = t * 0.7f; // alpha decai

        glColor4f(r, g, b, a);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(p.x - half, p.y - half, p.z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(p.x + half, p.y - half, p.z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(p.x + half, p.y + half, p.z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(p.x - half, p.y + half, p.z);
    }
    glEnd();

    glPopMatrix();
}
