/*stack.h
 *
 *manage stack
 */

#ifndef STACK_H
#define STACK_H

#include "global.h"


void pjz_init_stack();

// make push instructions, and store the instructions to CODE, from REG; return instruction nums
INSN_T pjz_make_push(REG_T reg);

//make pop instructions to REG, and store the instructions to CODE, return instruction nums
INSN_T pjz_make_pop(REG_T reg);

#endif
