#ifndef __GRAPHY_H__
#define __GRAPHY_H__


#define RGB(r,g,b) (short)((((r)<<11)&0xf800) | (((g)<<6)&0x07e0) | ((b)&0x001f))

void drawLine(int x1, int y1, int x2, int y2,int color);

#endif
