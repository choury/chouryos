global start
extern main
extern fork

use32
start:
    call main
    jmp $
