#ifndef __GRAPHY_H__
#define __GRAPHY_H__

typedef unsigned int RGB_t;

#define RGB(r,g,b) (RGB_t)((((r)<<16)&0xff0000) | (((g)<<8)&0xff00) | ((b)&0xff))

void drawLine(int x1, int y1, int x2, int y2,RGB_t color);

#endif
