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

#include "list.h"


#ifndef PACKAGE_VERSION
# define  PACKAGE_VERSION "(unknown)"
#endif



/* global settings */
typedef struct {
	unsigned exit : 1;
	pid_t target;
	list_t *commands;      /* command handlers */
	const char *current_cmdline; /* the command being executed */
	struct {
		unsigned short debug;
		unsigned short backend; /* if 1, scanmem will work as a backend, and output would be more machine-readable */

	} options;
} globals_t;


/* global settings */
extern globals_t globals;

#ifdef __cplusplus
}
#endif

#endif
