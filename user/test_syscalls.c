#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid = fork();
    if (pid == 0) {
        pause(5);
        exit(0);
    }
    
    pause(1);
    
    printSysCalls();
    printf("\n");
    printProcessSysCalls(pid);
    
    wait(0);
    exit(0);
}

