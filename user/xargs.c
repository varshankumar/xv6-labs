#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        exit(1);
    }

    char buf[512];
    int n;
    char* args[MAXARG];
    
    for(int i = 1; i < argc; i++){
        args[i-1] = argv[i];
    }

    while((n = read(0, buf, sizeof(buf))) > 0){
        int i = 0;
        while(i < n){
            if(buf[i] == ' ' || buf[i] == '\n'){
                buf[i] = '\0';
                args[argc-1] = buf;
                if(fork() == 0){
                    exec(args[0], args);
                    exit(0);
                }
                wait(0);
                i++;
            }
            i++;
        }
    }
    exit(0);
}