#include "view3DSky.h"

#include <math.h>
#include <vector>
#include <stdint.h>

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

//! размер текстуры облаков, пиксели
#define CLOUD_TEX_SIZE 256
//! размер одного повторения текстуры облаков, м
#define CLOUD_TILE 12000.0f
//! скорость ветра для облаков, м/с
#define CLOUD_WIND_X 15.0f
#define CLOUD_WIND_Z 6.0f

static float clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static float smooth(float t)
{
    return t * t * (3.0f - 2.0f * t);
}
static glm::vec3 mixColor(const glm::vec3 &a, const glm::vec3 &b, float t)
{
    return a + (b - a) * t;
}

//! целочисленный хеш для узлов решетки шума
static float latticeValue(int x, int y, int seed)
{
    uint32_t h = static_cast<uint32_t>(x) * 374761393u +
                 static_cast<uint32_t>(y) * 668265263u +
                 static_cast<uint32_t>(seed) * 2147483647u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (h & 0xffffff) / float(0xffffff);
}
//! бесшовный (периодический) value noise с периодом period узлов
static float tileNoise(float x, float y, int period, int seed)
{
    int x0 = static_cast<int>(floorf(x));
    int y0 = static_cast<int>(floorf(y));
    float fx = smooth(x - x0);
    float fy = smooth(y - y0);
    int xa = ((x0 % period) + period) % period;
    int ya = ((y0 % period) + period) % period;
    int xb = (xa + 1) % period;
    int yb = (ya + 1) % period;
    float v00 = latticeValue(xa, ya, seed);
    float v10 = latticeValue(xb, ya, seed);
    float v01 = latticeValue(xa, yb, seed);
    float v11 = latticeValue(xb, yb, seed);
    float a = v00 + (v10 - v00) * fx;
    float b = v01 + (v11 - v01) * fx;
    return a + (b - a) * fy;
}

View3DSky::View3DSky()
{
    cloudHeight  = 9000.0f;
    clouds       = true;
    cloudTexture = 0;
    // солнце: азимут 315 град, угол места 20 град (видно из начального положения камеры)
    const float az = 315.0f * float(M_PI) / 180.0f;
    const float el = 20.0f  * float(M_PI) / 180.0f;
    sunDir = glm::normalize(glm::vec3(cosf(el) * cosf(az), sinf(el), cosf(el) * sinf(az)));
}

void View3DSky::init()
{
    if(cloudTexture != 0)
        return;

    // фрактальный шум из нескольких октав, плотность облаков задается порогом
    std::vector<uint8_t> pixels(CLOUD_TEX_SIZE * CLOUD_TEX_SIZE * 4);
    for(int y = 0; y < CLOUD_TEX_SIZE; y++)
    {
        for(int x = 0; x < CLOUD_TEX_SIZE; x++)
        {
            float n = 0.0f, amp = 0.5f, sum = 0.0f;
            for(int oct = 0, period = 4; oct < 5; oct++, period *= 2)
            {
                float k = float(period) / CLOUD_TEX_SIZE;
                n   += amp * tileNoise(x * k, y * k, period, oct + 1);
                sum += amp;
                amp *= 0.5f;
            }
            n /= sum;
            float density = clamp01((n - 0.48f) / 0.25f);
            density = smooth(density);
            // плотные облака немного темнее снизу
            uint8_t shade = static_cast<uint8_t>(255.0f - 45.0f * density);
            uint8_t *p = &pixels[(y * CLOUD_TEX_SIZE + x) * 4];
            p[0] = shade;
            p[1] = shade;
            p[2] = static_cast<uint8_t>(shade + (255 - shade) / 3);
            p[3] = static_cast<uint8_t>(230.0f * density);
        }
    }
    glGenTextures(1, &cloudTexture);
    glBindTexture(GL_TEXTURE_2D, cloudTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec3 View3DSky::skyColor(float sinElevation, float altitude) const
{
    // с ростом высоты небо темнеет
    float k = clamp01(altitude / 25000.0f);
    const glm::vec3 zenith  = mixColor(glm::vec3(0.16f, 0.38f, 0.78f),
                                       glm::vec3(0.03f, 0.08f, 0.30f), k);
    const glm::vec3 horizon = mixColor(glm::vec3(0.72f, 0.84f, 0.96f),
                                       glm::vec3(0.45f, 0.62f, 0.90f), k);
    const glm::vec3 haze(0.55f, 0.64f, 0.74f);

    if(sinElevation >= 0.0f)
        return mixColor(horizon, zenith, powf(sinElevation, 0.45f));
    // ниже горизонта - дымка (обычно закрыта рельефом)
    return mixColor(horizon, haze, clamp01(-sinElevation * 4.0f));
}

void View3DSky::drawDome(const glm::vec3 &cameraPos, float radius)
{
    const int slices = 48;
    const int stacks = 24;
    // от -20 град до зенита
    const float elMin = -20.0f * float(M_PI) / 180.0f;
    const float elMax = float(M_PI) / 2.0f;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    glPushMatrix();
    glTranslatef(cameraPos.x, cameraPos.y, cameraPos.z);

    for(int i = 0; i < stacks; i++)
    {
        float el0 = elMin + (elMax - elMin) * i / stacks;
        float el1 = elMin + (elMax - elMin) * (i + 1) / stacks;
        glm::vec3 c0 = skyColor(sinf(el0), cameraPos.y);
        glm::vec3 c1 = skyColor(sinf(el1), cameraPos.y);
        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= slices; j++)
        {
            float az = 2.0f * float(M_PI) * j / slices;
            glColor3f(c1.r, c1.g, c1.b);
            glVertex3f(radius * cosf(el1) * cosf(az), radius * sinf(el1), radius * cosf(el1) * sinf(az));
            glColor3f(c0.r, c0.g, c0.b);
            glVertex3f(radius * cosf(el0) * cosf(az), radius * sinf(el0), radius * cosf(el0) * sinf(az));
        }
        glEnd();
    }
    // закрываем низ купола цветом дымки
    glm::vec3 cb = skyColor(-1.0f, cameraPos.y);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(cb.r, cb.g, cb.b);
    glVertex3f(0.0f, -radius, 0.0f);
    for(int j = 0; j <= slices; j++)
    {
        float az = 2.0f * float(M_PI) * j / slices;
        glVertex3f(radius * cosf(elMin) * cosf(az), radius * sinf(elMin), radius * cosf(elMin) * sinf(az));
    }
    glEnd();

    // солнце: ореол и диск в плоскости, перпендикулярной направлению на солнце
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::vec3 up = fabsf(sunDir.y) < 0.99f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
    glm::vec3 u = glm::normalize(glm::cross(sunDir, up));
    glm::vec3 v = glm::cross(u, sunDir);
    glm::vec3 c = sunDir * (radius * 0.95f);
    const float sizes[2]  = {0.18f, 0.028f};   // тангенс углового радиуса ореола и диска
    const float alphas[2] = {0.45f, 1.0f};
    for(int layer = 0; layer < 2; layer++)
    {
        float r = radius * sizes[layer];
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.98f, 0.88f, alphas[layer]);
        glVertex3f(c.x, c.y, c.z);
        glColor4f(1.0f, 0.95f, 0.80f, layer == 0 ? 0.0f : 0.9f);
        for(int j = 0; j <= 32; j++)
        {
            float a = 2.0f * float(M_PI) * j / 32;
            glm::vec3 p = c + (u * cosf(a) + v * sinf(a)) * r;
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
}

void View3DSky::drawClouds(const glm::vec3 &cameraPos, float radius, double time)
{
    if(clouds == false || cloudTexture == 0)
        return;

    const int rings  = 20;
    const int slices = 48;
    // смещение текстуры ветром (по модулю размера плитки, чтобы не терять точность)
    const float windX = fmodf(float(time) * CLOUD_WIND_X, CLOUD_TILE);
    const float windZ = fmodf(float(time) * CLOUD_WIND_Z, CLOUD_TILE);
    // начало координат текстуры привязано к плитке под камерой
    const float baseX = floorf(cameraPos.x / CLOUD_TILE) * CLOUD_TILE;
    const float baseZ = floorf(cameraPos.z / CLOUD_TILE) * CLOUD_TILE;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
                 GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // облака - кольца вокруг камеры, прозрачность растет к краю
    for(int i = 0; i < rings; i++)
    {
        float r0 = radius * i / rings;
        float r1 = radius * (i + 1) / rings;
        float a0 = 1.0f - smooth(float(i) / rings);
        float a1 = 1.0f - smooth(float(i + 1) / rings);
        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= slices; j++)
        {
            float az = 2.0f * float(M_PI) * j / slices;
            float ca = cosf(az), sa = sinf(az);
            float x0 = cameraPos.x + r0 * ca, z0 = cameraPos.z + r0 * sa;
            float x1 = cameraPos.x + r1 * ca, z1 = cameraPos.z + r1 * sa;

            glColor4f(1.0f, 1.0f, 1.0f, a0);
            glTexCoord2f((x0 - baseX + windX) / CLOUD_TILE, (z0 - baseZ + windZ) / CLOUD_TILE);
            glVertex3f(x0, cloudHeight, z0);
            glColor4f(1.0f, 1.0f, 1.0f, a1);
            glTexCoord2f((x1 - baseX + windX) / CLOUD_TILE, (z1 - baseZ + windZ) / CLOUD_TILE);
            glVertex3f(x1, cloudHeight, z1);
        }
        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
}
