#include <chouryos.h>

int main(){
    while(1){
        char a;
        read(STDIN_FILENO,&a,1);
        write(STDOUT_FILENO,&a,1);
    }
    return 0;
}
