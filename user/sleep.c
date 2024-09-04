#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if (argc > 0) {
		int time = atoi(argv[1]);
		sleep(time);
	}
	else {
		fprintf(2, "Argument not given\n");
	}
	exit(0);
}
