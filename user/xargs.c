#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    int n_flag = 1;
    int command = 1;
    if (argc >= 3 && strcmp(argv[1],"-n")==0) {
        n_flag = atoi(argv[2]);
        command=3;
        if (n_flag < 1 || argc < 4) {
            exit(1);
        }
    } else if (argc<2) {
        exit(1);
    }

    char buf[512];
    char *xargv[MAXARG];

    for (int i = command; i < argc; i++) {
        xargv[i - command] = argv[i];
    }

    while (1) {
        int n = 0;
        while (read(0,buf+n, 1) == 1 && buf[n] != '\n') {
            n++;
            if (n >= sizeof(buf)-1) {
                exit(1);
            }
        }

        if (n == 0) break;

        buf[n] = 0;

        xargv[argc-command] = buf;
        xargv[argc-command+1]=0;

        if (fork() == 0) {
            exec(argv[command], xargv);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}
