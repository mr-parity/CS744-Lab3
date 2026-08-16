#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{

    if(argc<2)
    {
        printf("No arguements passed. USAGE cmd <syscall> <arguements...> \n");
        exit(0);
    }

    // create a child process using fork
    int childProcessId = fork();

    if(childProcessId<0)
    {
        printf("Failed to create child process. \n");
        exit(1);
    }
    else if(childProcessId==0)
    {
        // child code
        exec(argv[1],&argv[1]);

        // on failure
        // exit(1)
    }
    else
    {
        // parent code
        wait(0);
    }

    exit(0);
}
