//
//  FBerry_CS430_ImageLoader.c
//  CS430 Project 1
//
//  Frankie Berry
//


// pre-processor directives
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "Error: Incorrect number of arguments; format should be -> [P# input.ppm output.ppm]\n");
		return -1;
	}
	
    char *filetype = argv[1];
	char *inputname = argv[2];
	char *outputname = argv[3];

	
	
	
	return 0;
}

