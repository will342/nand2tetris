// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

@row
M = 0

(loop)
@KBD
D = M
@empty
D;JEQ

//check if row needs to stop going up
@8191
D = A
@row
D = D - M
@stop
D;JLE

(fill)
@row
D = M
@SCREEN
A = A + D
M = -1
@row
M = M + 1
@loop
0;JMP

(stop)
@24575
M = -1
D = A
@8191
D = A
@row
M = D
@loop
0;JMP

(stop2)
@row
M = 0
@loop
0;JMP

(empty)
@row
D = M
@stop2
D;JEQ
@row
M = M - 1
D = M
@SCREEN
A = A + D
M = 0
@loop
0;JMP
