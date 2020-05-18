#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "scanmem.h"
#include "commands.h"
#include "handlers.h"
#include "show_message.h"

globals_t globals = {
	0,                          /* exit flag */
    0,                          /* pid target */
    0,                          /* match count */
    NULL,                       /* commands */
    NULL,                       /* current_cmdline */
    /* options */
    {
        0,                      /* debug */
        0,                      /* backend */
    }
};

void sighandler(int n)
{
	const char err_msg[] = "error: \nKilled by signal ";
    const char msg_end[] = ".\n";
    char num_str[4] = {0};
    ssize_t num_size;
    ssize_t wbytes;

    wbytes = write(STDERR_FILENO, err_msg, sizeof(err_msg) - 1);
    if (wbytes != sizeof(err_msg) - 1)
        goto out;
    /* manual int to str conversion */
    if (n < 10) {
        num_str[0] = (char) (0x30 + n);
        num_size = 1;
    } else if (n >= 100) {
        goto out;
    } else {
        num_str[0] = (char) (0x30 + n / 10);
        num_str[1] = (char) (0x30 + n % 10);
        num_size = 2;
    }
    wbytes = write(STDERR_FILENO, num_str, num_size);
    if (wbytes != num_size)
        goto out;
    wbytes = write(STDERR_FILENO, msg_end, sizeof(msg_end) - 1);
    if (wbytes != sizeof(msg_end) - 1)
        goto out;
out:
    _exit(EXIT_FAILURE);   /* also detaches from tracee */
}

bool sm_init(void)
{
    globals_t *vars = &globals;

 	if (1) //ÉèÖÃsignalµÄsighandler
	{
		fprintf(stderr, "let it crash and see the core dump.\n");
		(void)signal(SIGHUP, sighandler);
		(void)signal(SIGINT, sighandler);
		(void)signal(SIGSEGV, sighandler);
		(void)signal(SIGABRT, sighandler);
		(void)signal(SIGILL, sighandler);
		(void)signal(SIGFPE, sighandler);
		(void)signal(SIGTERM, sighandler);
	}

    if ((vars->commands = l_init()) == NULL) {
		fprintf(stderr, "sorry, there was a memory allocation error.\n");
		return false;
	}

	/* NULL shortdoc means dont display this command in `help` listing */
	registercommand("set",		(void*)handler__set,		vars->commands, SET_SHRTDOC,			SET_LONGDOC);
	registercommand("list",		(void*)handler__list,		vars->commands, LIST_SHRTDOC,			LIST_LONGDOC);
	registercommand("delete",	(void*)handler__delete,		vars->commands, DELETE_SHRTDOC,			DELETE_LONGDOC);
	registercommand("reset",	(void*)handler__reset,		vars->commands, RESET_SHRTDOC,			RESET_LONGDOC);
	registercommand("pid",		(void*)handler__pid,		vars->commands, PID_SHRTDOC,			PID_LONGDOC);
	registercommand("snapshot",	(void*)handler__snapshot,	vars->commands, SNAPSHOT_SHRTDOC,		SNAPSHOT_LONGDOC);
	registercommand("dregion",	(void*)handler__dregion,	vars->commands, DREGION_SHRTDOC,		DREGION_LONGDOC);
	registercommand("dregions",	(void*)handler__dregion,	vars->commands, NULL,					DREGION_LONGDOC);
	registercommand("lregions",	(void*)handler__lregions,	vars->commands, LREGIONS_SHRTDOC,		LREGIONS_LONGDOC);
	registercommand("version",	(void*)handler__version,	vars->commands, VERSION_SHRTDOC,		VERSION_LONGDOC);
	registercommand("=",		(void*)handler__decinc,		vars->commands, NOTCHANGED_SHRTDOC,		NOTCHANGED_LONGDOC);
	registercommand("!=",		(void*)handler__decinc,		vars->commands, CHANGED_SHRTDOC,		CHANGED_LONGDOC);
	registercommand("<",		(void*)handler__decinc,		vars->commands, LESSTHAN_SHRTDOC,		LESSTHAN_LONGDOC);
	registercommand(">",		(void*)handler__decinc,		vars->commands, GREATERTHAN_SHRTDOC,	GREATERTHAN_LONGDOC);
	registercommand("+",		(void*)handler__decinc,		vars->commands, INCREASED_SHRTDOC,		INCREASED_LONGDOC);
	registercommand("-",		(void*)handler__decinc,		vars->commands, DECREASED_SHRTDOC,		DECREASED_LONGDOC);
	registercommand("\"",		(void*)handler__string,		vars->commands, STRING_SHRTDOC,			STRING_LONGDOC);
	registercommand("update",	(void*)handler__update,		vars->commands, UPDATE_SHRTDOC,			UPDATE_LONGDOC);
	registercommand("exit",		(void*)handler__exit,		vars->commands, EXIT_SHRTDOC,			EXIT_LONGDOC);
	registercommand("quit",		(void*)handler__exit,		vars->commands, NULL,					EXIT_LONGDOC);
	registercommand("q",		(void*)handler__exit,		vars->commands, NULL,					EXIT_LONGDOC);
	registercommand("help",		(void*)handler__help,		vars->commands, HELP_SHRTDOC,			HELP_LONGDOC);
	registercommand("shell",	(void*)handler__shell,		vars->commands, SHELL_SHRTDOC,			SHELL_LONGDOC);
	registercommand("watch",	(void*)handler__watch,		vars->commands, WATCH_SHRTDOC,			WATCH_LONGDOC);
	registercommand("show",		(void*)handler__show,		vars->commands, SHOW_SHRTDOC,			SHOW_LONGDOC);
	registercommand("dump",		(void*)handler__dump,		vars->commands, DUMP_SHRTDOC,			DUMP_LONGDOC);
	registercommand("write",	(void*)handler__write,		vars->commands, WRITE_SHRTDOC,			WRITE_LONGDOC);
	registercommand("option",	(void*)handler__option,		vars->commands, OPTION_SHRTDOC,			OPTION_LONGDOC);
	registercommand("__eof",	(void*)handler__eof,		vars->commands, NULL,					NULL);
	registercommand(NULL,		(void*)handler__default,	vars->commands, DEFAULT_SHRTDOC,		DEFAULT_LONGDOC);

    return true;
}