#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    int p[2];  // Initial pipe for feeding numbers
    pipe(p);   // Create a pipe

    if (fork() == 0) {  // First child process
        close(p[1]);    // Close write-end of the first pipe (child only reads)

        // Infinite loop to handle each prime number
        while (1) {
            int prime;
            // Read the first number from the pipe, which will be a prime
            if (read(p[0], &prime, sizeof(prime)) == 0) {
                close(p[0]);  // Close the read-end when no more data
                exit(0);      // Exit when done
            }
            printf("prime %d\n", prime);  // Output the prime number

            int newp[2];  // Create a new pipe for the next stage
            pipe(newp);

            if (fork() == 0) {  // Fork a new process to handle the next prime
                close(newp[1]);  // Next process will only read
                p[0] = newp[0];  // Update the read-end for the new process
                continue;        // Continue the loop to handle the next prime
            } else {
                close(newp[0]);  // Parent process closes read-end
                int n;

                // Filter numbers that are not divisible by the current prime
                while (read(p[0], &n, sizeof(n)) > 0) {
                    if (n % prime != 0) {
                        write(newp[1], &n, sizeof(n));  // Send non-divisible numbers
                    }
                }

                // Close the descriptors when done
                close(p[0]);
                close(newp[1]);
                wait(0);  // Wait for the child process to finish
                exit(0);  // Parent process exits
            }
        }
    } else {
        close(p[0]);  // Parent closes read-end of the pipe

        // Parent feeds numbers 2 through 35 into the pipeline
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));  // Write each number to the pipe
        }

        close(p[1]);  // Close the write-end after sending all numbers
        wait(0);      // Wait for the child process to finish
    }

    exit(0);  // Exit when done
}