#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]){
    int pipe_pair[2];
    int res = pipe(pipe_pair);

    if(fork()==0){
        close(pipe_pair[1]);
    

    while(1){
        int prime;

        if (read(pipe_pair[0], &prime, sizeof(int)) == 0){
            close(pipe_pair[0]);
            exit(0);
        }
        printf("prime %d\n", prime);

        int new_pipe[2];
        pipe(new_pipe);

        if(fork()==0){
            close(new_pipe[0]);
            pipe_pair[0]=new_pipe[0];
            continue;
        }

        else{
            close(new_pipe[0]);
            int num;

            while(read(pipe_pair[0], &num, sizeof(int)) > 0){
                if(num%prime != 0){
                    write(new_pipe[1], &num, sizeof(int));
                }
            }
            close(pipe_pair[0]);
            close(new_pipe[0]);
            wait(0);
            exit(0);
        }
    }
    }

    else{
        close(pipe_pair[0]);
        for(int i=2; i<=35; i++){
            write(pipe_pair[1], &i, sizeof(i));
        }
        close(pipe_pair[1]);
        wait(0);
        
    }
    exit(0);

}