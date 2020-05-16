#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <string.h>
#include <jni.h>
#include <android/log.h>

#include "common.h"


/********************************************************
运行elf，
*********************************************************/


int main(int argc, char **argv)
{
	fprintf(stderr, "hello main");
	return 0;
}

