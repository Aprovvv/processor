#ifndef CMDS_H
#define CMDS_H
typedef long proc_elem_t;
const int LBL_SIZE = 32;
const int CMD_SIZE = 32;
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
    pushm = 4,
    pushmr = 8,
};
enum reg_names {
    ax,
    bx,
    cx,
    dx,
};
#endif
