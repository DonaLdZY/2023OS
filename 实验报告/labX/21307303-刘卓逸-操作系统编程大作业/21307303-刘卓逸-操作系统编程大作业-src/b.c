#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int n = atoi(argv[1]);
    if (argc != 2 || n <= 0) {
        printf("Usage: %s positive_integer\n", argv[0]);
        return 1;
    }
    pid = fork();
    if (pid == 0) {
        printf("%d", n);
        while (n != 1) {
            if (n % 2 == 0) {
                n /= 2;
            } else {
                n = n * 3 + 1;
            }
            printf(" %d", n);
        }
        printf("\n");
    } else if (pid > 0) {
        wait(NULL);
    } else {
        printf("fork error\n");
        return 1;
    }
    return 0;
}
