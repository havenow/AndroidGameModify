/*
 $Id: menu.c,v 1.15 2007-04-11 10:43:27+01 taviso Exp taviso $

 Copyright (C) 2006,2007,2009 Tavis Ormandy <taviso@sdf.lonestar.org>

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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#ifndef __ANDROID__
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "scanmem.h"
#include "commands.h"
#include "show_message.h"


/*
 * getcommand() reads in a command using readline, and places a pointer to the
 * read string into *line, _which must be free'd by caller_.
 * returns true on success, or false on error.
 */

bool getcommand(globals_t * vars, char **line)
{
    char prompt[64];
    bool success = true;

    assert(vars != NULL);

    snprintf(prompt, sizeof(prompt), "%ld> ", vars->num_matches);

#ifndef __ANDROID__
    rl_readline_name = "scanmem";
    rl_attempted_completion_function = commandcompletion;
#endif

    while (true) {
        if (vars->options.backend == 0)
        {
            /* for normal user, read in the next command using readline library */
#ifndef __ANDROID__
            success = ((*line = readline(prompt)) != NULL);
#else
	    return false;
#endif
        }
        else 
        {
            /* disable readline for front-end, since readline may produce ansi escape codes, which is terrible for front-end */
            printf("%s\n", prompt); /* add a newline for front-end */
            fflush(stdout); /* otherwise front-end may not receive this */
            *line = NULL; /* let getline malloc it */
            size_t n;
            ssize_t bytes_read = getline(line, &n, stdin);
            success = (bytes_read > 0);
            if (success)
                (*line)[bytes_read-1] = '\0'; /* remove the trialing newline */
        }
        if (!success) {
            /* EOF */
            if ((*line = strdup("__eof")) == NULL) {
                fprintf(stderr,
                        "error: sorry, there was a memory allocation error.\n");
                return false;
            }
        }

        if (strlen(*line)) {
            break;
        }

        free(*line);
    }

#ifndef __ANDROID__
    /* record this line to readline history */
    add_history(*line);
#endif
    return true;
}


