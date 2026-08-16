#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char* argv[])
{
    // handle no args
    if(argc<3)
    {
        printf("Missing Arguements. USAGE cp <source> <destination>\n");
        exit(0);
    }

    // open files for read and write
    int fileToRead = open(argv[1],O_RDONLY);

    if(fileToRead<0)
    {
        printf("Failed to open source file for reading ...\n");
        exit(1);
    }

    int fileToWrite = open(argv[2],O_WRONLY|O_CREATE);

    if(fileToWrite<0)
    {
        printf("Failed to open destination file for writing ...\n");
        exit(1);
    }

    char buffer[1024];
    int bytesRead;

    while((bytesRead = read(fileToRead,&buffer,sizeof(buffer)))>0)
    {
        write(fileToWrite,buffer,bytesRead);
    }

    // close file Descriptors
    close(fileToRead);
    close(fileToWrite);

    exit(0);
}

