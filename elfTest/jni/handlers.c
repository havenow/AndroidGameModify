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
#include "interrupt.h"
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
	unsigned block, seconds = 1;
	char *delay = NULL;
	char *end;
	bool cont = false;
	struct setting {
		char *matchids;
		char *value;
		unsigned seconds;
	} *settings = NULL;

	assert(argc != 0);
	assert(argv != NULL);
	assert(vars != NULL);


	if (argc < 2) {
		show_error("expected an argument, type `help set` for details.\n");
		return false;
	}

	/* supporting `set` for bytearray will cause annoying syntax problems... */
	if ((vars->options.scan_data_type == BYTEARRAY)
		|| (vars->options.scan_data_type == STRING))
	{
		show_error("`set` is not supported for bytearray or string, use `write` instead.\n");
		return false;
	}

	/* check if there are any matches */
	if (vars->num_matches == 0) {
		show_error("no matches are known.\n");
		return false;
	}

	/* --- parse arguments into settings structs --- */

	settings = calloca(argc - 1, sizeof(struct setting));

	/* parse every block into a settings struct */
	for (block = 0; block < argc - 1; block++) {

		/* first seperate the block into matches and value, which are separated by '=' */
		if ((settings[block].value = strchr(argv[block + 1], '=')) == NULL) {

			/* no '=' found, whole string must be the value */
			settings[block].value = argv[block + 1];
		}
		else {
			/* there is a '=', value+1 points to value string. */

			/* use strndupa() to copy the matchids into a new buffer */
			settings[block].matchids =
				strndupa(argv[block + 1],
				(size_t)(settings[block].value++ - argv[block + 1]));
		}

		/* value points to the value string, possibly with a delay suffix */

		/* matchids points to the match-ids (possibly multiple) or NULL */

		/* now check for a delay suffix (meaning continuous mode), eg 0xff/10 */
		if ((delay = strchr(settings[block].value, '/')) != NULL) {
			char *end = NULL;

			/* parse delay count */
			settings[block].seconds = strtoul(delay + 1, &end, 10);

			if (*(delay + 1) == '\0') {
				/* empty delay count, eg: 12=32/ */
				show_error("you specified an empty delay count, `%s`, see `help set`.\n", settings[block].value);
				return false;
			}
			else if (*end != '\0') {
				/* parse failed before end, probably trailing garbage, eg 34=9/16foo */
				show_error("trailing garbage after delay count, `%s`.\n", settings[block].value);
				return false;
			}
			else if (settings[block].seconds == 0) {
				/* 10=24/0 disables continous mode */
				show_info("you specified a zero delay, disabling continuous mode.\n");
			}
			else {
				/* valid delay count seen and understood */
				show_info("setting %s every %u seconds until interrupted...\n", settings[block].matchids ? settings[block].matchids : "all", settings[block].seconds);

				/* continuous mode on */
				cont = true;
			}

			/* remove any delay suffix from the value */
			settings[block].value =
				strndupa(settings[block].value,
				(size_t)(delay - settings[block].value));
		}                       /* if (strchr('/')) */
	}                           /* for(block...) */

								/* --- setup a longjmp to handle interrupt --- */
	if (INTERRUPTABLE()) {

		/* control returns here when interrupted */
		free(settings);
		detach(vars->target);
		ENDINTERRUPTABLE();
		return true;
	}

	/* --- execute the parsed setting structs --- */

	while (true) {
		uservalue_t userval;

		/* for every settings struct */
		for (block = 0; block < argc - 1; block++) {

			/* check if this block has anything to do this iteration */
			if (seconds != 1) {
				/* not the first iteration (all blocks get executed first iteration) */

				/* if settings.seconds is zero, then this block is only executed once */
				/* if seconds % settings.seconds is zero, then this block must be executed */
				if (settings[block].seconds == 0
					|| (seconds % settings[block].seconds) != 0)
					continue;
			}

			/* convert value */
			if (!parse_uservalue_number(settings[block].value, &userval)) {
				show_error("bad number `%s` provided\n", settings[block].value);
				goto fail;
			}

			/* check if specific match(s) were specified */
			if (settings[block].matchids != NULL) {
				char *id, *lmatches = NULL;
				unsigned num = 0;

				/* create local copy of the matchids for strtok() to modify */
				lmatches = strdupa(settings[block].matchids);

				/* now seperate each match, spearated by commas */
				while ((id = strtok(lmatches, ",")) != NULL) {
					match_location loc;

					/* set to NULL for strtok() */
					lmatches = NULL;

					/* parse this id */
					num = strtoul(id, &end, 0x00);

					/* check that succeeded */
					if (*id == '\0' || *end != '\0') {
						show_error("could not parse match id `%s`\n", id);
						goto fail;
					}

					/* check this is a valid match-id */
					loc = nth_match(vars->matches, num);
					if (loc.swath) {
						value_t v;
						value_t old;
						void *address = remote_address_of_nth_element(loc.swath, loc.index /* ,MATCHES_AND_VALUES */);

						/* copy val onto v */
						/* XXX: valcmp? make sure the sizes match */
						old = data_to_val(loc.swath, loc.index /* ,MATCHES_AND_VALUES */);
						v.flags = old.flags = loc.swath->data[loc.index].match_info;
						uservalue2value(&v, &userval);

						show_info("setting *%p to %#"PRIx64"...\n", address, v.int64_value);

						/* set the value specified */
						if (setaddr(vars->target, address, &v) == false) {
							show_error("failed to set a value.\n");
							goto fail;
						}

					}
					else {
						/* match-id > than number of matches */
						show_error("found an invalid match-id `%s`\n", id);
						goto fail;
					}
				}
			}
			else {

				matches_and_old_values_swath *reading_swath_index = (matches_and_old_values_swath *)vars->matches->swaths;
				int reading_iterator = 0;

				/* user wants to set all matches */
				while (reading_swath_index->first_byte_in_child) {

					/* Only actual matches are considered */
					if (flags_to_max_width_in_bytes(reading_swath_index->data[reading_iterator].match_info) > 0)
					{
						void *address = remote_address_of_nth_element(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */);

						/* XXX: as above : make sure the sizes match */

						value_t old = data_to_val(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */);
						value_t v;
						v.flags = old.flags = reading_swath_index->data[reading_iterator].match_info;
						uservalue2value(&v, &userval);

						show_info("setting *%p to %"PRIx64"...\n", address, v.int64_value);

						if (setaddr(vars->target, address, &v) == false) {
							show_error("failed to set a value.\n");
							goto fail;
						}
					}

					/* Go on to the next one... */
					++reading_iterator;
					if (reading_iterator >= reading_swath_index->number_of_bytes)
					{
						reading_swath_index = local_address_beyond_last_element(reading_swath_index /* ,MATCHES_AND_VALUES */);
						reading_iterator = 0;
					}
				}
			}                   /* if (matchid != NULL) else ... */
		}                       /* for(block) */

		if (cont) {
			sleep(1);
		}
		else {
			break;
		}

		seconds++;
	}                           /* while(true) */

	ENDINTERRUPTABLE();
	return true;

fail:
	ENDINTERRUPTABLE();
	return false;
}

#include <stdarg.h>
void appendMatchToLogFile(const char *format, ...)
{
	char content[1024];
	memset(content, 0, 1024);

	va_list args;
	va_start(args, format);
	vsprintf(content, format, args);
#if 1
	strcat(content, "\n");
#endif 	
	va_end(args);

	FILE* log_file = fopen("/data/local/tmp/match.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

/* XXX: add yesno command to check if matches > 099999 */
/* FORMAT (don't change, front-end depends on this): 
 * [#no] addr, value, [possible types (separated by space)]
 */
bool handler__list(globals_t * vars, char **argv, unsigned argc)
{
	unsigned i = 0;
	int buf_len = 128; /* will be realloc later if necessary */
	char *v = malloc(buf_len);
	if (v == NULL)
	{
		show_error("memory allocation failed.\n");
		return false;
	}
	char *bytearray_suffix = ", [bytearray]";
	char *string_suffix = ", [string]";

	USEPARAMS();

	if (!(vars->matches))
		return true;

	matches_and_old_values_swath *reading_swath_index = (matches_and_old_values_swath *)vars->matches->swaths;
	int reading_iterator = 0;

	/* list all known matches */
	while (reading_swath_index->first_byte_in_child) {

		match_flags flags = reading_swath_index->data[reading_iterator].match_info;

		//appendMatchToLogFile("### match number_of_bytes: %d", reading_swath_index->number_of_bytes);

		/* Only actual matches are considered */
		if (flags_to_max_width_in_bytes(flags) > 0)//add_element时是一个字节add的，如果超过一个字节 其他字节是zero_flag
			//old_value_and_match_info new_value = { get_u8b(&data_value), checkflags };
			//ld_value_and_match_info new_value = { get_u8b(&data_value), zero_flag };
		{
			switch (globals.options.scan_data_type)
			{
			case BYTEARRAY:
				; /* cheat gcc */
				buf_len = flags.bytearray_length * 3 + 32;
				v = realloc(v, buf_len); /* for each byte and the suffix', this should be enough */

				if (v == NULL)
				{
					show_error("memory allocation failed.\n");
					return false;
				}
				data_to_bytearray_text(v, buf_len, reading_swath_index, reading_iterator, flags.bytearray_length);
				assert(strlen(v) + strlen(bytearray_suffix) + 1 <= buf_len); /* or maybe realloc is better? */
				strcat(v, bytearray_suffix);
				break;
			case STRING:
				; /* cheat gcc */
				buf_len = flags.string_length + strlen(string_suffix) + 32; /* for the string and suffix, this should be enough */
				v = realloc(v, buf_len);
				if (v == NULL)
				{
					show_error("memory allocation failed.\n");
					return false;
				}
				data_to_printable_string(v, buf_len, reading_swath_index, reading_iterator, flags.string_length);
				assert(strlen(v) + strlen(string_suffix) + 1 <= buf_len); /* or maybe realloc is better? */
				strcat(v, string_suffix);
				break;
			default: /* numbers */
				; /* cheat gcc */
				value_t val = data_to_val(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */);//获取匹配的项 numbers
				truncval_to_flags(&val, flags);

				if (valtostr(&val, v, buf_len) != true) {
					strncpy(v, "unknown", buf_len);
				}
				break;
			}

			/* try to determine the size of a pointer */
#if ULONGMAX == 4294967295UL
#define POINTER_FMT "%10p"
#elif ULONGMAX == 18446744073709551615UL
#define POINTER_FMT "%20p"
#else
#define POINTER_FMT "%20p"
#endif

			fprintf(stdout, "[%2u] "POINTER_FMT", %s\n", i++, remote_address_of_nth_element(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */), v);
			//appendMatchToLogFile("[%2u] "POINTER_FMT", %s\n", i, remote_address_of_nth_element(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */), v);
		}

		/* Go on to the next one... */
		++reading_iterator;//搜索90时，reading_swath_index->number_of_bytes = 4，内存数据可能是 5A 00 5A 00，其实有两个90
		if (reading_iterator >= reading_swath_index->number_of_bytes)
		{
			assert(((matches_and_old_values_swath *)(local_address_beyond_last_element(reading_swath_index /* ,MATCHES_AND_VALUES */)))->number_of_bytes >= 0);
			reading_swath_index = local_address_beyond_last_element(reading_swath_index /* ,MATCHES_AND_VALUES */);
			reading_iterator = 0;
		}
	}

	free(v);
    return true;
}

/* XXX: handle multiple deletes, eg delete !1 2 3 4 5 6 */
bool handler__delete(globals_t * vars, char **argv, unsigned argc)
{
	return true;
}

bool handler__reset(globals_t * vars, char **argv, unsigned argc)
{
	show_info("handler__reset.\n");

	USEPARAMS();

	if (vars->matches) { free(vars->matches); vars->matches = NULL; vars->num_matches = 0; }

	/* refresh list of regions */
	l_destroy(vars->regions);

	/* create a new linked list of regions */
	if ((vars->regions = l_init()) == NULL) {
		show_error("sorry, there was a problem allocating memory.\n");
		return false;
	}

	/* read in maps if a pid is known */
	if (vars->target && readmaps(vars->target, vars->regions) != true) {
		show_error("sorry, there was a problem getting a list of regions to search.\n");
		show_warn("the pid may be invalid, or you don't have permission.\n");
		vars->target = 0;
		return false;
	}
   
    return true;
}

bool handler__pid(globals_t * vars, char **argv, unsigned argc)
{
	show_info("handler__pid.\n");
	char *resetargv[] = { "reset", NULL };
	char *end = NULL;

	if (argc == 2) {
		vars->target = (pid_t)strtoul(argv[1], &end, 0x00);

		if (vars->target == 0) {
			show_error("`%s` does not look like a valid pid.\n", argv[1]);
			return false;
		}
	}
	else if (vars->target) {
		/* print the pid of the target program */
		show_info("target pid is %u.\n", vars->target);
		return true;
	}
	else {
		show_info("no target is currently set.\n");
		return false;
	}

	return handler__reset(vars, resetargv, 1);
}

bool handler__snapshot(globals_t * vars, char **argv, unsigned argc)
{

    return true;
}

/* dregion [!][x][,x,...] */
bool handler__dregion(globals_t * vars, char **argv, unsigned argc)
{
	unsigned id;
	bool invert = false;
	char *end = NULL, *idstr = NULL, *block = NULL;
	element_t *np, *pp;
	list_t *keep = NULL;
	region_t *save;

	/* need an argument */
	if (argc < 2) {
		show_error("expected an argument, see `help dregion`.\n");
		return false;
	}

	/* check that there is a process known */
	if (vars->target == 0) {
		show_error("no target specified, see `help pid`\n");
		return false;
	}

	/* check for an inverted match */
	if (*argv[1] == '!') {
		invert = true;
		/* create a copy of the argument for strtok(), +1 to skip '!' */
		block = strdupa(argv[1] + 1);

		/* check for lone '!' */
		if (*block == '\0') {
			show_error("inverting an empty set, maybe try `reset` instead?\n");
			return false;
		}

		/* create a list to keep the specified regions */
		if ((keep = l_init()) == NULL) {
			show_error("memory allocation error.\n");
			return false;
		}

	}
	else {
		invert = false;
		block = strdupa(argv[1]);
	}

	/* loop for every number specified, eg "1,2,3,4,5" */
	while ((idstr = strtok(block, ",")) != NULL) {
		region_t *r = NULL;

		/* set block to NULL for strtok() */
		block = NULL;

		/* attempt to parse as a regionid */
		id = strtoul(idstr, &end, 0x00);

		/* check that worked, "1,abc,4,,5,6foo" */
		if (*end != '\0' || *idstr == '\0') {
			show_error("could not parse argument %s.\n", idstr);
			if (invert) {
				if (l_concat(vars->regions, &keep) == -1) {
					show_error("there was a problem restoring saved regions.\n");
					l_destroy(vars->regions);
					l_destroy(keep);
					return false;
				}
			}
			assert(keep == NULL);
			return false;
		}

		/* initialise list pointers */
		np = vars->regions->head;
		pp = NULL;

		/* find the correct region node */
		while (np) {
			r = np->data;

			/* compare the node id to the id the user specified */
			if (r->id == id)
				break;

			pp = np; /* keep track of prev for l_remove() */
			np = np->next;
		}

		/* check if a match was found */
		if (np == NULL) {
			show_error("no region matching %u, or already moved.\n", id);
			if (invert) {
				if (l_concat(vars->regions, &keep) == -1) {
					show_error("there was a problem restoring saved regions.\n");
					l_destroy(vars->regions);
					l_destroy(keep);
					return false;
				}
			}
			if (keep)
				l_destroy(keep);
			return false;
		}

		np = pp;

		/* save this region if the match is inverted */
		if (invert) {

			assert(keep != NULL);

			l_remove(vars->regions, np, (void *)&save);
			if (l_append(keep, keep->tail, save) == -1) {
				show_error("sorry, there was an internal memory error.\n");
				free(save);
				return false;
			}
			continue;
		}

		/* check for any affected matches before removing it */
		if (vars->num_matches > 0)
		{
			region_t *s;

			/* determine the correct pointer we're supposed to be checking */
			if (np) {
				assert(np->next);
				s = np->next->data;
			}
			else {
				/* head of list */
				s = vars->regions->head->data;
			}

			if (!(vars->matches = delete_by_region(vars->matches, &vars->num_matches, s, false)))
			{
				show_error("memory allocation error while deleting matches\n");
			}
		}

		l_remove(vars->regions, np, NULL);
	}

	if (invert) {
		region_t *s = keep->head->data;

		if (vars->num_matches > 0)
		{
			if (!(vars->matches = delete_by_region(vars->matches, &vars->num_matches, s, true)))
			{
				show_error("memory allocation error while deleting matches\n");
			}
		}

		/* okay, done with the regions list */
		l_destroy(vars->regions);

		/* and switch to the keep list */
		vars->regions = keep;
	}

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

//#include <sys/stat.h>
//#include <sys/ptrace.h>
//#include <sys/types.h>
//#include <sys/wait.h>
bool handler__default(globals_t * vars, char **argv, unsigned argc)
{
	show_info("handler__default.\n");
	//list_t *regions = vars->regions;
	//int nBlock = regions->size;
	//show_info("block %d\n", nBlock);

	//int8_t val;
	//val = argv[0];


	//int status;
	//if (ptrace(PTRACE_ATTACH, vars->target, NULL, NULL) == -1L)
	//{
	//	show_info("failed to attach\n");
	//}
	//if (waitpid(vars->target, &status, 0) == -1 || !WIFSTOPPED(status))
	//{
	//	show_info("there was an error waiting for the target to stop.\n");
	//}

	//element_t* n = vars->regions->head;
	//region_t *r;
	//void *address = NULL;
	//int count = 0;
	//while (n) {
	//	unsigned offset, nread = 0;
	//	r = n->data;
	//	nread = r->size;

	//	for (offset = 0; offset < nread; offset++) {
	//		int8_t data_value;

	//		memset(&data_value, 0x00, sizeof(data_value));
	//		address = r->start + offset;

	//		data_value = ptrace(PTRACE_PEEKDATA, vars->target, address, NULL);

	//		if (val == data_value)
	//		{
	//			//show_info("find %d at: %lx ", val, address);
	//			count++;
	//		}
	//	}

	//	n = n->next;
	//}
	//show_info("#########	find %d\n", count);
	//show_info("#########	detach\n");
	//ptrace(PTRACE_DETACH, vars->target, 1, 0);

	uservalue_t val;
	bool ret;
	bytearray_element_t *array = NULL;

	USEPARAMS();

	switch (vars->options.scan_data_type)
	{
	case ANYNUMBER:
	case ANYINTEGER:
	case ANYFLOAT:
	case INTEGER8:
	case INTEGER16:
	case INTEGER32:
	case INTEGER64:
	case FLOAT32:
	case FLOAT64:
		/* attempt to parse command as a number */
		if (argc != 1)
		{
			show_error("unknown command\n");
			ret = false;
			goto retl;
		}
		show_info("handler__default `%s`\n", argv[0]);
		if (!parse_uservalue_number(argv[0], &val)) {
			show_error("unable to parse command `%s`\n", argv[0]);
			ret = false;
			goto retl;
		}
		break;
	case BYTEARRAY:
		/* attempt to parse command as a bytearray */
		array = calloc(argc, sizeof(bytearray_element_t));

		if (array == NULL)
		{
			show_error("there's a memory allocation error.\n");
			ret = false;
			goto retl;
		}
		int i = 0;
		for (; i < argc; i++)
		{
			show_info("handler__default `%s`\n", argv[i]);
		}
		if (!parse_uservalue_bytearray(argv, argc, array, &val)) {
			show_error("unable to parse command `%s`\n", argv[0]);
			ret = false;
			goto retl;
		}
		break;
	case STRING:
		show_error("unable to parse command `%s`\nIf you want to scan for a string, use command `\"`.\n", argv[0]);
		ret = false;
		goto retl;
		break;
	default:
		assert(false);
		break;
	}

	/* need a pid for the rest of this to work */
	if (vars->target == 0) {
		ret = false;
		goto retl;
	}

	/* user has specified an exact value of the variable to find */
	if (vars->matches) {
		/* already know some matches */
		if (checkmatches(vars, MATCHEQUALTO, &val) != true) {
			show_error("failed to search target address space.\n");
			ret = false;
			goto retl;
		}
	}
	else {
		/* initial search */
		if (searchregions(vars, MATCHEQUALTO, &val) != true) {
			show_error("failed to search target address space.\n");
			ret = false;
			goto retl;
		}
	}

	/* check if we now know the only possible candidate */
	if (vars->num_matches == 1) {
		show_info("match identified, use \"set\" to modify value.\n");
		show_info("enter \"help\" for other commands.\n");
	}

	ret = true;

retl:
	if (array)
		free(array);

	return ret;
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
