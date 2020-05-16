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

#define PACKAGE_VERSION "0.13"
#define PACKAGE_BUGREPORT "http://code.google.com/p/scanmem/"
/********************************************************
运行elf，
*********************************************************/
typedef struct {
	unsigned exit : 1;
	pid_t target;
} globals_t;

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
			//vars->options.backend = 1;
			break;
		case 'd':
			//vars->options.debug = 1;
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

	if (1) 
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

	fprintf(stderr, "main exit 0");
	return 0;
}

