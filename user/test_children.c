#include "kernel/types.h"
#include "user/user.h"

void
mysleep(int ticks)
{
    int start = uptime();

    while (uptime() - start < ticks) {
        // busy wait
    }
}

int
main()
{
    int p1, p2;

    printf("[PARENT] PID: %d\n", getpid());

    // Create CHILD 1
    p1 = fork();

    if (p1 < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (p1 == 0) {

        printf("[CHILD 1] PID: %d, Parent PID: %d\n",
               getpid(), getppid());

        // Create GRANDCHILD
        int gc = fork();

        if (gc < 0) {
            printf("fork failed\n");
            exit(1);
        }

        if (gc == 0) {

            printf("[GRANDCHILD] PID: %d, Parent PID: %d\n",
                   getpid(), getppid());

            printf("[GRANDCHILD] Children: %d\n",
                   getChildCount());

            exit(0);
        }

        // Give parent time to observe us
        mysleep(20);

        printf("[CHILD 1] Children: %d\n",
               getChildCount());

        wait(0);
        exit(0);
    }

    // Create CHILD 2
    p2 = fork();

    if (p2 < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (p2 == 0) {

        printf("[CHILD 2] PID: %d, Parent PID: %d\n",
               getpid(), getppid());

        printf("[CHILD 2] Children: %d\n",
               getChildCount());

        mysleep(20);

        exit(0);
    }

    // Parent waits for Child 1 to create grandchild
    mysleep(5);

    printf("[PARENT] Children: %d\n",
           getChildCount());

    printf("[PARENT] Children of CHILD 1 (PID %d): %d\n",
           p1, getProcessChildCount(p1));

    printf("[PARENT] Children of CHILD 2 (PID %d): %d\n",
           p2, getProcessChildCount(p2));

    wait(0);
    wait(0);

    exit(0);
}

