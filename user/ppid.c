#include "kernel/types.h"
#include "user/user.h"

int main()
{
    
    int childProcessId = fork();

    if(childProcessId<0)
    {
        printf("Failed to create child process. Exiting...\n");
        exit(1);
    }
    else if(childProcessId==0)
    {
        // child process code
        printf("[CHILD] PID: %d \n",getpid());
        printf("[CHILD] PARENT PID: %d \n", getppid());
    }
    else
    {

        wait(0);

        // parent process code
        printf("[PARENT] Forking a child process... \n");
        printf("[PARENT] PID is %d \n",getpid());
    }

    exit(0);

}
