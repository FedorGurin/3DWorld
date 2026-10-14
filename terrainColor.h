#ifndef TERRAINCOLOR_H
#define TERRAINCOLOR_H

#include <QtGlobal>

//! цвет рельефа по высоте и крутизне склона (slope: 1 - горизонтально, 0 - отвесно)
inline void terrainColor(float h, float slope, float c[3])
{
    struct TColorStop {float h; float r, g, b;};
    static const TColorStop stops[] = {
        {   0.0f, 0.24f, 0.42f, 0.20f},   // низменность
        { 300.0f, 0.33f, 0.52f, 0.24f},
        { 900.0f, 0.50f, 0.54f, 0.30f},   // предгорья
        {1600.0f, 0.52f, 0.44f, 0.31f},   // горы
        {2600.0f, 0.50f, 0.47f, 0.44f},   // скалы
        {3400.0f, 0.93f, 0.94f, 0.97f},   // снег
    };
    const int n = sizeof(stops) / sizeof(stops[0]);
    if(h < 3.0f)
    {
        // вода
        c[0] = 0.16f; c[1] = 0.32f; c[2] = 0.55f;
        return;
    }
    int k = 0;
    while(k < n - 2 && h > stops[k + 1].h)
        k++;
    float t = (h - stops[k].h) / (stops[k + 1].h - stops[k].h);
    t = qBound(0.0f, t, 1.0f);
    c[0] = stops[k].r + (stops[k + 1].r - stops[k].r) * t;
    c[1] = stops[k].g + (stops[k + 1].g - stops[k].g) * t;
    c[2] = stops[k].b + (stops[k + 1].b - stops[k].b) * t;
    // крутые склоны - каменистые, снег на них не держится
    float rock = qBound(0.0f, (0.85f - slope) / 0.25f, 1.0f);
    const float rc[3] = {0.45f, 0.42f, 0.38f};
    for(int i = 0; i < 3; i++)
        c[i] = c[i] + (rc[i] - c[i]) * rock * 0.8f;
}

#endif // TERRAINCOLOR_H
