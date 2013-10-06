#ifndef __KEYBOAD_H__
#define __KEYBOAD_H__

#define  KEY_SHIFT 0x1
#define  KEY_CTRL  0x2
#define  KEY_ALT   0x4
#define  KEY_CAP   0x10


#define keyhead (*(unsigned char*)4622)
#define keytail (*(unsigned char*)4623)
#define keybuff ((unsigned char *)4612)

void KeyBoadHandler();

char getone();

#endif
