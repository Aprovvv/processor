#ifndef CMDS_H
#define CMDS_H
const int LBL_SIZE = 32;
enum cmds {
    HLT,
    PUSH,
    POP,
    OUT,
    SUM,
    SUB,
    MULT,
    DIV,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JME,
    LBL,
};
enum dests {
    push = 1,
    pushr = 2,
    pushm = 3,
    pushmr = 7,
};
#endif
