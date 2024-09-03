#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char *argv[]) {
	if (argc > 1)
		Sleep(atoi(argv[1]));
	else
		printf("Argument not given\n");
	exit(0);
}