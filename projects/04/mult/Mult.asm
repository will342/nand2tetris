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

//initalize variables
@i
M = 0
@R2
M = 0

//check R1 is 0, if so end program
@R1
D = M
@End
D; JEQ

//check R0 is 0, if so end program
//load R0
@R0
D = M
@End
D; JEQ

//addition loop
(Loop)
@R0
D = M
@R2
M = M + D
@i
M = M + 1
D = M
@R1
D = M - D
@Loop
D; JGT 

(End)
@End
0;JMP



