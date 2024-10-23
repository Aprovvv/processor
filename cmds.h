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
    mask_numb    = 0x01,
    mask_reg     = 0x02,
    mask_mem     = 0x04,
    mask_reg_mem = 0x08,
    mask_plus    = 0x10,
    mask_minus   = 0x20,
    mask_star    = 0x40,
    mask_slash   = 0x80,
};
enum reg_names {
    ax,
    bx,
    cx,
    dx,
};
#endif
