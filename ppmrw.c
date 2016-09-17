//
//  ppmrw.c
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


int main(int argc, char** argv)
{
	if(argc != 4) // checks for 4 arguments which includes the argv[0] path argument as well as the 3 required arguments of format [P# input.ppm output.ppm]
	{
		fprintf(stderr, "Error: Incorrect number of arguments; format should be -> [# input.ppm output.ppm]\n");
		return -1;
	}
	
	// block of code storing each of the 3 arguments passed into the command line
	char *file_type = argv[1];
	char *input_name = argv[2];
	char *output_name = argv[3];
	
	if(strcmp(file_type, "3") != 0 && strcmp(file_type, "6") != 0)
	{
		fprintf(stderr, "Error: Given file format is invalid; desired final file format should be specified as '3' or '6'\n");
		return -1;
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
	header_buffer->file_format = (char *)malloc(100);
	header_buffer->file_comment = (char *)malloc(1024);
	header_buffer->file_height = (char *)malloc(100);
	header_buffer->file_width = (char *)malloc(100);
	header_buffer->file_maxcolor = (char *)malloc(100);
	
	
	// function calls which start the bulk of the program, including reading header data, reading image data, and then writing out both that header and image data into a file
	read_header_data(input_name); // reads and parses header information

	// intermediate image_buffer memory allocation here as file_width and file_height were previously unavailable
	image_buffer = (image_data *)malloc(sizeof(image_data) * atoi(header_buffer->file_width) * atoi(header_buffer->file_height)  + 1); // + 1
	
	read_image_data(input_name); // reads and stores image information
	
    write_image_data(file_type, output_name); // writes out stored header and image information to file 
	printf("Success! Finished writing!\n");	
	
	return 0;
	
}

// read_header_data function takes in a sole input_file_name argument in order to know which file to read from
void read_header_data(char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
	
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
	if(maxcolor != 255)
		{
			fprintf(stderr, "Error: Image not an 8-bit channel (max color value is not 255)\n");
		    exit(1); // exits out of program due to error	
		}
	
	current_location = ftell(fp); // stores location of file pointer after header is read in for later use when reading image data
	 
	fclose(fp);
}

// read_image_data function takes in a sole input_file_name argument in order to know which file to read from
void read_image_data(char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
		
	// error checking again on input_file to validate its existence
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
		
	// fseek points file pointer to space right after header information in the input file, so that the image_data buffer only reads in image data
	fseek(fp, current_location, SEEK_SET);
	
	// strcmp to check for type of input file format
	if(strcmp(header_buffer->file_format, "P3") == 0)
	{
		
		char* current_line; // character pointer used to read information from fgets
		current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
		char temp[64] = {0}; // temporary character array to store header information later on
		int c = fgetc(fp); // initializes int c to the first character in the input file
		int i = 0; // initializes iterator variable
		int j = 0; // initializes iterator variable
		int current_number = 0; // used hold the atoi value of the number read in
		image_data current_pixel; // temp image_data struct which will hold RGB pixels
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through global buffer
		current_pixel.r = '0';
		current_pixel.g = '0'; // initializes current pixel RGB values to 0
		current_pixel.b = '0';
		
		// while loop used to guarantee that there isn't any white space after header data
		while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n' || !(c >= '0' && c <= '9')) 
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
		
		// while loop which continually reads in pixels from the file
		while(1)
		{
			if(c == '\n' || c == ' ' || feof(fp)) // checks for white space or end of file to verify that pixel has been fully stored in temp
            {
				temp[i] = 0; // adds null-terminator to stored pixel
				current_number = atoi(temp); // converts temp to integer for error checking
				if(j == 0) // evaluates if current pixel is the "r" in "RGB"
				{
					current_pixel.r = atoi(temp); // stores integer form of temp into current_pixel.r
					j++; // increments j to indicate next pixel value should be "g"
							
				}
				else if(j == 1) // evaluates if current pixel is the "g" in "RGB"
				{
					current_pixel.g = atoi(temp); // stores integer form of temp into current_pixel.g
					j++; // increments j to indicate next pixel value should be "b"
						
				}
				else if(j == 2) // evaluates if current pixel is the "b" in "RGB"
				{
					current_pixel.b = atoi(temp); // stores integer form of temp into current_pixel.b
					j = 0; // resets j value to 0 to indicate new pixel set
					*temp_ptr = current_pixel; // effectively stores current pixel in temporary buffer
					temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
					current_pixel.r = '0';
					current_pixel.g = '0'; // resets current pixel RGB values to 0
					current_pixel.b = '0';
				}
				i = 0; // resets iterator variable to 0
				memset(temp, 0, 64); // resets all values in temp to 0 for later use
				
				
				if(current_number < 0 || current_number > 255)
				{
					fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
					exit(1); // exits out of program due to error				
				}
				c = fgetc(fp); // grabs the next character from the file						
				
			
			}
			if(feof(fp)) // breaks out of loop after last number has been added
			{
				break;
			}

			temp[i++] = c; // stores non-white space character into temp chaacter array
			c = fgetc(fp);	// stores next character in file in c	
					
		}
		fclose(fp);
	}
	
	// strcmp to check for type of input file format
	else if(strcmp(header_buffer->file_format, "P6") == 0)
	{	
		char* current_line; // character pointer used to read information from fgets
		current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
		char temp[64] = {0}; // temporary character array to store header information later on
		int c = fgetc(fp); // initializes int c to the first character in the input file
		int i = 0; // initializes iterator variable
		int current_number = 0; // used hold the atoi value of the number read in
		image_data current_pixel; // temp image_data struct which will hold RGB pixels
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through global buffer
		current_pixel.r = '0';
		current_pixel.g = '0'; // initializes current pixel RGB values to 0
		current_pixel.b = '0';
		
	
		// while loop used to guarantee that there isn't any white space after header data
		while(c == '#' || c == ' ' || c == '\t' || c == '\r' || c == '\n') 
		{
			if(c == '#')
			{
				
				fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
				current_location = ftell(fp); // since P6 doesn't use fgetc after initial whitespace removal verification, current_location needs to be stored before fget(c) to avoid missing the first number of the file
				c = fgetc(fp); // new character grabbed from file for next while evaluation
			}
			else
			{
				current_location = ftell(fp); // same logic as above comment
				c = fgetc(fp); // if there was whitespace and it wasn't a comment, then grab next character for loop evaluation
			}
		}

				
		fclose(fp); // after using fgetc to verify white space after header is gone, closes file
		fopen(input_file_name, "rb"); // reopens file to be able to read in bytes
		fseek(fp, current_location, SEEK_SET); // sets file pointer to previously calculated current_location global variable
		
		
		// while loop which iterates for every pixel in the file using width * height
		while(i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
		{
			fread(&current_pixel.r, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.r field
			fread(&current_pixel.g, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.g field
			fread(&current_pixel.b, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.b field
			*temp_ptr = current_pixel; // effectively stores current pixel in temporary buffer
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			
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
			current_pixel.g = '0'; // resets current pixel RGB values to 0
			current_pixel.b = '0';
			i++; // increments iterator variable
		}
		
		fclose(fp);
	}
	
	// file format was neither P3 nor P6 so exits with error
	else
	{
		fprintf(stderr, "Error: File format to read in not recognized\n");
		exit(1); // exits out of program due to error	
	}
}

// write_image_data function takes in file_format to verify file output as well as the output_file_name to know where to open the file
void write_image_data(char* file_format, char* output_file_name)
{
	*(header_buffer->file_format + 1) = *(file_format); // rewrites # in P# to match correct destination file format before writing out
	FILE *fp;
	
	fp = fopen(output_file_name, "a"); // opens file to be appended to (file will be created if one does not exist)
	
	if(fp == NULL) 
	{
		fprintf(stderr, "Error: File couldn't be created/modified.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line;
	
	// block of code which writes header information into the output file along with whitespaces accordingly
	fprintf(fp, header_buffer->file_format); 
	fprintf(fp, "\n");
	fprintf(fp, header_buffer->file_width);
	fprintf(fp, " ");
	fprintf(fp, header_buffer->file_height);
	fprintf(fp, "\n");
	fprintf(fp, header_buffer->file_maxcolor);
	fprintf(fp, "\n");
	
	// strcmp to check for type of input file format
	if(strcmp(header_buffer->file_format, "P3") == 0)
	{		
		int i = 0; // initializes iterator variable
		unsigned char temp[64] = {0}; // creates temp array of unsigned char
		char temp_string[64]; // creates temp_string to hold converted value from file as a string
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through stored pixels in the global buffer
		
		// while loop which iterates for every pixel in the file using width * height
		while(i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
	    {
			sprintf(temp_string, "%d", (*temp_ptr).r); // converts read-in pixel "r" value to a string
			fprintf(fp, temp_string); // writes converted pixel as a string to the file
			memset(temp_string, 0, 64); // resets all values in temp to 0 for reuse
			fprintf(fp, "\n"); // prints a newline to act as "whitespace" between pixel information
			
			sprintf(temp_string, "%d", (*temp_ptr).g); // converts read-in pixel "g" value to a string
			fprintf(fp, temp_string); // writes converted pixel as a string to the file
			memset(temp_string, 0, 64); // resets all values in temp to 0 for reuse
			fprintf(fp, "\n"); // prints a newline to act as "whitespace" between pixel information
			
			sprintf(temp_string, "%d", (*temp_ptr).b); // converts read-in pixel "b" value to a string	
			fprintf(fp, temp_string); // writes converted pixel as a string to the file
			memset(temp_string, 0, 64); // resets all values in temp to 0 for reuse
			fprintf(fp, "\n"); // prints a newline to act as "whitespace" between pixel information
				
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			i++;  // increments iterator variable
		}

		fclose(fp);
    }
	
	// strcmp to check for type of input file format
	else if(strcmp(header_buffer->file_format, "P6") == 0)
	{
		fclose(fp); // closes file after writing header information since P6 requires writing bytes
		fopen(output_file_name, "ab"); // opens file to be appended to in byte mode
		int i = 0; // initializes iterator variable
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through stored pixels in the global buffer
		
		// while loop which iterates for every pixel in the file using width * height
		while(i != atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
	    {
			fwrite(&(*temp_ptr).r, sizeof(unsigned char), 1, fp); // writes the current pixels "r" value of an "unsigned char" byte to the file
			fwrite(&(*temp_ptr).g, sizeof(unsigned char), 1, fp); // writes the current pixels "g" value of an "unsigned char" byte to the file
			fwrite(&(*temp_ptr).b, sizeof(unsigned char), 1, fp); // writes the current pixels "b" value of an "unsigned char" byte to the file
				
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			i++;  // increments iterator variable
		}
		
		fclose(fp);
	}
	
	// extra error checking in case file format given was invalid, but should have been caught earlier
	else
	{
		fprintf(stderr, "Error: File format to write out not recognized\n");
		fclose(fp); // closes file before exiting out
		exit(1); // exits out of program due to error	
	}
}

