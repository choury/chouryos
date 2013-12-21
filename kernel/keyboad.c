#include <chouryos.h>
#include <keyboad.h>

static int keyhead=0;
static int keytail=0;
static u8  keybuff[10];

void KeyBoadHandler(){
    u8 a=inp(0x60);
    int h=keyhead;
    h=(h+1)%10;
    if(h!=keytail){
        keybuff[keyhead]=a;
        keyhead=h;
    }
    outp(0x20,0x20);
}


char getone() {
    static const char table[]= {' ',' ','1','2','3','4','5','6','7','8','9','0','-','=',
                                '\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
                                ' ', 'a','s','d','f','g','h','j','k','l',';','\'','`',
                                ' ', '\\','z','x','c','v','b','n','m',',','.','/',' ',
                                ' ',' ',' ',' ',' ',
                               };
    static const char table_s[]= {' ',' ','!','@','#','$','%','^','&','*','(',')','_','+',
                                  '\b','\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
                                  ' ', 'A','S','D','F','G','H','J','K','L',':','\"','~',
                                  ' ', '|','Z','X','C','V','B','N','M','<','>','?',' ',
                                  ' ',' ',' ',' ',' ',
                                 };
    static int flag=0;
goback:
    while(keyhead==keytail);
    unsigned char a=keybuff[keytail];
    if(++keytail==10)
        keytail=0;
    if(a==0x2a || a==0x36) {
        flag |= KEY_SHIFT;
        goto goback;
    }
    if(a==0xaa || a==0xb6) {
        flag &= (~KEY_SHIFT);
        goto goback;
    }
    if(a==0x3a) {
        flag |= KEY_CAP;
        goto goback;
    }
    if(a==0xba) {
        flag &= ~KEY_CAP;
        goto goback;
    }
    if(a==0x38) {
        flag |= KEY_ALT;
        goto goback;
    }
    if(a==0xb8) {
        flag &= ~KEY_ALT;
        goto goback;
    }
    if(a==0x1d) {
        flag |= KEY_CTRL;
        goto goback;
    }
    if(a==0x9d) {
        flag &= ~KEY_CTRL;
        goto goback;
    }
    if(a<=0x39) {
        if(flag & KEY_SHIFT) {
            return table_s[a];
        } else {
            return table[a];
        }
    }
    goto goback;
}
