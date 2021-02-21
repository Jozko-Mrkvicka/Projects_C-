#ifndef CUBE_H
#define CUBE_H

typedef struct
{
	float x;
	float y;
	float z;
} point_t;

typedef struct
{
	float x;
	float y;
} point2d_t;

typedef struct
{
    int x;
    int y;
} pixel_t;

typedef struct
{
	float x;
	float y;
	float z;
} vector_t;

typedef struct
{
    int x;
    int y;
    int z1;
    int z2;
} border_t;

typedef enum
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
} axis_t;

#endif

