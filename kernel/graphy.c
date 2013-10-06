#include <chouryos.h>
#include <graphy.h>




void BresenhamLine(int x1, int y1, int x2, int y2,int color,int mod)
{
    int x,y,dx,dy,p;
    dx=x2-x1;
    dy=y2-y1;
    x=x1;
    y=y1;
    p=2*dy-dx;
    for(; x<x2; x++) {
        switch(mod) {
        case 1:                     //线为1>m>0
            setpoint(x,y,color);
            break;
        case 2:                     //线为m>0
            setpoint(y,x,color);
            break;
        case 3:                     //线为-1<m<0
            setpoint(x,2*y1-y,color);
            break;
        case 4:                     //线为m<-1
            setpoint(y,2*x1-x,color);
            break;
        }
        if(p>=0) {
            y++;
            p+=2*(dy-dx);
        } else {
            p+=2*dy;
        }
    }
}


void drawLine(int x1, int y1, int x2, int y2,int color)
{

    if(x1>x2) {                         //交换两个点坐标
        int temp=x2;
        x2=x1;
        x1=temp;
        temp=y2;
        y2=y1;
        y1=temp;
    }
    if(y1<y2) {
        if(x2-x1>y2-y1) {               //1>m>0
            BresenhamLine(x1,y1,x2,y2,color,1);
        } else {                        //m>1
            BresenhamLine(y1,x1,y2,x2,color,2);
        }
    } else {
        if(x2-x1>y1-y2) {               //0>m>-1
            BresenhamLine(x1,y1,x2,2*y1-y2,color,3);
        } else {                        //m>-1
            BresenhamLine(y1,x1,2*y1-y2,x2,color,4);
        }
    }
}
