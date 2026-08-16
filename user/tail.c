#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char* argv[])
{

    // using double read to implement tail
    // could use queue to keep N latest lines but keeping it simple here
    
    // step 1 : open file for read

    if(argc<2)
    {
        printf("No arguements found! USAGE: tail <filename> <count> \n");
        exit(0);
    }

    int fileDescriptor = open(argv[1],O_RDONLY);
    if(fileDescriptor<0)
    {
        printf("Error opening file for reading");
        exit(1);
    }

    // step 2: Read complete file once and find total new lines
    int newLines = 0;
    char buffer;
    int bytesRead = 0;

    while((bytesRead = read(fileDescriptor,&buffer,1))>0)
    {
        if(buffer=='\n')
        {
            newLines++;
        }
    }

    close(fileDescriptor);

    // step 3 read N lines
    fileDescriptor = open(argv[1],O_RDONLY);

    if(fileDescriptor<0)
    {
        printf("Error opening file for reading\n");
        exit(0);
    }


    int linesToRead=5;

    if(argc<3)
    {
        printf("No lines to read entered defaulting to 5...\n");
    }
    else
    {
        linesToRead= atoi(argv[2]);
    }

    int linesToSkip = newLines<linesToRead ? 0 :  newLines-linesToRead;

    while((bytesRead=read(fileDescriptor,&buffer,1))>0)
    {
        if(linesToSkip==0)
        {
            write(1,&buffer,1);
            continue;
        }
        else if(buffer=='\n')
        {
            linesToSkip--;
        }
    }

    close(fileDescriptor);
    exit(0);
}

