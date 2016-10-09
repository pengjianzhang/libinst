/*counting_insn.h
	
*/

#ifndef COUNTING_INSN_H
#define COUNTING_INSN_H

#include "global.h"
#include "isa.h"
#include "basicblock.h"

void pjz_instrument_insn(INSN_CLASS i_class, ADDRESS start, UINT64 * counter);
void pjz_instrument_insn_2(INSN_CLASS i_class, struct bb * bb_p);
#endif
