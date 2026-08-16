#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
	if(argc<2)
	{
		printf("No arguements passed. Usage head <filename> <count> \n");
		exit(0);
	}

	// valid args passes
	// step1 : open file read
	int fileDescriptor = open(argv[1],O_RDONLY);

	if(fileDescriptor<0)
	{
		printf("Error opening file for reading \n");
		exit(1);
	}

	// step2:  create a singre char buffer to store read char
	char buffer;
	int linesToRead=5;

	// check for number of bytes to read
	if(argc<3)
	{
		printf("No count to read found defaulting to 5... \n");
	}
	else
	{
		linesToRead = atoi(argv[2]); // convert ascii number to int 
	}

	// step 3: read one byte at a time to handle new lines
	int newLinesRead=0;
	int bytesRead;

	while(newLinesRead<linesToRead)
	{
		if((bytesRead=read(fileDescriptor,&buffer,1))<=0)
		{
			printf("Reached EOF!");
			break;
		}

		write(1,&buffer,1);

		if(buffer=='\n')
		{
			newLinesRead++;
		}
	}

	// close if all success
	close(fileDescriptor);
	exit(0);
}
