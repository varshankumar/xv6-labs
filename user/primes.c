#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    int p[2];  // First pipe for communication
    pipe(p);

    if (fork() == 0) {  // Child process that will handle the sieve
        while (1) {
            close(p[1]);  // Close the write-end for the child
            int prime;

            // Read the first number from the pipe (this is the prime number)
            if (read(p[0], &prime, sizeof(prime)) == 0) {
                close(p[0]);  // Close the read-end when no more data
                exit(0);  // Exit if no more data
            }

            printf("prime %d\n", prime);  // Print the prime number

            int newp[2];  // Create a new pipe for the next process
            pipe(newp);

            if (fork() == 0) {  // Fork a new child for the next sieve stage
                p[0] = newp[0];  // Next process will read from newp
                continue;  // Continue loop to handle the next prime in the chain
            } else {
                close(newp[0]);  // Parent process closes the read end
                int n;

                // Filter numbers and pass them to the next pipe
                while (read(p[0], &n, sizeof(n)) > 0) {
                    if (n % prime != 0) {
                        write(newp[1], &n, sizeof(n));
                    }
                }

                // Close all file descriptors when done
                close(p[0]);
                close(newp[1]);
                wait(0);  // Wait for the child process to finish
                exit(0);  // Parent process exits
            }
        }
    } else {
        close(p[0]);  // Parent closes the read-end of the initial pipe

        // Send numbers 2 to 35 into the pipe
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]);  // Close the write-end after sending all numbers
        wait(0);  // Wait for the first child process to finish
    }

    exit(0);
}