#ifndef _pre_defs_H
#define _pre_defs_H

#include <float.h>

//*********************************************************************************
//#define inst_del

#define largesttype long long
#define largesttype_un unsigned long long

#define sizetype size_t
#define dimtype size_t

#define numclustype size_t

//*********************************************************************************
#define dis_is_double

#ifdef dis_is_double
#define distype double
#define dismax DBL_MAX
#else
#define distype float
#define dismax FLT_MAX
#endif

//*********************************************************************************
#define coords_are_double

#ifdef coords_are_double
#define coordtype double
#define coordmax DBL_MAX
#else
#define coordtype float
#define coordmax FLT_MAX
#endif

#endif
