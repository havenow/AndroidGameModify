/*

 $Id: scanmem.h,v 1.21 2007-06-05 01:45:35+01 taviso Exp taviso $

*/

#ifndef _SCANMEM_INC
#define _SCANMEM_INC            /* include guard */

#ifdef __cplusplus
extern "C"
{
#endif

/*lint +libh(config.h) */
#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>          /*lint !e537 */
#include <stdbool.h>

#include "scanroutines.h"
#include "list.h"
#include "target_memory_info_array.h"

#ifndef PACKAGE_VERSION
# define  PACKAGE_VERSION "(unknown)"
#endif



/* global settings */
typedef struct {
	unsigned exit : 1;
	pid_t target;
	matches_and_old_values_array *matches;
	long num_matches;
	list_t *regions;
	list_t *commands;      /* command handlers */
	const char *current_cmdline; /* the command being executed */
	struct {
		unsigned short debug;
		unsigned short backend; /* if 1, scanmem will work as a backend, and output would be more machine-readable */
		
		/* options that can be changed during runtime */
		scan_data_type_t scan_data_type;
		region_scan_level_t region_scan_level;
	} options;
} globals_t;


/* global settings */
extern globals_t globals;

bool sm_init(void);
void sm_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
