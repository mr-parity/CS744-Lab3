#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void mysleep(int seconds) {
    long long limit = (long long)seconds * 100000000;
    for (volatile long long i = 0; i < limit; i++) {
    }
}

int main() {
    int n = 3;
    int pids[3]; // Fixed: Changed from 'int pids;' to an array
    int pid = getpid();
    
    printf("[parent] pid: %d\n", pid);
    printf("[parent] forking %d children\n", n);
    
    int ret = nfork(n, pids);
    if (ret == 0) {
        int current_pid = getpid();
        
        if (current_pid == pid + 1) {
            mysleep(1);
        } else if (current_pid == pid + 2) {
            mysleep(2);
        } else if (current_pid == pid + 3) {
            mysleep(3);
        }
        
        printf("[child] pid: %d, ppid: %d\n", current_pid, getppid());
        exit(0);
    } else if (ret == n) {
        for(int i = 0; i < n; i++) {
            wait(0);
        }
        
        int count = getChildCount();
        printf("[parent] get_child_count returned %d\n", count);
    }
    
    exit(0);
}

