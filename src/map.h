#ifndef MAP_H
#define MAP_H
#include "global.h"

void * pjz_get_space_in_range(ADDRESS start,ADDRESS end, unsigned size);

void * pjz_get_space_at_address(ADDRESS start,unsigned size);


void * pjz_get_space_in_any_place(unsigned size);
#endif
