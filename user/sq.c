#include "kernel/types.h"
#include "user/user.h"

int main()
{
    int sq2 = square(2);
    int sq5 = square(5);

    printf("Square of 2 is %d\n",sq2);
    printf("Square of 5 is %d\n",sq5);

    exit(0);
}

