/*
 $Id: handlers.c,v 1.12 2007-06-05 01:45:34+01 taviso Exp taviso $

 Copyright (C) 2006,2007,2009 Tavis Ormandy <taviso@sdf.lonestar.org>
 Copyright (C) 2009           Eli Dupree <elidupree@charter.net>
 Copyright (C) 2009,2010      WANG Lu <coolwanglu@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <assert.h>
#include <setjmp.h>
#include <alloca.h>
#include <strings.h>            /*lint -esym(526,strcasecmp) */
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>            /* to determine the word width */
#include <errno.h>
#include <inttypes.h>

#ifndef __ANDROID__
#include <readline/readline.h>
#endif

#include "commands.h"
#include "handlers.h"
//#include "interrupt.h"
#include "show_message.h"

#define USEPARAMS() ((void) vars, (void) argv, (void) argc)     /* macro to hide gcc unused warnings */

/*lint -esym(818, vars, argv) dont want want to declare these as const */

/*
 * This file defines all the command handlers used, each one is registered using
 * registercommand(), and when a matching command is entered, the commandline is
 * tokenized and parsed into an argv/argc.
 * 
 * argv[0] will contain the command entered, so one handler can handle multiple
 * commands by checking whats in there, but you still need seperate documentation
 * for each command when you register it.
 *
 * Most commands will also need some documentation, see handlers.h for the format.
 *
 * Commands are allowed to read and modify settings in the vars structure.
 *
 */

#define calloca(x,y) (memset(alloca((x) * (y)), 0x00, (x) * (y)))

bool handler__set(globals_t * vars, char **argv, unsigned argc)
{
   
    return true;
    
}

/* XXX: add yesno command to check if matches > 099999 */
/* FORMAT (don't change, front-end depends on this): 
 * [#no] addr, value, [possible types (separated by space)]
 */
bool handler__list(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

/* XXX: handle multiple deletes, eg delete !1 2 3 4 5 6 */
bool handler__delete(globals_t * vars, char **argv, unsigned argc)
{
	return true;
}

bool handler__reset(globals_t * vars, char **argv, unsigned argc)
{
   
    return true;
}

bool handler__pid(globals_t * vars, char **argv, unsigned argc)
{
	show_info("handler__pid.\n");
    return true;
}

bool handler__snapshot(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

/* dregion [!][x][,x,...] */
bool handler__dregion(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

bool handler__lregions(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

/* the name of the function is for history reason, now GREATERTHAN & LESSTHAN are also handled by this function */
bool handler__decinc(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

bool handler__version(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

bool handler__string(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

bool handler__default(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

bool handler__update(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

bool handler__exit(globals_t * vars, char **argv, unsigned argc)
{
	vars->exit = 1;
	show_info("handler__exit.\n");
    return true;
}

#define DOC_COLUMN  11           /* which column descriptions start on with help command */

bool handler__help(globals_t * vars, char **argv, unsigned argc)
{
	show_info("handler__help.\n");
    return true;
}

bool handler__eof(globals_t * vars, char **argv, unsigned argc)
{

	return true;
}

/* XXX: handle !ls style escapes */
bool handler__shell(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

bool handler__watch(globals_t * vars, char **argv, unsigned argc)
{
	return true;
}

bool handler__show(globals_t * vars, char **argv, unsigned argc)
{
   
    return true;
}

bool handler__dump(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

bool handler__write(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}

bool handler__option(globals_t * vars, char **argv, unsigned argc)
{
    
    return true;
}
