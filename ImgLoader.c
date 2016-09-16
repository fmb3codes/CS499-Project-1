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

typedef struct image_data {
  unsigned char r, g, b;
} image_data;

pixel_data *header_buffer;

image_data *image_buffer;

int current_location;

void read_header_data(char* file_format, char* input_file_name);

void read_image_data(char* file_format, char* input_file_name, int file_size);

void write_image_data(char* file_format, char* output_file_name);

void print_pixels(image_data* pixel);
//
//	FREEEEEEEE MEMORY?
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
	
	// CHECK FILE TYPE
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
	
	
	printf("Buffer hasn't been properly allocated memory yet and header_buffer size is: %d\n", sizeof(header_buffer));
	
	header_buffer = (struct pixel_data*)malloc(sizeof(struct pixel_data)); // allocates memory to struct pointer
	header_buffer->file_format = (char *)malloc(3);
	header_buffer->file_comment = (char *)malloc(1024);
	header_buffer->file_height = (char *)malloc(100);
	header_buffer->file_width = (char *)malloc(100);
	header_buffer->file_maxcolor = (char *)malloc(100);
	
	
	// Potentially remove this
	if(header_buffer == NULL || header_buffer->file_data == NULL || header_buffer->file_format == NULL || header_buffer->file_comment == NULL || header_buffer->file_height == NULL || header_buffer->file_width == NULL || header_buffer->file_maxcolor == NULL)
	{
		fprintf(stderr, "Error: Buffer wasn't properly allocated memory.\n");
		return -1;
	}
	
	printf("Buffer has been properly allocated memory and header_buffer size is: %d\n", sizeof(header_buffer));
	
	read_header_data(filetype, inputname);
	
	printf("Size of image struct is: %d\n", sizeof(image_data));
	header_buffer->file_data = (unsigned char *)malloc(sizeof(pixel_data) * atoi(header_buffer->file_width) * atoi(header_buffer->file_height)); // allocates after finding width and height
	image_buffer = (image_data *)malloc(sizeof(image_data) * atoi(header_buffer->file_width) * atoi(header_buffer->file_height)  + 1); // + 1
	
	read_image_data(filetype, inputname, filesize);
	printf("Done reading and going to write...\n");
	
	print_pixels(image_buffer);
    write_image_data(filetype, outputname);
	
	//CHECK IF NUMBERS GREATER THAN MAX COLOR VALUE
	
	
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

	
	while(1)
	{
		printf("C is currently: %c\n", c);
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if(c == '#')
			{
			fgets(current_line, 1024, fp);
			}
			break;
		}
		temp[i++] = c;
        c = fgetc(fp);			
	}
	
	temp[i] = 0;
	strcpy(header_buffer->file_format, temp);
	memset(temp, 0, 64);
	
	
	printf("file format is: %s\n", header_buffer->file_format);
	
	if(strcmp(header_buffer->file_format, "P3\r\n") == 0)
		printf("They're equal\n");
	
	printf("size of file_format is: %d\n", strlen(header_buffer->file_format));
	
    //printf("file_format equal to %d and %d and %d\n", strcmp(header_buffer->file_format, "P3"), strcmp(header_buffer->file_format, "P3\\"), strcmp(header_buffer->file_format, "P3\n"));
	if((strcmp(header_buffer->file_format, "P3") != 0) && (strcmp(header_buffer->file_format, "P6") != 0))
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
	while(1)
	{
		printf("C is currently: %c\n", c);
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if(c == '#')
			{
			fgets(current_line, 1024, fp);
			}
			break;
		}
		temp[i++] = c;
        c = fgetc(fp);			
	}
	
	temp[i] = 0;
	strcpy(header_buffer->file_width, temp);
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
	while(1)
	{
		printf("C is currently: %c\n", c);
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if(c == '#')
			{
			fgets(current_line, 1024, fp);
			}
			break;
		}
		temp[i++] = c;
        c = fgetc(fp);			
	}
	
	temp[i] = 0;
	strcpy(header_buffer->file_height, temp);
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
	while(1)
	{
		printf("C is currently: %c\n", c);
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if(c == '#')
			{
				fgets(current_line, 1024, fp);
			}
			break;
		}
		temp[i++] = c;
        c = fgetc(fp);			
	}
	
	temp[i] = 0;
	strcpy(header_buffer->file_maxcolor, temp);
	int maxcolor = atoi(temp);
	printf("File maxcolor is: %d\n", maxcolor);
	memset(temp, 0, 64);
	
	if(maxcolor < 0 || maxcolor > 255)
		{
			fprintf(stderr, "Error: Max color value is off\n");
		    exit(1); // exits out of program due to error	
		}
	
	current_location = ftell(fp);
	printf("Current location is: %d\n", current_location);
	 
	//free(current_line);
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
	
	//char* current_line;
	//current_line = (char *)malloc(1500);
	//char temp[64] = {0};
	//int c = fgetc(fp);
	//int i = 0;
	
	//image_buffer = (image_data *)malloc(sizeof(image_data) * atoi(header_buffer->file_width) * atoi(header_buffer->file_height));
	
	while(1)
	{
		//fseek(fp, current_location, SEEK_SET);
		//printf("Current line is: %s\n", fgets(current_line, 1024, fp));
		break; // POTENTIALLY READ UP TO NON-HEADER INFO and move to both P3 and P6 sections
	}
	
	printf("The current file format is %s\n", header_buffer->file_format);
	fseek(fp, current_location, SEEK_SET);
	
	//TRY getc?
	if(strcmp(header_buffer->file_format, "P3") == 0)
	{
		
		char* current_line;
	    current_line = (char *)malloc(1000);
	    signed char temp[64] = {0};
	    int c = fgetc(fp);
	    int i = 0;
		int j = 0;
		//int struct_counter = 0;
		int current_number = 0;
		image_data current_pixel;
		image_data* temp_ptr = image_buffer;
		current_pixel.r = '0';
		current_pixel.g = '0';
		current_pixel.b = '0';
		int line = 0;
		
		
		while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n' || !(c >= '0' && c <= '9')) // COPY OVER TO P6 SECTION (reads up to non-header data)
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
		
		while(1)
		{
			if(c == '\n' || c == ' ' || feof(fp))
            {
				printf("Found a white space\n");
				printf("On line#%d\n", line++);
				temp[i] = 0;
				printf("Temp is now: %s\n", temp);
				current_number = atoi(temp);
			    //strcat(header_buffer->file_data, temp);	
				//i++;
				printf("Current i value is %d\n", i);
				printf("Current j value is %d\n", j);
				if(j == 0)
				{
					current_pixel.r = atoi(temp);
					//current_pixel.r = temp;
					printf("Adding r pixel %d\n", current_pixel.r);
					j++;
							
				}
				else if(j == 1)
				{
					current_pixel.g = atoi(temp);
					//current_pixel.g = temp;
					printf("Adding g pixel %d\n", current_pixel.g);
					j++;
						
				}
				else if(j == 2)
				{
					current_pixel.b = atoi(temp);
					//current_pixel.b = temp;
					printf("Adding b pixel %d\n", current_pixel.b);
					j = 0;
					*temp_ptr = current_pixel;
					temp_ptr++;
					current_pixel.r = '0';
					current_pixel.g = '0';
					current_pixel.b = '0';
				}
				i = 0;
			
			
				memset(temp, 0, 64);
				
				printf("Current number is: %d\n", current_number);
				
				if(current_number < 0 || current_number > 255)
				{
					fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
					exit(1); // exits out of program due to error				
				}
				c = fgetc(fp);
				
				//printf("R pixel is:%c\nG pixel is:%c\nB pixel is:%c\n", current_pixel.r, current_pixel.g, current_pixel.b);
				//printf("R pixel is:%c\nG pixel is:%c\nB pixel is:%c\n", (*image_buffer).r, (*image_buffer).g, (*image_buffer).b);
				//print_pixel(current_pixel);
				
				
			
			}
			if(feof(fp))
			{
				break;
			}
			//current_character = atoi(fgetc(fp));
			//fgets(current_line, 1024, fp); // CHANGE THE MAX NUMBER
			
			printf("C is currently: %c\n", c);
			temp[i++] = c;			
			c = fgetc(fp);
					
		}
		//printf("Final header_buffer is: %s\n", header_buffer->file_data);
		//free(current_line);
		fclose(fp);
	}
	
	else if(strcmp(header_buffer->file_format, "P6") == 0)
	{	
		
		char* current_line;
	    current_line = (char *)malloc(1000);
	    //signed char temp[64] = {0};
	    int c = fgetc(fp);
		//printf("C before init is %d or %s\n", c, c);
	    int i = 0;
		int j = 0;
		//int struct_counter = 0;
		int current_number = 0;
		image_data current_pixel;
		image_data* temp_ptr = image_buffer;
		current_pixel.r = '0';
		current_pixel.g = '0';
		current_pixel.b = '0';
		int line = 0;
		int test_counter = 0;
		
		
		printf("C before anything else 1 is: %d or %c and size is: %d\n", c, c, sizeof(c));
		while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') // COPY OVER TO P6 SECTION (reads up to non-header data)
		{
			printf("Loop evaluated\n");
			if(c == '#')
			{
				fgets(current_line, 1024, fp);
				current_location = ftell(fp); // since P6 doesn't use fgetc after initial whitespace removal verification, current_location needs to be stored before fget(c) to avoid missing the first number of the file
				c = fgetc(fp);
			}
			else
			{
				current_location = ftell(fp); // same logic as above comment
				c = fgetc(fp);
			}
		}

				
		
		fclose(fp);
		fopen(input_file_name, "rb");
		fseek(fp, current_location, SEEK_SET);
		
		
		//for(i = 0; i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height); i++)
		//while(!feof(fp))
		while(i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
		{
			fread(&current_pixel.r, sizeof(unsigned char), 1, fp); //<-- Note inner for(j... has been removed
			fread(&current_pixel.g, sizeof(unsigned char), 1, fp);
			fread(&current_pixel.b, sizeof(unsigned char), 1, fp);	
			printf("On lines#%d\n", line+=3);
			printf("Red is: %d and Blue is %d and Green is: %d\n", current_pixel.r, current_pixel.b, current_pixel.g);
			*temp_ptr = current_pixel;
			temp_ptr++;
			
			// error checking block for each individual pixel to make sure they're not outside the color range limit
			if(current_pixel.r < 0 || current_pixel.r  > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.g < 0 || current_pixel.g > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.b < 0 || current_pixel.b > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			current_pixel.r = '0';
			current_pixel.g = '0';
			current_pixel.b = '0';
			i++;
		}
		

		//printf("Final header_buffer is: %s\n", header_buffer->file_data);		
		//free(current_line);
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
	*(header_buffer->file_format + 1) = *(file_format + 1); // rewrites # in P# to match correct destination file format before writing out
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
	
	
	printf("The current file format is %s\n", header_buffer->file_format);
	fprintf(fp, header_buffer->file_format); // need to write null-terminator?
	fprintf(fp, "\n");
	fprintf(fp, header_buffer->file_width);
	fprintf(fp, " ");
	fprintf(fp, header_buffer->file_height);
	fprintf(fp, "\n");
	fprintf(fp, header_buffer->file_maxcolor);
	fprintf(fp, "\n");
	//current_location = ftell(fp);
	//printf("New current location is: %d\n", current_location);
	
	
	if(strcmp(file_format, "P3") == 0)
	{		
		int i = 0;
		int j = 0;
		unsigned char temp[64] = {0};
		char temp_string[64];
		image_data current_pixel;
		image_data* temp_ptr = image_buffer;
		
		printf("Writing P3 data\n");
		while(i != atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
	    {
			printf("Writing Pixels to file currently with pixels %d %d %d\n", (*temp_ptr).r, (*temp_ptr).g, (*temp_ptr).b);
			sprintf(temp_string, "%d", (*temp_ptr).r);
			fprintf(fp, temp_string);
			memset(temp_string, 0, 64);
			fprintf(fp, "\n");
			
			sprintf(temp_string, "%d", (*temp_ptr).g);
			fprintf(fp, temp_string);
			memset(temp_string, 0, 64);
			fprintf(fp, "\n");
			
			sprintf(temp_string, "%d", (*temp_ptr).b);			
			fprintf(fp, temp_string);
			memset(temp_string, 0, 64);
			fprintf(fp, "\n");
				
			temp_ptr++;
			i++;
		}
        //fprintf(fp, header_buffer->file_data);

		fclose(fp);
    }
	
	else if(strcmp(file_format, "P6") == 0)
	{
		fclose(fp);
		fopen(output_file_name, "ab");
		//printf("Current location is still %d\n", current_location);
		//fseek(fp, current_location, SEEK_SET);
		int i = 0;
		char temp_string[64];
		image_data current_pixel;
		image_data* temp_ptr = image_buffer;
		
		printf("Writing P6 data\n");
		while(i != atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
	    {
			/*
			printf("Writing Pixels to file currently with pixels %d %d %d\n", (*temp_ptr).r, (*temp_ptr).g, (*temp_ptr).b);
			sprintf(temp_string, "%d", (*temp_ptr).r);			
			fwrite(temp_string, 1, sizeof(temp_string), fp);
			memset(temp_string, 0, 64);
			strcpy(temp_string, " ");
			fwrite(temp_string, 1, sizeof(temp_string), fp);
			memset(temp_string, 0, 64);
			
			sprintf(temp_string, "%d", (*temp_ptr).g);
			fprintf(fp, temp_string);
			fwrite(temp_string, 1, sizeof(temp_string), fp);
			memset(temp_string, 0, 64);
			strcpy(temp_string, " ");
			fwrite(temp_string, 1, sizeof(temp_string), fp);
			memset(temp_string, 0, 64);
			
			sprintf(temp_string, "%d", (*temp_ptr).b);	
			fwrite(temp_string, 1, sizeof(temp_string), fp);			
			fprintf(fp, temp_string);
			memset(temp_string, 0, 64);
			strcpy(temp_string, " ");
			fwrite(temp_string, 1, sizeof(temp_string), fp);
			memset(temp_string, 0, 64);*/
			printf("Writing Pixels to file currently with pixels %d %d %d\n", (*temp_ptr).r, (*temp_ptr).g, (*temp_ptr).b);
			fwrite(&(*temp_ptr).r, sizeof(unsigned char), 1, fp);
			fwrite(&(*temp_ptr).g, sizeof(unsigned char), 1, fp);
			fwrite(&(*temp_ptr).b, sizeof(unsigned char), 1, fp);
				
			temp_ptr++;
			i++;
		}
		//fwrite(header_buffer->file_data, sizeof(unsigned char), strlen(header_buffer->file_data), fp); // CHANGE THE MAX NUMBER HERE
		
		fclose(fp);
	}
	
	else
	{
		fprintf(stderr, "Error: File format to write out not recognized\n");
		exit(1); // exits out of program due to error	
	}
	//close file?
}

void print_pixels(image_data* pixels)
{
	int i = 0;
	printf("Width is: %d\nHeight is: %d\n", atoi(header_buffer->file_width), atoi(header_buffer->file_height));
	while(i != atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
	{
		char temp_str[64];
		sprintf(temp_str, "%d", (*pixels).r);
		printf("Printing pixel #%d...\n", i++); 
		printf("R pixel is:%d\nG pixel is:%d\nB pixel is:%d\n", (*pixels).r, (*pixels).g, (*pixels).b);
		printf("Printing R pixel as string: %s\n", temp_str);;
		pixels++;
	}
}