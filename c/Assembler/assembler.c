#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <unistd.h>
#include "assembler.h"

/** This program does the following things: 
 * 	
 * 	1. Open a file to read in from
 * 	2. Read the file and locate the labels.  
 * 	3. Read in a line from the the file.
 * 	4. Parse the line to get the assembly
 * 		instructions. 
 * 	5. Pack the assembly instructions into a number. 
 * 	6. Print out the decimal converted assembly instructions.    
 *
 **/
#define MAXMEM 65536


FILE* inFile; 
FILE* outFile;
int	  numberOfLines; 
char *tags[MAXMEM]; 

int main (int argc, char *argv[] ) 
{

	int opt;
	char line[100]; 

	while ((opt = getopt (argc, argv, "i:o:")) != -1)
	{
		//printf("Option is: %c \n", opt); 
		//printf("Optarg is: %s \n", optarg); 
		switch (opt)
		{
		 	 case 'i':
				 //input file 
				 inFile = fopen(optarg, "r");
				 break; 

			case 'o': 
				 //output file

				 outFile = fopen(optarg, "w"); 
				 break; 

		 	default: 
				 //printf("%c is not an option", opt); 
				 break; 
		}

   	 }

	 numberOfLines = countLines(inFile); 	
	 
	 locateTags(inFile);  

	 int lineNumber = 0; 	 
	 rewind(inFile); 
	 while( fgets( line, 100, inFile) != NULL)
	 {
		 
		//when there is no outfile. 
		if(outFile == NULL)
		{
			printf("%d\n", mainRoutine(line, lineNumber) ); 
		}
		//when there is an outfile. 			
		else
		{
			fprintf(outFile, "%d\n", mainRoutine(line, lineNumber) ); 	
		}
		lineNumber = lineNumber + 1; 
	 }	 

	 fclose(inFile);  

	 if( outFile != NULL)
	 {
		 fclose(outFile); 
	 }

}

//This function brings all of our 
//functions together so that we can 
//translate assembly into machine code. 
int mainRoutine( char line[], int lineNumber ) 
{
	char* assemblyElements[5]; 
	int assemblyElementsAsInt[5];

	parseLine( line, assemblyElements); 

	translateIntoInts( assemblyElements, assemblyElementsAsInt, lineNumber);

	int result = packIntoNumber( assemblyElementsAsInt, lineNumber ); 	
	return result;  
}


//Locates the tag addresses, 
//takes a file object, and an array to store the tags in.  
void locateTags(FILE* file)
{
	char line[100]; 
	int lineNumber = 0; 

	rewind(file); 
	while( fgets( line, 100, file) != NULL)
	{
		int i = 0; 
		char c = line[0];
		char* subArray = malloc(6 * sizeof(char) ); 	

		while( isWhiteSpace(c) != 1)
		{
			subArray[i] = c; 

			i = i + 1; 
			c = line[i]; 
		}
		tags[lineNumber] = subArray; 
		lineNumber = lineNumber + 1; 
	}
}

int countLines(FILE* file)
{
	int count = 0; 
	char line[100]; 
	
	rewind(file); 
	while( fgets( line, 100, file) != NULL)
	{
		count = count + 1; 
	}
	return count; 
}

//returns the address of the tag. 
//if the tag is not located return -1. 
int tagAddress( char tagName[]) 
{
	for(int j = 0; j < numberOfLines; j++)
	{
		if( strcmp( tagName, tags[j] ) == 0) 
		{
			return j; 
		}
	}
	return -1; 
}


int isAtag( char tagName[] )
{
	if( tagAddress( tagName ) != -1)
	{
		return 1; 
	}
	else { return 0; } 
}

//This function takes in a one dememsional arary 
//of chars and out puts a 2-D char array that 
//contains the assembly instruction. 
//NOTE: this can only store one instruction line in the output
//array at a time. 
void parseLine (char inStr[], char* outStr[] )
{

	//doing these two while loops 4 times should 
	//get us from the first whitespace 
	//to the begining of the comments.
	//
	// i - Keep track of element number
	// k - Keep track of character position 
	// j - keep track of subarray position.   

	int  k = 0; 
	for(int i = 0; i < 4; i++)
	{
		int  j = 0; 
		char c = inStr[k]; 
		char   *subArray = malloc( 6 * sizeof(char) ); 
		
		while(  isWhiteSpace( c ) == 1 ) 
		{
			k = k + 1; 
			c = inStr[k]; 
		}
		
		while( isWhiteSpace(c) == 0 )
		{
			subArray[j] = c;
		
			//incr j k and char
			j = j + 1; 
			k = k + 1; 
			c = inStr[k]; 
		}

		//put our temp array into our result. 
		if( determineOppcode(subArray) != -1 && i > 0)
		{
			//if there is a tag, then write over the tag with the oppcode. 
			outStr[i-1] = subArray; 
			i = i -1; 
		}
		else 
		{
			outStr[i] = subArray; 
		}

		
		//If the Instruction type is type j, f or o we 
		//Don't want to fill the array. 
		if(i == 0 )
		{
			char type = determineInstructionType( subArray); 

			if(type == 'o')
			{
				outStr[1] = NULL; 
				outStr[2] = NULL; 
				outStr[3] = NULL; 
				break;
			}
		}

		if( i == 1) 
		{
			char type = determineInstructionType( outStr[0] ); 
			if( type == 'f' || type == 'j') 
			{
				outStr[2] = NULL; 
				outStr[3] = NULL; 
				break;
			}	
		}
		
	}//end of For Loop. 
	
	//ensure that the string is null terminated. 
	outStr[4] = NULL;

}



//This function takes an array that contains the elements of an
//instruction and will fill the outArray with the 
//corresponding decimal values in the 
//same order that they came in as. 
void translateIntoInts(char* instruction[], int outArr[], int lineNumber)
{
	int i = 0;
	while( instruction[i] != NULL)
	{
		//add the oppcode into the output
		if(i == 0)
		{
			outArr[i] = determineOppcode( instruction[i] ); 
		}
		//if it is not the oppcode, then it 
		//is either a register number or a tag. 
		else 
		{
			//if it is a register number then 
			//convert the char to the corresponding int. 
			// '5' - '0' = 5
			if( isAtag( instruction[i] ) == 0)  
			{
				int digit = *instruction[i]; 

				//if it is a negitive number. 
				if( *instruction[i] == 45)
				{
					digit = instruction[i][1]; 
					digit = '0' - digit; 
					outArr[i] = digit ; 

				}
				else 
				{
					digit = digit - '0'; 
					outArr[i] = digit;  
				}

			}
			//if it is not a register number it is a tag. 
			else 
			{
				int address = tagAddress( instruction[i] ); 
				outArr[i] = address; 
				
				//if it is a symbolic address calculate the
				//pc relative address
				if (outArr[0]  == 4) 
				{
					int pcoffset = address - (lineNumber +1); 
					outArr[i] = pcoffset;
				}
			}
			
		}
		i = i + 1; 
	}
}

//checks whether or not a char
//is a white space [ tabs, spaces ]
// tab = \t, 9  
// space = ' ' 32 
int isWhiteSpace ( char c ) 
{

	if( c == ' ' || c == '\t' || c == '\n'  ){ return 1; }	
	else { return 0; }
}


//takes in the oppcode field and 
//returns it's associated feild. 
int determineOppcode ( char oppcode[] ) 
{

	if( strcmp(oppcode, "add")   == 0)	{ return 0;	}
	if( strcmp(oppcode, "nand")  == 0) 	{ return 1;	}
	if( strcmp(oppcode, "lw")    == 0)	{ return 2;	}
	if( strcmp(oppcode, "sw")    == 0)	{ return 3;	}
	if( strcmp(oppcode, "beq")   == 0)	{ return 4;	}
	if( strcmp(oppcode, "jalr")  == 0)	{ return 5;	}
	if( strcmp(oppcode, "halt")  == 0)	{ return 6;	}
	if( strcmp(oppcode, "noop")  == 0)	{ return 7;	}
	if( strcmp(oppcode, ".fill") == 0)	{ return 8;	}
	return -1; 
	//printf("Specified oppcode: %s was not found\n", oppcode); 	
	//exit(1); 
}

//takes in an oppcode field an 
//returns it's associated field 
char determineInstructionType ( char oppcode[] )
{
	if (strcmp (oppcode, "add") == 0  || strcmp (oppcode, "nand") == 0 ){ return 'r'; } 	
	if (strcmp (oppcode, "lw") == 0  || strcmp (oppcode, "sw") == 0  
			|| strcmp ( oppcode, "beq" ) == 0  )  { return 'i'; } 
	if (strcmp (oppcode, "halt") == 0 || strcmp (oppcode, "noop") == 0){ return 'o'; }
	if (strcmp (oppcode, "jalr")  == 0) { return 'j'; } 
	if (strcmp (oppcode, ".fill") == 0) { return 'f'; } 
	return -1;
}

//takes int array
//and makes a call to the correct packer function 
//also does a bunch of error checking on the values while we are at  
int packIntoNumber( int instruct[], int lineNumber ) 
{
	int result; 
	int offset;


	//Do error checks on our input before we pack them into a result. 	
	

	//Check for correct oppcode. 	
	if( instruct[0] < 0) 
	{
		fprintf( stderr, "On lineNumber %d,	the oppcode was not recognized.\n", lineNumber); 
		exit(1); 
	}

	//check for correct register numbers. 
	if( instruct[0] < 6)
	{
		if( instruct[1] < 0 || instruct[1] > 7)
		{
			fprintf( stderr, "On lineNumber %d, the value for feild1 (%d) is not a vaild register number\n", lineNumber, instruct[1]);
			exit(1); 
		}
		if( instruct[2] < 0 || instruct[2] > 7)
		{
			fprintf( stderr, "On lineNumber %d, the value for field2 (%d) is not a vaild register number\n", lineNumber, instruct[2]);
			exit(1); 
		}

	}	


	switch( instruct[0] )
	{
		case 0: 
		case 1:
			// r types. 
			// oppcode, regDest, regA, regB -> 
			// oppcode, regA, regB, regDest

			//checkReg that regB is correct.
			if(instruct[3] < 0 || instruct[3] > 7)
			{
				fprintf( stderr, "On lineNumber %d, the value for field3 (%d) is not a vaild register number\n", lineNumber, instruct[3]);
				exit(1); 
			}
			result = packRtype( instruct[0], instruct[2], instruct[3], instruct[1] ); 
			break; 
		case 2:
		case 3: 
		case 4: 
			// i types: 
			// oppcode, regA, regB, offset -> 
			// oppcode, regA, regB, offset 
			offset  = instruct[3]; 

			if(offset > 32768 || offset < -32767) 
			{
				fprintf( stderr, "On line number %d; The offset value %d will not fit into a 16 bit two's compliment number. \n", lineNumber, offset);  	
				exit(1); 
			}	
			result = packItype( instruct[0], instruct[1], instruct[2], offset ); 
			break; 
		case 5:
			// j type 
			// oppcode, regA, regB, ->  
			// oppcode, regA, regB, 

			result = packJtype( instruct[0], instruct[1], instruct[2]); 
			break; 
		case 6: 
		case 7: 
			// o type
			// just oppcode 
			
			result = packOtype(instruct[0]); 	
			break; 
		case 8: 
			//.fill directive. 
			//return the number we are filling with. 
			return instruct[1];
			break; 
		default: 
			fprintf( stderr, "On lineNumber %d,	the oppcode was not recognized.\n", lineNumber); 
			exit(1); 
		return result; 

	}
}


int packRtype(int oppcode, int regA, int regB, int regDest)
{
	int result = 0;

	//clean result value, and then OR in our shifted value. 
	result = ( (result & 0x00000000) | (oppcode << 22) ); 
	result = ( (result & 0x01c00000) | (regA << 19)); 
	result = ( (result & 0x01f80000) | (regB << 16)); 
	result = ( (result & 0x1ff0000) | regDest); 
	
	return result; 

}
int packItype(int oppcode, int regA, int regB, int offset)
{
	int result = 0 ; 
	result = ( (result & 0x00000000) | (oppcode << 22) ); 
	result = ( (result & 0x01c00000) | (regA << 19)); 
	result = ( (result & 0x01f80000) | (regB << 16)); 

	result = ( (result & 0x1ff0000)  | 	(offset & 0x0000ffff ) ); 
}
int packJtype(int oppcode, int regA, int regB)
{
	int result = 0; 

	result = ( (result & 0x00000000) | (oppcode << 22) ); 
	result = ( (result & 0x01c00000) | (regA << 19)); 
	result = ( (result & 0x01f80000) | (regB << 16)); 
	return result; 
}
int packOtype( int oppcode)
{
	int result = 0; 
	result = ( (result & 0x00000000) | (oppcode << 22) ); 
	return result;
}
