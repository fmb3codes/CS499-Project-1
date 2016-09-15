//
//  FBerry_CS430_ImageLoader.c
//  CS430 Project 1
//
//  Frankie Berry
//


// pre-processor directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel_data {
  char* file_format;
  char* file_comment;
  char* file_height;
  char* file_width;
  char* file_maxcolor;
  void* file_data;
} pixel_data;

pixel_data *buffer;

void read_header_data(char* file_format, char* input_file_name);

void read_image_data(char* file_format, char* input_file_name);

void write_image_data(char* file_format, char* output_file_name);

//REMEMBER to go through and add error checking for incorrect file type/argument size/etc
int main(int argc, char** argv)
{
	
	printf("argc is: %d\n", argc);
	if(argc != 4) // checks for 4 arguments which includes the argv[0] path argument as well as the 3 required arguments of format [P# input.ppm output.ppm]
	{
		fprintf(stderr, "Error: Incorrect number of arguments; format should be -> [P# input.ppm output.ppm]\n");
		return -1;
	}
	
	char *filetype = argv[1];
	char *inputname = argv[2];
	char *outputname = argv[3];
	
	printf("Filetype is: %s\ninputname is: %s\noutputname is: %s\n", filetype, inputname, outputname);
	
	FILE *fp;
	int filesize;
	
	fp = fopen(inputname, "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		return -1;
	}
	
	printf("File opened successfully\n");
	
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	
	fclose(fp);
	
	printf("Calculated file size is: %d\n", filesize);
	
	
	printf("Buffer hasn't been properly allocated memory yet and buffer size is: %d\n", sizeof(buffer));
	
	buffer = (struct pixel_data*)malloc(sizeof(struct pixel_data)); // allocates memory to struct pointer
	buffer->file_data = (unsigned char *)malloc(filesize);
	buffer->file_format = (char *)malloc(3);
	buffer->file_comment = (char *)malloc(1024);
	buffer->file_height = (char *)malloc(100);
	buffer->file_width = (char *)malloc(100);
	buffer->file_maxcolor = (char *)malloc(100);
	
	// Potentially remove this
	if(buffer == NULL || buffer->file_data == NULL || buffer->file_format == NULL || buffer->file_comment == NULL || buffer->file_height == NULL || buffer->file_width == NULL || buffer->file_maxcolor == NULL)
	{
		fprintf(stderr, "Error: Buffer wasn't properly allocated memory.\n");
		return -1;
	}
	
	printf("Buffer has been properly allocated memory and buffer size is: %d\n", sizeof(buffer));
	
	read_header_data(filetype, inputname);
	
}

void read_header_data(char* file_format, char* input_file_name)
{
	
}

void read_image_data(char* file_format, char* input_file_name)
{
	
}

void write_image_data(char* file_format, char* output_file_name)
{
	
}