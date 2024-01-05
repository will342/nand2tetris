// R2 = max(R0, R1)
// if (R0 > R1) then R2 = R0
// else R2 = R1

@R0
D = M
@R1
D = D - M
@COND1
D;JGT

@R1
D = M
@R2
M = D
@END
0;JMP

(COND1)
@R0
D = M
@R2
M = D
@END
0;JMP

(END)
@END
0;JMP