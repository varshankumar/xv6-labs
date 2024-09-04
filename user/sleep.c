#include<unistd.h>
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if (argc > 1)
		sleep(atoi(argv[1]));
	else
		fprintf(2, "Argument not given\n");
	exit(0);
}
