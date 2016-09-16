//
//  FBerry_CS430_Project1.c
//  CS430 Project 1
//
//  Frankie Berry
//

// pre-processor directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// header_data buffer which is intended to contain all relevant header information
typedef struct header_data {
  char* file_format;
  char* file_comment;
  char* file_height;
  char* file_width;
  char* file_maxcolor;
} header_data;

// image_data buffer which is intended to hold a set of RGB pixels represented as unsigned char's
typedef struct image_data {
  unsigned char r, g, b;
} image_data;

// global header_data buffer
header_data *header_buffer;

// global image_data buffer
image_data *image_buffer;

// global variable meant to track a file pointer, specifically in order to track where the header information in a .ppm file ends for convenience
int current_location;

//function prototypes
void read_header_data(char* input_file_name); // function meant to read and parse through the header information of a .ppm file

void read_image_data(char* input_file_name); // function meant to read and parse through all image information located after the header information in a .ppm file

void write_image_data(char* file_format, char* output_file_name); // function meant to write header and image data into a specified .ppm format (P3 or P6); when called, necessary information has been read into the global buffers

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
	
	// block of code storing each of the 3 arguments passed into the command line
	char *file_type = argv[1];
	char *input_name = argv[2];
	char *output_name = argv[3];
	
	// CHECK FILE TYPE
	printf("Filetype is: %s\ninput_name is: %s\noutput_name is: %s\n", file_type, input_name, output_name);
	
	if(strcmp(file_type, "P3") != 0 && strcmp(file_type, "P6") && strcmp(file_type, "p3") != 0 && strcmp(file_type, "p6"))
	{
		fprintf(stderr, "Error: Given file format is invalid; file format should be 'P3' or 'P6'\n");
		return -1;
	}
	
	// capitalizes the P in P# if necessary for convenience later on
	if(*file_type >= 'a' && *file_type <= 'z')
	{
		*(file_type) = 'P'; 
	}
	
	// block of code which checks to make sure that user inputted a .ppm file for both the input and output command line arguments
	char* temp_ptr;
	int input_length = strlen(input_name);
	int output_length = strlen(output_name);
	
	temp_ptr = input_name + (input_length - 4); // sets temp_ptr to be equal to the last 4 characters of the input_name, which should be .ppm
	if(strcmp(temp_ptr, ".ppm") != 0)
	{
		fprintf(stderr, "Error: Input file must be a .ppm file\n");
		return -1;
	}
	
	temp_ptr = output_name + (output_length - 4); // sets temp_ptr to be equal to the last 4 characters of the output_name, which should be .ppm
	if(strcmp(temp_ptr, ".ppm") != 0)
	{
		fprintf(stderr, "Error: Output file must be a .ppm file\n");
		return -1;
	}
	// end of .ppm extension error checking	
		
	
	// block of code which immediately checks to see if input file exists, whereas output file will be created if not
	FILE *fp;
	int filesize;
	
	fp = fopen(input_name, "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		return -1;
	}	
	// end of input file error checking
	
	
	// block of code allocating memory to global header_buffer before its use
	header_buffer = (struct header_data*)malloc(sizeof(struct header_data)); 
	header_buffer->file_format = (char *)malloc(3);
	header_buffer->file_comment = (char *)malloc(1024);
	header_buffer->file_height = (char *)malloc(100);
	header_buffer->file_width = (char *)malloc(100);
	header_buffer->file_maxcolor = (char *)malloc(100);
	
	
	// function calls which start the bulk of the program, including reading header data, reading image data, and then writing out both that header and image data into a file
	read_header_data(input_name); // reads and parses header information

	// intermediate image_buffer memory allocation here as file_width and file_height were previously unavailable
	image_buffer = (image_data *)malloc(sizeof(image_data) * atoi(header_buffer->file_width) * atoi(header_buffer->file_height)  + 1); // + 1
	
	read_image_data(input_name); // reads and stores image information
	printf("Done reading and going to write...\n");
	
	print_pixels(image_buffer); 
    write_image_data(file_type, output_name); // writes out stored header and image information to file 
	printf("All finished!\n");
	
	//CHECK IF NUMBERS GREATER THAN MAX COLOR VALUE
	
	
	return 0;
	
}

// read_header function takes in a sole input_file_name argument in order to know which file to read from
void read_header_data(char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
	
	printf("Input file name is: %s\n", input_file_name);
	
	// error checking again on input_file to validate its existence
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line; // character pointer used to read information from fgets
	current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
	char temp[64] = {0}; // temporary character array to store header information later on
	int c = fgetc(fp); // initializes int c to the first character in the input file
	int i = 0; // initializes iterator variable

	// determining file format
	// while loop which is intended to read in the file format until whitespace is found
	while(1)
	{
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') // white space conditional
		{
			if(c == '#')
			{
				fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			}
			break; // breaks out of whitespace once file format is read
		}
		temp[i++] = c; // stores non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c		
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_format, temp); // copies what should be the file_format (P3/P6) into the header_buffer->file_format field
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	// error check to make sure .ppm file contains either P3 or P6 as its magic number
	if((strcmp(header_buffer->file_format, "P3") != 0) && (strcmp(header_buffer->file_format, "P6") != 0))
	{
		fprintf(stderr, "Error: Given file format is neither P3 nor P6.\n");
		exit(1); // exits out of program due to error		
	}
	
	
	// determining file width
	i = 0; // resets iterator variable to 0
	c = fgetc(fp); // grabs the next character in the file
	
	// while loop which loop so long as the current character read in is whitespace, ignoring it accordingly
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
		    c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		else
		{
			c = fgetc(fp); // if there was whitespace and it wasn't a comment, then grab next character for loop evaluation
		}
	}
	
	// new while loop which should be entered only after the current character is a non-whitespace character
	while(1)
	{
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') // if statement which checks for white space
		{
			if(c == '#')
			{
				fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			}
			break; // breaks out of while loop once file width is read
		}
		temp[i++] = c; // stores non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c			
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_width, temp); // stores file width as a string in the global header_buffer
	int width = atoi(temp); // converts read-in width to int for error checking
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	
	
	// determining file height
	i = 0; // resets iterator variable to 0
	c = fgetc(fp); // grabs the next character in the file
	
	// while loop which loop so long as the current character read in is whitespace, ignoring it accordingly
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
		    c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		else
		{
			c = fgetc(fp); // if there was whitespace and it wasn't a comment, then grab next character for loop evaluation
		}
	}
	
	// new while loop which should be entered only after the current character is a non-whitespace character
	while(1)
	{
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') // if statement which checks for white space
		{
			if(c == '#')
			{
				fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			}
			break; // breaks out of while loop once file height is read
		}
		temp[i++] = c; // stores non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c			
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_height, temp); // stores file height as a string in the global header_buffer
	int height = atoi(temp); // converts read-in height to int for error checking
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	 
	// error check to make sure height and width are both greater than 0
	if(height < 0 || width < 0)
		{
			fprintf(stderr, "Error: Invalid height or width.\n");
		    exit(1); // exits out of program due to error
		}
		
		
    // determining max color
	i = 0; // resets iterator variable to 0
	c = fgetc(fp); // grabs the next character in the file
	
	// while loop which loop so long as the current character read in is whitespace, ignoring it accordingly	
	while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
		    c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		else
		{
			c = fgetc(fp); // if there was whitespace and it wasn't a comment, then grab next character for loop evaluation
		}
	}
	
	// new while loop which should be entered only after the current character is a non-whitespace character
	while(1)
	{
		if(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') // if statement which checks for white space
		{
			if(c == '#')
			{
				fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			}
			break; // breaks out of while loop once file maxcolor is read
		}
		temp[i++] = c; // stores non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c			
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_maxcolor, temp); // stores file maxcolor as a string in the global header_buffer
	int maxcolor = atoi(temp); // converts read-in maxcolor to int for error checking
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	// error check to make sure max color fits within the correct color channel for this project
	if(maxcolor < 0 || maxcolor > 255)
		{
			fprintf(stderr, "Error: Max color value is off\n");
		    exit(1); // exits out of program due to error	
		}
	
	current_location = ftell(fp); // stores location of file pointer after header is read in for later use when reading image data
	 
	fclose(fp);
}

void read_image_data(char* input_file_name)
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