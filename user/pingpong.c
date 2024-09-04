#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// #include <stdio.h>

// #include <stdlib.h>

// #include <unistd.h>

// #include <string.h>

// #include <fcntl.h>

// #include <assert.h>

// #include <sys/wait.h>

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
        n = read(pipe_pair[0], buf, 1024);
        printf("(pid:%d) :received ping\n", (int) getpid());
        write(pipe_pair[1], "pong", 4);

    } else {
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
        write(pipe_pair[1], "ping", 4);
        n = read(pipe_pair[0], buf, 1024);

        if (n < 0) {
                printf("Error reading from pipe: %d", n);
                exit(-1);
        }

        buf[n] = '\0';

        printf("Read %d bytes from pipe: %s\n", n, buf);

    }
    exit(0);

}