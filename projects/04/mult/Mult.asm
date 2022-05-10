// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

(START)
	@i
	M = 1 	// i = i
	
	@2
	M = 0 	//initlize product as 0
	
(LOOP)
	@i
	D = M 		// D = i
	@1
	D = D - M 	// D = i - value b
	@END
	D;JGT		//if i - b > 0 goto END
	@0
	D = M  		// D = a
	@2
	M = D + M	//sum a
	@i
	M = M + 1 	//i++
	@LOOP
	0;JMP
	
(END)
	@END
	0;JMP
	
	
	
	


