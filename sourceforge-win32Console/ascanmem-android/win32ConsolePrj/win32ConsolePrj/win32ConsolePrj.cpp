// win32ConsolePrj.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "target_memory_info_array.h"
#include "scanroutines.h"
#include "show_message.h"
#include "value.h"

#include <stdarg.h>
void dumpMem(long data)
{
	FILE* log_file = fopen("/data/local/tmp/memRegion", "a+");
	if (log_file != NULL) {
		fwrite(&data, 1, 4, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}


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

	FILE* log_file = fopen("./match.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

typedef struct {
	unsigned exit : 1;
	unsigned int target;
	matches_and_old_values_array *matches;
	long num_matches;
	list_t *regions;
	list_t *commands;      /* command handlers */
	const char *current_cmdline; /* the command being executed */
	struct {
		unsigned short alignment;
		unsigned short debug;
		unsigned short backend; /* if 1, scanmem will work as a backend, and output would be more machine-readable */

								/* options that can be changed during runtime */
		scan_data_type_t scan_data_type;
		region_scan_level_t region_scan_level;
		unsigned short detect_reverse_change;
		unsigned short dump_with_ascii;
	} options;
} globals_t;

globals_t globals = {
	0,                          /* exit flag */
	0,                          /* pid target */
	NULL,                       /* matches */
	0,                          /* match count */
	NULL,                       /* regions */
	NULL,                       /* commands */
	NULL,                       /* current_cmdline */
								/* options */
	{
		1,                      /* alignment */
		0,                      /* debug */
		0,                      /* backend */
		ANYINTEGER,             /* scan_data_type */
		REGION_HEAP_STACK_EXECUTABLE_BSS, /* region_detail_level */
		0,                      /* detect_reverse_change */
		1,                      /* dump_with_ascii */
	}
};

# define EXPECT(x,y) x


int main()
{
	matches_and_old_values_swath *writing_swath_index;
	int required_extra_bytes_to_record = 0;
	region_t *r;
	void *address = NULL;

	scan_match_type_t match_type = MATCHEQUALTO;
	scan_data_type_t scan_data_type = ANYINTEGER;

	match_flags zero_flag;
	memset(&zero_flag, 0, sizeof(zero_flag));

	if (choose_scanroutine(scan_data_type, match_type) == false)
	{
		show_error("unsupported scan for current data type.\n");
		return false;
	}

	assert(g_scan_routine);

	unsigned offset, nread = 0;
	nread = 0x97000000 - 0x955dd000;//1A23000	27406336
	void *start = (void *)0x955dd000;
	uservalue_t uservalue;
	
	unsigned char uip_buf1[] = { 0x5A, 0x00, 0x00, 0x00 };//内存中的十六进制数据
	int* resultInt = (int*)(&(uip_buf1));//90

	char* search_data = "90";
	if (!parse_uservalue_number(search_data, &uservalue))//判断可能的数据类型 设置flag
	{
		show_error("unable to parse command `%s`\n", search_data);
	}

	if (!(globals.matches = allocate_array(globals.matches, nread)))
	{
		show_error("could not allocate match array\n");
		return 1;
	}

	writing_swath_index = (matches_and_old_values_swath *)globals.matches->swaths;

	writing_swath_index->first_byte_in_child = NULL;
	writing_swath_index->number_of_bytes = 0;


	FILE* memfile = fopen("../../com.and.games505.TerrariaPaid-955dd000-97000000.bin", "rb");
	if (memfile == NULL) 
		return 1;

	int64_t data;

	int nMatchSize = 8;
	for (offset = 0; offset < nread - 7; offset++)
	{
		match_flags checkflags;
		value_t data_value;

		memset(&data_value, 0x00, sizeof(data_value));
		valnowidth(&data_value);//data_value flag 设置为1

		address = (int8_t*)0x0000 + offset;

		fread(&(data_value.int64_value), 1, 8, memfile);
		fseek(memfile, -7, SEEK_CUR);

		memset(&checkflags, 0, sizeof(checkflags));

		int match_length;
		/* check if we have a match */
		if (EXPECT(((match_length = (*g_scan_routine)(&data_value, NULL, &uservalue, &checkflags, address)) > 0), false)) 
		{
			old_value_and_match_info new_value = { get_u8b(&data_value), checkflags };
			writing_swath_index = add_element((&(globals.matches)), writing_swath_index, (int8_t*)start + offset, &new_value);

			++globals.num_matches;

			required_extra_bytes_to_record = match_length - 1;

			//show_info("### if match_length %d", match_length);
			//show_info("### if offset %d", offset);
			//show_info("### if required_extra_bytes_to_record  %d", required_extra_bytes_to_record);

			nMatchSize += 16;
			printf("### if   %16p %16p %16p, %8d %4d %d\n", globals.matches, writing_swath_index, &(writing_swath_index->data[0]), globals.num_matches, match_length, nMatchSize);
			
		}
		else if (required_extra_bytes_to_record)
		{
			old_value_and_match_info new_value = { get_u8b(&data_value), zero_flag };
			writing_swath_index = add_element((&globals.matches), writing_swath_index, (int8_t*)start + offset, &new_value);
			--required_extra_bytes_to_record;

			//show_info("### else required_extra_bytes_to_record %d", required_extra_bytes_to_record);
			//show_info("### else offset %d", offset);

			nMatchSize += 8;
			printf("### else %16p %16p %16p %d\n", globals.matches, writing_swath_index, &(writing_swath_index->data[required_extra_bytes_to_record+1]), nMatchSize);
			
		}
	}

	show_info("### 27406336: last offset %d", offset);
	fclose(memfile);

	if (!(globals.matches = null_terminate(globals.matches, writing_swath_index /* ,MATCHES_AND_VALUES */)))
	{
		show_error("memory allocation error while reducing matches-array size\n");
		return -1;
	}

	//打印match数据
	unsigned i = 0;
	int buf_len = 128; /* will be realloc later if necessary */
	char *v = (char*)malloc(buf_len);
	if (v == NULL)
	{
		show_error("memory allocation failed.\n");
		return false;
	}
	char *bytearray_suffix = ", [bytearray]";
	char *string_suffix = ", [string]";


	matches_and_old_values_swath *reading_swath_index = (matches_and_old_values_swath *)globals.matches->swaths;
	int reading_iterator = 0;

	/* list all known matches */
	while (reading_swath_index->first_byte_in_child) {

		match_flags flags = reading_swath_index->data[reading_iterator].match_info;

		appendMatchToLogFile("### match number_of_bytes: %d", reading_swath_index->number_of_bytes);

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
				v = (char*)realloc(v, buf_len); /* for each byte and the suffix', this should be enough */

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
				v = (char *)realloc(v, buf_len);
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

			fprintf(stdout, "[%2u] %20p, %s\n", i++, remote_address_of_nth_element(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */), v);
			appendMatchToLogFile("[%2u] 0x%20p, %s\n", i, remote_address_of_nth_element(reading_swath_index, reading_iterator /* ,MATCHES_AND_VALUES */), v);
		}

		/* Go on to the next one... */
		++reading_iterator;
		if (reading_iterator >= reading_swath_index->number_of_bytes)
		{
			assert(((matches_and_old_values_swath *)(local_address_beyond_last_element(reading_swath_index /* ,MATCHES_AND_VALUES */)))->number_of_bytes >= 0);
			reading_swath_index = (matches_and_old_values_swath *)local_address_beyond_last_element(reading_swath_index /* ,MATCHES_AND_VALUES */);
			reading_iterator = 0;
		}
	}

	free(v);

	//check matches
	{
		matches_and_old_values_swath *reading_swath_index = (matches_and_old_values_swath *)globals.matches->swaths;
		matches_and_old_values_swath reading_swath = *reading_swath_index;

		long bytes_scanned = 0;
		long total_scan_bytes = 0;
		matches_and_old_values_swath *tmp_swath_index = reading_swath_index;
		while (tmp_swath_index->number_of_bytes)
		{
			total_scan_bytes += tmp_swath_index->number_of_bytes;
			tmp_swath_index = (matches_and_old_values_swath *)(&tmp_swath_index->data[tmp_swath_index->number_of_bytes]);
		}

		int reading_iterator = 0;
		matches_and_old_values_swath *writing_swath_index = (matches_and_old_values_swath *)globals.matches->swaths;
		writing_swath_index->first_byte_in_child = NULL;
		writing_swath_index->number_of_bytes = 0;

		/* used to fill in non-match regions */
		match_flags zero_flag;
		memset(&zero_flag, 0, sizeof(zero_flag));

		int required_extra_bytes_to_record = 0;
		globals.num_matches = 0;

		if (choose_scanroutine(scan_data_type, match_type) == false)
		{
			show_error("unsupported scan for current data type.\n");
			return false;
		}

		assert(g_scan_routine);

		while (reading_swath.first_byte_in_child) {
			int match_length = 0;
			value_t data_value;
			match_flags checkflags;

			void *address = (int8_t*)reading_swath.first_byte_in_child + reading_iterator;

			/* Read value from this address */
			memset(&data_value, 0x00, sizeof(data_value));
			//valnowidth(&data_value);//data_value flag 设置为1
			data_value.flags.s32b = 1;//过滤32位的项 201
			//data_value.flags.s16b = 1;//			 657
			//data_value.flags.s8b = 1;//			 10345
			data_value.int64_value = 90;
			
			if (false/*EXPECT(peekdata(vars->target, address, &data_value) == false, false)*/) {
				/* Uhh, what? We couldn't look at the data there? I guess this doesn't count as a match then */
			}
			else
			{
				value_t old_val = data_to_val_aux(reading_swath_index, reading_iterator, reading_swath.number_of_bytes /* ,MATCHES_AND_VALUES */);

				match_flags flags = reading_swath_index->data[reading_iterator].match_info;
				/* these are not harmful for bytearray routine, since it will ignore flags of new_value & old_value */
				truncval_to_flags(&old_val, flags);
				truncval_to_flags(&data_value, flags);

				memset(&checkflags, 0, sizeof(checkflags));

				match_length = (*g_scan_routine)(&data_value, &old_val, &uservalue, &checkflags, address);
			}

			if (match_length > 0)
			{
				/* Still a candidate. Write data.
				(We can get away with overwriting in the same array because it is guaranteed to take up the same number of bytes or fewer, and because we copied out the reading swath metadata already.)
				(We can get away with assuming that the pointers will stay valid, because as we never add more data to the array than there was before, it will not reallocate.) */

				old_value_and_match_info new_value = { get_u8b(&data_value), checkflags };
				writing_swath_index = add_element((&globals.matches), writing_swath_index, address, &new_value /* ,MATCHES_AND_VALUES */);

				++globals.num_matches;

				required_extra_bytes_to_record = match_length - 1;
			}
			else if (required_extra_bytes_to_record)
			{
				old_value_and_match_info new_value = { get_u8b(&data_value), zero_flag };
				writing_swath_index = add_element(&globals.matches, writing_swath_index, address, &new_value /* ,MATCHES_AND_VALUES */);
				--required_extra_bytes_to_record;
			}

			if (EXPECT((total_scan_bytes >= 110) && (bytes_scanned % ((total_scan_bytes) / 10) == 10), false)) {
				/* for user, just print a dot */
				show_scan_progress(bytes_scanned, total_scan_bytes);
			}
			++bytes_scanned;

			/* Go on to the next one... */
			++reading_iterator;
			if (reading_iterator >= reading_swath.number_of_bytes)
			{
				assert(((matches_and_old_values_swath *)(&reading_swath_index->data[reading_swath.number_of_bytes]))->number_of_bytes >= 0);
				reading_swath_index = (matches_and_old_values_swath *)(&reading_swath_index->data[reading_swath.number_of_bytes]);
				reading_swath = *reading_swath_index;
				reading_iterator = 0;
				required_extra_bytes_to_record = 0; /* just in case */
			}
		}

		if (!(globals.matches = null_terminate(globals.matches, writing_swath_index /* ,MATCHES_AND_VALUES */)))
		{
			show_error("memory allocation error while reducing matches-array size\n");
			return false;
		}

		/* hack for front-end, it needs this information */
		/* TODO: we'll need progress for checkmatches too */
		if (true)
		{
			show_scan_progress(total_scan_bytes, total_scan_bytes);
		}

		show_info("we currently have %ld matches.\n", globals.num_matches);

	}

    return 0;
}

