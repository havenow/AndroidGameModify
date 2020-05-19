/*
 $Id: commands.h,v 1.5 2007-04-11 10:43:26+01 taviso Exp $
*/

#ifndef _COMMANDS_INC
#define _COMMANDS_INC           /* include guard */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#include "scanmem.h"

typedef bool(*handler_ptr)(globals_t *vars, char **argv, unsigned argc);

/*lint -esym(534,registercommand) okay to ignore return value */

typedef struct {
    bool(*handler) (globals_t * vars, char **argv, unsigned argc);
    char *command;
    char *shortdoc;
    char *longdoc;
} command_t;


bool registercommand(const char *command, handler_ptr handler, list_t * commands,
                     char *shortdoc, char *longdoc);
bool execcommand(globals_t * vars, const char *commandline);

#ifdef __cplusplus
}
#endif

#endif
