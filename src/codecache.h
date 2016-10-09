/*codecache.h
 *
 *Manage of codecache
 */

#ifndef CODECACHE_H
#define	CODECACHE_H

#include "global.h"

void pjz_cc_init();

void pjz_cc_add_code(unsigned * code, unsigned len);


//get the top address of codecache
ADDRESS pjz_cc_get_top_address();

void pjz_cc_flush();

#endif
