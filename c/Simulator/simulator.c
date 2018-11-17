#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXMEMORY 65536
#define NUMREGS   8


FILE *inFile;
int numCycles;

	
typedef struct state_struct  {
	int pc;
	int reg[NUMREGS];
	int mem[MAXMEMORY];
	int num_memory;

} statetype;

void print_state(statetype *stateptr);
void print_stats(int numCycles, int n_instrs);
void mainRoutine( int machineCode, statetype *stateptr, int numInstructs );	
int determineOppcode( int machineCode );
int convert_num(int num);

	
void add(int machineCode, statetype *stateptr); 
void nand(int machineCode, statetype *stateptr);
void lw(int machineCode, statetype *stateptr);
void sw(int machineCode, statetype *stateptr);
void beq(int machineCode, statetype *stateptr);
void jalr(int machineCode, statetype *stateptr);



int main( int argc, char *argv[] )
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

		 	default: 
				 //printf("%c is not an option", opt); 
				 break; 
		}

	}// end of GNU getopt
	
	//read in the machine code and store into memory.	

	statetype state; 

	int linecount = 0; 
	char line2[100]; 

	rewind(inFile);
	while( fgets(line2, 100, inFile) != NULL)
	{
		state.mem[linecount] = atoi( line2 ); 
		linecount = linecount + 1; 
	}
	state.num_memory = linecount;

	numCycles = 0; 
	int numInstructions = 0; 
	while( state.pc <= linecount ) 
	{
		print_state( &state ); 
		state.pc = state.pc + 1; 
		numInstructions = numInstructions + 1;  
		mainRoutine( state.mem[state.pc -1], &state,  numInstructions);
	}

	return 0; 
}


void mainRoutine( int machineCode, statetype *stateptr, int numInstructions)	
{
	int oppcode = determineOppcode( machineCode ); 

	switch(oppcode)
	{
		case 0: 
			numCycles = numCycles + 2; 
			add( machineCode,  stateptr);
			break;	
		case 1:
			numCycles = numCycles + 2; 			
			nand( machineCode, stateptr);
			break; 
		case 2:
			numCycles = numCycles + 5; 
			lw( machineCode,   stateptr ); 
			break; 
		case 3: 
			numCycles = numCycles + 5;  
			sw( machineCode,   stateptr ); 
			break; 
		case 4:
			numCycles = numCycles + 3;
			beq( machineCode,  stateptr); 	
			break; 
		case 5:
			numCycles = numCycles + 1; 
			jalr( machineCode , stateptr);
			break; 
		case 6: 
			//halt 	
			numCycles = numCycles + 1; 

			printf("machine halted\n");
			print_stats(numCycles, numInstructions);
			exit(0);
			break;
		case 7: 
			//noop
			numCycles = numCycles + 1; 
			//pc counter already incremented. 
			break; 
	}
}

int determineOppcode( int machineCode )
{
	int oppcode; 
	oppcode = machineCode & 0x01c00000; 
	oppcode = oppcode >> 22;
	return oppcode;
}

void add (int machineCode, statetype *stateptr)
{

	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;
	int destReg = machineCode & 0x00000007;

	regA = stateptr -> reg[regA];
	regB = stateptr -> reg[regB];


	//now store the results of the values of regA + regB
	stateptr -> reg[destReg] = regA + regB;
	
}
void nand(int machineCode, statetype *stateptr)
{
	//take the values of regA and regB and apply a NAND. 
	//Store the result in destReg. 
	int result = 0; 
	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;
	int destReg = machineCode & 0x00000007;

	regA = stateptr -> reg[regA];
	regB = stateptr -> reg[regB];

	result = ~ ( regA & regB );
	stateptr -> reg[destReg] = result; 
}
void lw(int machineCode, statetype *stateptr)
{
	//take the value at the memory location 
	//regB + offset and store it into regA. 
	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;
	int offset = machineCode & 0x0000ffff;

	//only need the value from regB
	regB = stateptr -> reg[regB];

	int memoryAddress = regB + offset; 
	int memoryValue  = stateptr -> mem[memoryAddress];

	stateptr -> reg[regA] = memoryValue; 
}
void sw(int machineCode, statetype *stateptr)
{
	//take regA and store it at the address of 
	// value of regB + offset
	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;
	int offset = machineCode & 0x0000ffff;

	regA = stateptr -> reg[regA];
	
	int memoryAddress = regB + offset; 

	if( (memoryAddress + 1) > (stateptr -> num_memory) )
	{
		stateptr -> num_memory = memoryAddress + 1;
	}
	
	stateptr -> mem[memoryAddress] = regA; 
}
void beq(int machineCode, statetype *stateptr)
{
	int result; 
	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;
	int offset = machineCode & 0x0000ffff;
	
	regA = stateptr -> reg[regA];
	regB = stateptr -> reg[regB];
	result = regA - regB;

	offset = convert_num(offset);	
	//if the two numbers are the same then jump to the
	//address of pc + 1 + the offset field. 
	//otherwise do nothing. 
	if(result == 0) { stateptr -> pc = stateptr->pc + offset; }
}
void jalr(int machineCode, statetype *stateptr)
{
	int regA = (machineCode & 0x00380000) >> 19;
	int regB = (machineCode & 0x00070000) >> 16;

	regB = stateptr -> reg[regB]; 
	
	//store PC +1 into regA. 
	stateptr -> reg[regA] = stateptr->pc; 
	
	//set the PC to the value from regB.  
	stateptr -> pc = regB; 
}
int convert_num(int num){
	if (num & (1<<15) ) {
	num -= (1<<16);
	}
	return(num);
}

void print_state(statetype *stateptr){
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");
	
	for(i = 0; i < stateptr-> num_memory; i++){
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}
	printf("\tregisters:\n");
	
	for(i = 0; i < NUMREGS; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
	printf("end state\n");
}

void print_stats(int n_cycles, int n_instrs)
{
	printf("CYCLES: %d\n", n_cycles);
	printf("INSTRUCTIONS: %d\n", n_instrs); // total executed instructions
}
