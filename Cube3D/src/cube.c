/*
  This is simple demonstration program to create 3D (wire-frame) cube.
  This code creates 3D space. Inside of this space there is drawn a cube.
  The cube can be translated and rotated. The projection plane is placed
  to the beginning of the coordinate system and it is fixed. 
  The cube is drawn only in front of the projection plane to a certain
  distance. The drawing of the cube is limited also in width and height.
*/

#include "cube_dat.h"

point_t P[8];
point_t middle;

vector_t Vx;
vector_t Vy;
vector_t Vz;
vector_t normal;

border_t border;

axis_t axis;

int translation;
int angle;

char g;


int main(void)
{
    init_window();

    /* The middle point of the projection */
    middle.x = getmaxx() / 2; 
    middle.y = getmaxy() / 2;
    middle.z = -(getmaxx() / 2);

    /* Normal vector of the pojection plane. */
    normal.x = 0; 
    normal.y = 0;
    normal.z = 1;

    /* Space limits where the cube is visible. */
    border.z1 = 0;
    border.z2 = 400;

    border.x = getmaxx() * 4;
    border.y = getmaxy() * 4;

    /* Translation and rotation speed. */
    translation = 10;
    angle = 10;

    init(100, 100, 5, 80);

    g = '0';
    while (g != 'q')
    {
        cleardevice();
        draw();

        if (kbhit())
        {
            g = getch();
            switch (g)
            {
              case 'a' : translate(-translation,0,0); break;
              case 'd' : translate( translation,0,0); break;
              case 's' : translate(0,translation, 0); break;
              case 'w' : translate(0,-translation,0); break;
              case 'r' : translate(0,0, translation); break;
              case 'f' : translate(0,0,-translation); break;
              case 'u' : rotate( angle,AXIS_Y);       break;
              case 'o' : rotate(-angle,AXIS_Y);       break;
              case 'j' : rotate( angle,AXIS_Z);       break;
              case 'l' : rotate(-angle,AXIS_Z);       break;
              case 'i' : rotate( angle,AXIS_X);       break;
              case 'k' : rotate(-angle,AXIS_X);       break;
            }
        }

        delay(1);
    }

    closegraph(ALL_WINDOWS);
    return 0;
}

