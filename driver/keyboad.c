#include <common.h>
#include <keyboad.h>
#include <schedule.h>
#include <malloc.h>
#include <signal.h>

static int keyhead=0;
static int keytail=0;
static uint8  keybuff[10];

static int flag=0;

void KeyBoadHandler(){
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
    outp(0x20,0x20);
    uint8 a=inp(0x60);
    if(a==0x2a || a==0x36) {
        flag |= KEY_SHIFT;
        goto ret;
    }
    if(a==0xaa || a==0xb6) {
        flag &= (~KEY_SHIFT);
        goto ret;
    }
    if(a==0x3a) {
        flag |= KEY_CAP;
        goto ret;
    }
    if(a==0xba) {
        flag &= ~KEY_CAP;
        goto ret;
    }
    if(a==0x38) {
        flag |= KEY_ALT;
        goto ret;
    }
    if(a==0xb8) {
        flag &= ~KEY_ALT;
        goto ret;
    }
    if(a==0x1d) {
        flag |= KEY_CTRL;
        goto ret;
    }
    if(a==0x9d) {
        flag &= ~KEY_CTRL;
        goto ret;
    }
    if(a<=0x39) {
        if((flag & KEY_CTRL) && table[a] == 'c'){
            kill(curpid,SIGTERM);
            goto ret;
        }
        if((flag & KEY_SHIFT) || (flag & KEY_CAP)) {
            a = table_s[a];
        } else {
            a = table[a];
        }
        int h=keyhead;
        h=(h+1)%10;
        if(h==keytail && ++keytail==10){
            keytail=0;
        }
        keybuff[keyhead]=a;
            keyhead=h;
    }
    int i;
ret:
    for(i=0;i<MAX_PROCESS;++i){
        if(PROTABLE[i].status == waiting &&
            PROTABLE[i].waitfor == DTTY){
            unblock(i);
        }
    }
}


char getone() {
    while(keyhead==keytail){
        block(curpid,DTTY);
    }
    unsigned char a=keybuff[keytail];
    if(++keytail==10)
        keytail=0;
    return a;
}
