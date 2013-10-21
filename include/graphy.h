#ifndef __GRAPHY_H__
#define __GRAPHY_H__

typedef unsigned short RGB_t;

#define RGB(r,g,b) (RGB_t)((((r)<<11)&0xf800) | (((g)<<6)&0x07e0) | ((b)&0x001f))

void drawLine(int x1, int y1, int x2, int y2,RGB_t color);

#endif
