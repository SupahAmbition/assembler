# ocamlAssembler
This is a little thing I hacked together over a weekend.

Inspired by a school project, I decided to see if I could implement
an assembler with a functional langauge, and see how different that
proramming experiance would be compared to implemening something like
in a c style imperative language. 


## Makfile
all: -> complies the assembler program with the correct 
		modules and libraries to produce an bytecode executable.  
test: -> compiles the assembler program and runs each test file on it. 
		 Does not check that the test programs are giving correct output.  
clean -> clean the directory of all complied files and whatnot.   

##assembler
The compiled program that runs the assembler. 
I used an ocmal version of get opt to parse command line arguments. 

The two options are:  
	-i inputFile: specifies an assembly file to read in.  
	-o outputFile: specified an output file to write to.   
				   If you don't specify a file to write to, 
				   the program will just print to stdout. 

### Example 
	./assembler -i testcases/test1.asm 
	./assembler -i testcases/test2.asm -o testcases/test2.mc


## assembler.ml 
The ocaml source code file. 
Works pretty well, but there are always improvements to be made. 

todo:  
0. Implement tags / tag replacement. 
1. Implement error checking, throw execptions on bad input. 
2. Implement multithreading. 


## testcases
test1 -> basic single line tests for each oppcode.  
test2 -> test relative addressing.  
test3 -> testing tag implementation.   
test4 -> testing tag relative addressing.   
