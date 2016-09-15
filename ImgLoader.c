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

void read_image_data(char* file_format, char* input_file_name, int file_size);

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
	//read_image_data(filetype, inputname, filesize);
	//write_image_data(filetype, outputname);
	
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
	current_line = (char *)malloc(1500);
	char temp[64] = {0};
	int c = fgetc(fp);
	int i = 0;

	
	while(c != ' ') // potentially change while loop to account for all white space
	{
		printf("C is currently: %c\n", c);
		if(c == '\n')
			{
			    temp[i++] = c;	
                break;				
			}
			temp[i++] = c;
            c = fgetc(fp);			
	}
	
	temp[i] = 0;
	strcpy(buffer->file_format, temp);
	memset(temp, 0, 64);
	
	
	printf("file format is: %s\n", buffer->file_format);
	
	if(strcmp(buffer->file_format, "P3\r\n") == 0)
		printf("They're equal\n");
	
	printf("size of file_format is: %d\n", strlen(buffer->file_format));
	
    //printf("file_format equal to %d and %d and %d\n", strcmp(buffer->file_format, "P3"), strcmp(buffer->file_format, "P3\\"), strcmp(buffer->file_format, "P3\n"));
	if((strcmp(buffer->file_format, "P3\n") != 0) && (strcmp(buffer->file_format, "P6\n") != 0) && (strcmp(buffer->file_format, "P3\r\n") != 0) && (strcmp(buffer->file_format, "P6\r\n") != 0) && (strcmp(buffer->file_format, "P3") != 0) && (strcmp(buffer->file_format, "P6") != 0))
	{
		fprintf(stderr, "Error: Given file format is neither P3 nor P6.\n");
		exit(1); // exits out of program due to error		
	}
	
	
	// file width
	i = 0;
	c = fgetc(fp);
	printf("Is c a comment? %c\n", c);
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n' || !(c >= '0' && c <= '9'))
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp);
		    c = fgetc(fp);
		}
		else
		{
			c = fgetc(fp);
		}
	}
	while(c != ' ')
	{
		printf("C is currently: %c\n", c);
		if(c == '\n')
			{
			    temp[i++] = c;	
                break;				
			}
		temp[i++] = c;		
		c = fgetc(fp);
	}
	
	temp[i] = 0;
	strcpy(buffer->file_width, temp);
	int width = atoi(temp);
	printf("File width is: %d\n", width);
	memset(temp, 0, 64);
	
	
	
	// file height
	i = 0;
	c = fgetc(fp);
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n' || !(c >= '0' && c <= '9'))
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp);
		    c = fgetc(fp);
		}
		else
		{
			c = fgetc(fp);
		}
	}
	while(c != ' ')
	{
		printf("C is currently: %c\n", c);
		if(c == '\n')
			{
			    temp[i++] = c;	
                break;				
			}
		temp[i++] = c;		
		c = fgetc(fp);
	}
	
	temp[i] = 0;
	strcpy(buffer->file_height, temp);
	int height = atoi(temp);
	printf("File height is: %d\n", height);
	memset(temp, 0, 64);
	
	
	if(height < 0 || width < 0)
		{
			fprintf(stderr, "Error: Invalid height or width.\n");
		    exit(1); // exits out of program due to error
		}
		
		
	
    // max color
	i = 0;
	c = fgetc(fp);
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n' || !(c >= '0' && c <= '9')) // potentially remove non-digit checker
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp);
		    c = fgetc(fp);
		}
		else
		{
			c = fgetc(fp);
		}
	}
	while(c != ' ')
	{
		printf("C is currently: %c\n", c);
		if(c == '\n')
			{
			    temp[i++] = c;	
                break;				
			}
		temp[i++] = c;
		c = fgetc(fp);
			
	}
	
	temp[i] = 0;
	strcpy(buffer->file_maxcolor, temp);
	int maxcolor = atoi(temp);
	printf("File maxcolor is: %d\n", maxcolor);
	memset(temp, 0, 64);
	
	if(maxcolor < 0 || maxcolor > 255)
		{
			fprintf(stderr, "Error: Max color value is off\n");
		    exit(1); // exits out of program due to error	
		}
	
	 
	fclose(fp);
}

void read_image_data(char* file_format, char* input_file_name, int file_size)
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
	
	//TRY getc?
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
		fclose(fp);
	}
	
	else if(strcmp(buffer->file_format, "P6\n") == 0)
	{
		fclose(fp);
		fopen(input_file_name, "rb");
		unsigned char * test;
		test = (unsigned char *)calloc(1, sizeof(unsigned char));
		fread(buffer->file_data, sizeof(unsigned char), file_size, fp); // CHANGE 5000 TO FILE SIZE

		printf("Final buffer is: %s\n", buffer->file_data);		
		fclose(fp);
	}
	
	else
	{
		fprintf(stderr, "Error: File format to read in not recognized\n");
		exit(1); // exits out of program due to error	
	}
}

void write_image_data(char* file_format, char* output_file_name)
{
	*(buffer->file_format + 1) = *(file_format + 1); // rewrites # in P# to match correct destination file format before writing out
	printf("Getting to start\n");
	FILE *fp;
	
	//printf("Output file name is: %s\n", output_file_name);
	fp = fopen(output_file_name, "a"); // error checking around creating new file?
	
	//printf("Output file name is: %s\n", output_file_name);
	
	if(fp == NULL) // CHANGE this?
	{
		fprintf(stderr, "Error: File couldn't be created/modified.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line;
	
	printf("The current file format is %s\n", buffer->file_format);
	
	if(strcmp(file_format, "P3") == 0)
	{	
		printf("Printing P3 data\n");
        fprintf(fp, buffer->file_data);

		fclose(fp);
    }
	else if(strcmp(file_format, "P6") == 0)
	{
		fclose(fp);
		fopen(output_file_name, "wb");
		fwrite(buffer->file_data, sizeof(unsigned char), strlen(buffer->file_data), fp); // CHANGE THE MAX NUMBER HERE
		
		fclose(fp);
	}
	
	else
	{
		fprintf(stderr, "Error: File format to write out not recognized\n");
		exit(1); // exits out of program due to error	
	}
	//close file?
}