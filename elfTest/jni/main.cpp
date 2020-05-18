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


void printversion(FILE *outfd)
{
	fprintf(outfd, "scanmem version %s\n"
		"Copyright (C) 2009,2010 Tavis Ormandy, Eli Dupree, WANG Lu\n"
		"Copyright (C) 2006-2009 Tavis Ormandy\n"
		"scanmem comes with ABSOLUTELY NO WARRANTY; for details type `show warranty'.\n"
		"This is free software, and you are welcome to redistribute it\n"
		"under certain conditions; type `show copying' for details.\n\n", PACKAGE_VERSION);
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

static void parse_parameters(int argc, char **argv, char **initial_commands, bool *exit_on_error)
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
				exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			printversion(stderr);
			exit(EXIT_FAILURE);
		case 'h':
			printhelp();
			exit(EXIT_FAILURE);
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
			exit(EXIT_FAILURE);
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
			exit(EXIT_FAILURE);
		}
	}//done 结束
}

int main(int argc, char **argv)
{
	char *initial_commands = NULL;
	bool exit_on_error = false;
	parse_parameters(argc, argv, &initial_commands, &exit_on_error);

	int ret = EXIT_SUCCESS;
	globals_t *vars = &globals;
/*********************************************************************************************/

	if (!sm_init()) {
		fprintf(stderr, "Initialization failed.\n");
		ret = EXIT_FAILURE;
		goto end;
	}


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

end:
	fprintf(stderr, "main exit");
	l_destroy(vars->commands);
	return ret;
}

