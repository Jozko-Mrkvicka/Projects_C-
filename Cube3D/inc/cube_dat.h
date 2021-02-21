#ifndef CUBE_DAT_H
#define CUBE_DAT_H

#include <graphics.h>
#include <math.h>
#include "cube.h"

extern point_t P[];
extern point_t middle;

extern vector_t Vx;
extern vector_t Vy;
extern vector_t Vz;
extern vector_t normal;

extern border_t border;

extern axis_t axis;

extern int translation;
extern int angle;

extern char g;


void init_window(void);
void refresh(void);
void draw(void);
void init(int x, int y, int z, int a);
void rot2d(float x, float y, float Sx, float Sy, float angle);
void translate(int x, int y, int z);
void rotate(int angle, axis_t axis);

#endif

