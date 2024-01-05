// exercise 1
// D <- 2
@2
D=A

//reset
D = 0

//exercise 2
// D <- D + 23
@23
D = D + A

//reset
D = 0

//exercise 3
//RAM[100] <- RAM[200]
@200
D = M
@100
M = D

//reset
D = 0

//exercise 4
//RAM[3] <- RAM[3] - 15
@15
D = A
@3
M = M - D

//reset
D = 0

//exercise 5
//RAM[3] <- RAM[4] + 1
@4
D = M + 1
@3
M = D

//reset
D = 0

//exercise 6
// computes RAM[2] = RAM[0] + RAM[1] + 17
@0
D = M
@1
D = D + M
@17
D = D + A
@2
M = D

//reset
D = 0

//exercise 7
//if (D=0) goto 300
@300
D;JEQ

//exercise 8
//if (RAM[3] <100) goto 12
@3
D = M
@100
D = D - A
@12
D;JLT

//
//variables
//

//exercise 9
// sum = 0
@sum
M = 0

// x = 512
@512
D = A
@x
M = D

// n = n - 1
@n
M = M - 1

//sum = sum + x
@sum
M = D
@x
D = D + M
@sum
M = D

//
//pointers
//

//exercise 1
//Gets R1 = RAM[R0]
//Usage: Put some non-negative value in R0
//in other words, R0 will contain a memory address x
//go to that memory address, copy the value to R1

@R0
A = M
D = M
@R1
M = D

//
//array processing
//

//exercise1
////variable declarations
//int[] arr = new int[5];
//int sum = 0;
////...enters values into array
////sums up array elements
//for (int j = 0; j<5; j++){
//    sum = sum + arr[j]
//}
////increment each array elements
//for (int j = 0; j<5; j++){
//      arr[j] = arr[j] + 1    
//}

//sum = sum + arr[j]
@arr
D = M
@j
A = D + M
D = M
@sum
M = M + D
//...

//arr[j] = arr[j] + 1
@arr
D = M
@j
A = D + M
M = M + 1
//...
