#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>


#include <string.h>
#include <jni.h>
#include <android/log.h>

#include "common.h"
#include "scanmem.h"
#include "commands.h"
#include "handlers.h"
#include "menu.h"

#define PACKAGE_VERSION "0.13"
#define PACKAGE_BUGREPORT "http://code.google.com/p/scanmem/"
/********************************************************
运行elf：将elf文件拷贝到/data/local/tmp/目录下面，修改权限
./elfTest -p 100
./elfTest -v		打印版本之后直接返回
./elfTest -h		打印版本和帮助之后直接返回
./elfTest -b
./elfTest -d

下面是输入有参数有误时
./elfTest -p		进入default，打印版本和帮助之后直接返回
./elfTest			进入设置signal的sighandler 这一行
./elfTest 3			进入done
./elfTest f			进入done
*********************************************************/

globals_t globals = {
	0,                          /* exit flag */
	0,                          /* pid target */
};

void printversion(FILE *outfd)
{
	fprintf(outfd, "scanmem version %s\n"
		"Copyright (C) 2009,2010 Tavis Ormandy, Eli Dupree, WANG Lu\n"
		"Copyright (C) 2006-2009 Tavis Ormandy\n"
		"scanmem comes with ABSOLUTELY NO WARRANTY; for details type `show warranty'.\n"
		"This is free software, and you are welcome to redistribute it\n"
		"under certain conditions; type `show copying' for details.\n\n", PACKAGE_VERSION);
}

void sighandler(int n)
{
	fprintf(stderr, "\nKilled by signal %d.\n", n);

	exit(EXIT_FAILURE);
}

void printhelp()
{
	printversion(stderr);

	fprintf(stderr, "Usage: scanmem [OPTION]... [PID]\n"
		"Interactively locate and modify variables in an executing process.\n"
		"\n"
		"-p, --pid=pid\t\tset the target process pid\n"
		"-b, --backend\t\trun as backend, used by frontend\n"
		"-h, --help\t\tprint this message\n"
		"-v, --version\t\tprint version information\n"
		"\n"
		"scanmem is an interactive debugging utility, enter `help` at the prompt\n"
		"for further assistance.\n"
		"\n" "Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
	return;
}

int main(int argc, char **argv)
{
	char *end;
	int optindex, ret = EXIT_SUCCESS;
	globals_t *vars = &globals;
	struct option longopts[] = {
		{ "pid", 1, NULL, 'p' },  /* target pid */
		{ "version", 0, NULL, 'v' },      /* print version */
		{ "help", 0, NULL, 'h' }, /* print help summary */
		{ "backend", 0, NULL, 'b' }, /* run as backend */
		{ "debug", 0, NULL, 'd' }, /* enable debug mode */
		{ NULL, 0, NULL, 0 },
	};

	while (true) {
		switch (getopt_long(argc, argv, "vhbdp:", longopts, &optindex)) {
		case 'p':
			fprintf(stderr, "argv p\n");
			vars->target = (pid_t)strtoul(optarg, &end, 0);

			/* check if that parsed correctly */
			if (*end != '\0' || *optarg == '\0' || vars->target == 0) {
				fprintf(stderr, "invalid pid specified.\n");
				return EXIT_FAILURE;
			}
			break;
		case 'v':
			printversion(stderr);
			return EXIT_SUCCESS;
		case 'h':
			printhelp();
			return EXIT_SUCCESS;
		case 'b':
			vars->options.backend = 1;
			break;
		case 'd':
			vars->options.debug = 1;
			break;
		case -1://不是"vhbdp:"这几个参数时；注意p后面是带参数的
			goto done;
		default:
			fprintf(stderr, "argv default\n");
			printhelp();
			return EXIT_FAILURE;
		}
	}

done:

	/* parse any pid specified after arguments */
	if (optind <= argc && argv[optind]) {
		fprintf(stderr, "done\n");
		vars->target = (pid_t)strtoul(argv[optind], &end, 0);

		/* check if that parsed correctly */
		if (*end != '\0' || argv[optind][0] == '\0' || vars->target == 0) {
			fprintf(stderr, "invalid pid specified.\n");
			return EXIT_FAILURE;
		}
	}//done 结束

/*********************************************************************************************/

	if (1) //设置signal的sighandler
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
		ret = EXIT_FAILURE;
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


	/* main loop, read input and process commands */
	while (!vars->exit) {
		char *line;

		/* reads in a commandline from the user, and returns a pointer to it in *line */
		if (getcommand(vars, &line) == false) {
			fprintf(stderr, "failed to read in a command.\n");
			ret = EXIT_FAILURE;
			break;
		}

		/* execcommand returning failure isnt fatal, just the a command couldnt complete. */
		if (execcommand(vars, line) == false) {
			if (vars->target == 0) {
				fprintf(stderr, "Enter the pid of the process to search using the \"pid\" command.\n");
				fprintf(stderr, "Enter \"help\" for other commands.\n");
			}
			else {
				fprintf(stderr, "Please enter current value, or \"help\" for other commands.\n");
			}
		}

		free(line);
	}

	fprintf(stderr, "main exit 0");
	return 0;
}

