#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]){
    int pipe_pair[2];
    int res = pipe(pipe_pair);
    int n;
    char buf[1024];

    if (res < 0) {
        exit(1);
    }

    int rc = fork();

    if (rc < 0) {
        exit(1);

    } else if (rc == 0) {
        while(1){
            close(pipe_pair[1]);
            int prime;
            n = read(pipe_pair[0], (int) prime, sizeof(prime));
            if(n == 0){
                exit(0);
            }
            printf("prime %d\n", n);

            
            int new_pipe[2];
            pipe(new_pipe);
            int new_rc = fork();

            if(new_rc == 0){
                pipe_pair[0] = new_pipe[0];
                continue;
            } else {
                int num;
                while(1){
                    n = read(pipe_pair[0], (int) num, sizeof(num));
                    if(n == 0){
                        break;
                    }
                    if(num % prime != 0){
                        write(new_pipe[1], num, sizeof(num));
                    }
                }
                close(new_pipe[1]);
                close(pipe_pair[0]);
                wait(0);
                exit(0);
                
            }
        }

    } else {
        close(pipe_pair[0]);
         for (int i = 2; i <= 35; i++) {
            write(pipe_pair[1], &i, sizeof(i));
        }

        close(pipe_pair[1]);
        wait(0);

    }
    exit(0);

}