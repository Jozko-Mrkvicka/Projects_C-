#include "cube_dat.h"


void init_window(void)
{
    int width = 640;
    int height = 480;
    const char* name = "Windows BGI";
    int pos_x = 100;
    int pos_y = 100; 

    initwindow(width, height, name, pos_x, pos_y);
}


/* Adds direction vectors to all of the cube corners (all but P[1]). */
void refresh(void)
{
    P[1].x = P[0].x + Vx.x;
    P[1].y = P[0].y + Vx.y;
    P[1].z = P[0].z + Vx.z;

    P[2].x = P[0].x + Vy.x;
    P[2].y = P[0].y + Vy.y;
    P[2].z = P[0].z + Vy.z;

    P[3].x = P[0].x + Vz.x;
    P[3].y = P[0].y + Vz.y;
    P[3].z = P[0].z + Vz.z;

    P[4].x = P[0].x + Vx.x + Vy.x;
    P[4].y = P[0].y + Vx.y + Vy.y;
    P[4].z = P[0].z + Vx.z + Vy.z;

    P[5].x = P[0].x + Vx.x + Vz.x;
    P[5].y = P[0].y + Vx.y + Vz.y;
    P[5].z = P[0].z + Vx.z + Vz.z;

    P[6].x = P[0].x + Vy.x + Vz.x;
    P[6].y = P[0].y + Vy.y + Vz.y;
    P[6].z = P[0].z + Vy.z + Vz.z;

    P[7].x = P[0].x + Vx.x + Vy.x + Vz.x;
    P[7].y = P[0].y + Vx.y + Vy.y + Vz.y;
    P[7].z = P[0].z + Vx.z + Vy.z + Vz.z;
}


/* This function draws the cube. It projects all of the cube corners to the projection plane
   and connects them together. */
void draw(void)
{
	vector_t V;
	pixel_t pix[8];
	float t;
	int i;

	if ((P[0].z >= border.z1) && (P[0].z < border.z2) && 
	    (P[0].x > -border.x)  && (P[0].x < border.x)  && 
	    (P[0].y > -border.y)  && (P[0].y < border.y)) 
	{
        for (i = 0; i < 8; i++)
        {
            V.x = P[i].x - middle.x;
            V.y = P[i].y - middle.y;
            V.z = P[i].z - middle.z;

            t = -(normal.x*P[i].x + normal.y*P[i].y + normal.z*P[i].z)/(normal.x*V.x + normal.y*V.y + normal.z*V.z);

            pix[i].x = round(P[i].x + V.x*t);
            pix[i].y = round(P[i].y + V.y*t);
        }

        line(pix[0].x, pix[0].y, pix[1].x, pix[1].y);
        line(pix[0].x, pix[0].y, pix[2].x, pix[2].y);
        line(pix[0].x, pix[0].y, pix[3].x, pix[3].y);
        line(pix[3].x, pix[3].y, pix[5].x, pix[5].y);
        line(pix[3].x, pix[3].y, pix[6].x, pix[6].y);
        line(pix[4].x, pix[4].y, pix[2].x, pix[2].y);
        line(pix[4].x, pix[4].y, pix[1].x, pix[1].y);
        line(pix[4].x, pix[4].y, pix[7].x, pix[7].y);
        line(pix[5].x, pix[5].y, pix[1].x, pix[1].y);
        line(pix[5].x, pix[5].y, pix[7].x, pix[7].y);
        line(pix[6].x, pix[6].y, pix[2].x, pix[2].y);
        line(pix[6].x, pix[6].y, pix[7].x, pix[7].y);
	}
}


/* x,y,z - Position in space.
   a     - Length of edge. */
void init(int x, int y, int z, int a)
{
    P[0].x = x;
    P[0].y = y;
    P[0].z = z;

    Vx.x = a;
    Vx.y = 0;
    Vx.z = 0;

    Vy.x = 0;
    Vy.y = a;
    Vy.z = 0;

    Vz.x = 0;
    Vz.y = 0;
    Vz.z = a;

    refresh();
}


void translate(int x, int y, int z)
{
    P[0].x = P[0].x + x;
    P[0].y = P[0].y + y;
    P[0].z = P[0].z + z;

    refresh();
}


/* SX,SY - Center of rotation.
   x,y   - A point to be rotated. */
void rot2d(float *x, float *y, float Sx, float Sy, float angle)
{
	float m;
	float n;
	float c;

    c = angle * M_PI / 180;

    m = (*x - Sx) * cos(c) - (*y - Sy) * sin(c) + Sx;
    n = (*x - Sx) * sin(c) + (*y - Sy) * cos(c) + Sy;

    *x = m;
    *y = n;
 }


/* This function computes a middle of the cube (center of rotation). */
void rotate(int angle, axis_t axis)
{
	point_t S;
	int i;

    S.x = P[0].x + (P[7].x - P[0].x) / 2;
    S.y = P[0].y + (P[7].y - P[0].y) / 2;
    S.z = P[0].z + (P[7].z - P[0].z) / 2;

    /* Rotates 4 main cube points according to switched axis. */
    switch (axis)
    {
    	case AXIS_X:
            for (i = 0; i < 4; i++)
    			rot2d(&(P[i].z), &(P[i].y), S.z, S.y, angle);
    		break;

      	case AXIS_Y:
            for (i = 0; i < 4; i++)
      			rot2d(&(P[i].x), &(P[i].z), S.x, S.z, angle);
   			break;
 
      	case AXIS_Z:
            for (i = 0; i < 4; i++)
      			rot2d(&(P[i].x), &(P[i].y), S.x, S.y, angle);
   			break;
    }

    /* Computes direction vectors from 4 main cube points. */
    Vx.x = P[1].x - P[0].x;
    Vx.y = P[1].y - P[0].y;
    Vx.z = P[1].z - P[0].z;

    Vy.x = P[2].x - P[0].x;
    Vy.y = P[2].y - P[0].y;
    Vy.z = P[2].z - P[0].z;

    Vz.x = P[3].x - P[0].x;
    Vz.y = P[3].y - P[0].y;
    Vz.z = P[3].z - P[0].z;

    /* Computes new position of the cube points from direction vectors. */
    refresh(); 
}

