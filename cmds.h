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
    mask_numb = 1,
    mask_reg = 2,
    mask_mem = 4,
    mask_reg_mem = 8,
    mask_plus = 16,
    mask_minus = 32,
    mask_star = 64,
    mask_slash = 128,
};
enum reg_names {
    ax,
    bx,
    cx,
    dx,
};
#endif
