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
		exit(1);
	}
	
	printf("File opened successfully\n");
	
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	
	fclose(fp);
	
	printf("Calculated file size is: %d\n", filesize);
	
	
	printf("Buffer hasn't been properly allocated memory yet and buffer size is: %d\n", sizeof(buffer));
	
	buffer = (struct pixel_data*)malloc(sizeof(struct pixel_data)); // allocates memory to struct pointer
	buffer->file_data = (unsigned char *)calloc(filesize, sizeof(unsigned char));
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
	read_image_data(filetype, inputname);
	
	return 0;
	
}

void read_header_data(char* file_format, char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
	
	printf("Input file name is: %s\n", input_file_name);
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line;
	
	printf("Getting to this point and file_format is: %s okay\n", file_format);

	//fgets(current_line, 1024, fp);
	
	//printf("Current line is %s and length is %d\n", current_line, strlen(current_line));
	
	printf("Passed fgets\n");
	//printf("About to read file format into buffer:%s\n", current_line);
	
	printf("Current stored buffer file format is: %s\n", buffer->file_format);
	
	
	strcpy(buffer->file_format, fgets(current_line, 1024, fp));
	
	//(*buffer).file_format = (unsigned char *)malloc(3);
	
	printf("Current stored buffer file format is: %s\n", buffer->file_format);
	printf("Current line is %s and length is %d\n", current_line, strlen(current_line));

	
	//buffer->file_format = fgets(current_line, 1024, fp); // stores file format 
	
	if(strcmp(buffer->file_format, "P3\n") == 0)
	{
		printf("Yes the formats are equal\n");
	}

	
    //printf("file_format equal to %d and %d and %d\n", strcmp(buffer->file_format, "P3"), strcmp(buffer->file_format, "P3\\"), strcmp(buffer->file_format, "P3\n"));
	if((strcmp(buffer->file_format, "P3\n") != 0) && (strcmp(buffer->file_format, "P6\n") != 0))
	{
		fprintf(stderr, "Error: Given file format is neither P3 nor P6.\n");
		exit(1); // exits out of program due to error		
	}
	
	
	 fgets(current_line, 1024, fp);
	 
	 printf("Current_line contains: %s\n", current_line);
	 
	 int is_comment = 1;
	 
	 //checks for comment
	 if(*current_line == '#')
	 {
		strcpy(buffer->file_comment, current_line);
		is_comment = 0;
		
		fgets(current_line, 1024, fp);
		 
		char* width_and_height = strtok(current_line, " ");
		
		int width = atoi(width_and_height);
		strcpy(buffer->file_width, width_and_height);
		printf("Current width is: %d\n",width);
		
		width_and_height = strtok(NULL, " ");
		
		int height = atoi(width_and_height);
		strcpy(buffer->file_height, width_and_height);
		printf("Current height is: %d\n", height);
		
		if(height < 0 || width < 0)
		{
			fprintf(stderr, "Error: Invalid height or width.\n");
		    exit(1); // exits out of program due to error
		}
		
		strcpy(buffer->file_maxcolor, fgets(current_line, 1024, fp));
		
		if(atoi(current_line) < 0 || atoi(current_line) > 255)
		{
			fprintf(stderr, "Error: Max color value is off\n");
		    exit(1); // exits out of program due to error	
		}
		
		printf("Current max color value is: %d\n", atoi(current_line));
		
		
	 }
	 
	 //evaluates if there was no comment, simply changing the flow of reading in each line (same process as above minus the comment read/assignment"
	 if(is_comment == 1)
	 {
	    char* width_and_height = strtok(current_line, " ");
		
		int width = atoi(width_and_height);
		strcpy(buffer->file_width, width_and_height);
		printf("Current width is: %d\n",width);
		
		width_and_height = strtok(NULL, " ");
		
		int height = atoi(width_and_height);
		strcpy(buffer->file_height, width_and_height);
		printf("Current height is: %d\n", height);
		
		if(height < 0 || width < 0)
		{
			fprintf(stderr, "Error: Invalid height or width.\n");
		    exit(1); // exits out of program due to error
		}
		
		strcpy(buffer->file_maxcolor, fgets(current_line, 1024, fp));
		
		if(atoi(current_line) < 0 || atoi(current_line) > 255)
		{
			fprintf(stderr, "Error: Max color value is off\n");
		    exit(1); // exits out of program due to error	
		}
		
		printf("Current max color value is: %d\n", atoi(current_line));
		
	 }
	 
	fclose(fp);
}

void read_image_data(char* file_format, char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
	
	printf("Input file name is: %s\n", input_file_name);
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line;
	
	printf("The current file format is %s\n", buffer->file_format);
	
	if(strcmp(buffer->file_format, "P3\n") == 0)
	{
		
		int current_number = 0;
		while(1)
		{
			//current_character = atoi(fgetc(fp));
			fgets(current_line, 1024, fp); // CHANGE THE MAX NUMBER
			if(feof(fp))
			{
				break;
			}
			strcat(buffer->file_data, current_line);
			current_number = atoi(current_line);
			printf("Current line is: %s\n", current_line);
			printf("Current line after conversion is: %d\n", atoi(current_line));
			
		    if(current_number < 0 || current_number > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
				exit(1); // exits out of program due to error				
			}
			
			
		}
		printf("Final buffer is: %s\n", buffer->file_data);
	}
	
	else if(strcmp(buffer->file_format, "P6\n") == 0)
	{
		
	}
	
	else
	{
		fprintf(stderr, "Error: File format not recognized\n");
		exit(1); // exits out of program due to error	
	}
}

void write_image_data(char* file_format, char* output_file_name)
{
	
}