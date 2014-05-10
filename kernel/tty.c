#include <keyboad.h>
#include <common.h>
#include <string.h>

static int line = 0;
static int colume = 0;


int tty_read(void *buff, size_t len)
{
    int count=0;
    while (count < len) {
        char a = getone();
        if(a=='\b'){
            if(count>0){
                ((char *)buff)[--count] = ' ';
            }else{
                continue;
            }
        }else{
            ((char *)buff)[count++] = a;
        }
        printf("%c", a);
        if (a == '\n')
            return count;
    }
    return len;
}


int tty_write(const void *ptr, size_t len)
{
    int count=0;
    while (count < len) {
        char c = ((char *)ptr)[count++];
        if (c == '\n') {
            line++;
            colume = 0;
        } else if (c == '\b') {
            colume--;
            if (colume < 0) {
                line--;
                colume = 79;
            }
            charbuff[line * 80 + colume] = 0x0720;
        } else {
            charbuff[line * 80 + colume++] = c | 0x0700;
        }
        if (colume == 80) {
            line++;
            colume = 0;
        }
        if (line == 25) {
            memcpy(charbuff, charbuff + 80, 80 * 26 * 2);
            line = 24;
        }
    }
    charbuff[line * 80 + colume] = 0x0720;
    outp(0x3d4, 14);
    outp(0x3d5, (line * 80 + colume) >> 8);
    outp(0x3d4, 15);
    outp(0x3d5, line * 80 + colume);
    return len;
}

