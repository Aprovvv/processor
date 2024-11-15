#ifndef CMDS_H
#define CMDS_H
typedef long proc_elem_t;
const int LBL_SIZE = 32;
const int CMD_SIZE = 32;
const int PRECISION = 10000;
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
    SQRT,
    DRAW,
    CALL,
    RET,
    IN,
};
enum dests {
    mask_numb      = 0x001,
    mask_reg       = 0x002,
    mask_mem       = 0x004,
    mask_reg_mem   = 0x008,
    mask_plus      = 0x010,
    mask_minus     = 0x020,
    mask_backminus = 0x040,
    mask_star      = 0x080,
    mask_slash     = 0x100,
    mask_backslash = 0x200
};
enum reg_names {
    ax,
    bx,
    cx,
    dx,
};
#endif
/*
*/
