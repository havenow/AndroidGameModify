# 结构体

```
typedef struct {
	uint8_t old_value;
	match_flags match_info;//4
} old_value_and_match_info;//8

typedef struct {
	void *first_byte_in_child;
	unsigned long number_of_bytes;
	old_value_and_match_info data[0];
} matches_and_old_values_swath;//8

typedef struct {
	unsigned long bytes_allocated;
	unsigned long max_needed_bytes;
	matches_and_old_values_swath swaths[0];
} matches_and_old_values_array;//8

typedef struct {
	matches_and_old_values_swath *swath;
	long index;
} match_location;//8
```

# 取出内存数据(8字节)，比较数据，匹配的保存

```
	matches_and_old_values_swath *writing_swath_index;
	int required_extra_bytes_to_record = 0;
    writing_swath_index = (matches_and_old_values_swath *)vars->matches->swaths;
    
    writing_swath_index->first_byte_in_child = NULL;
    writing_swath_index->number_of_bytes = 0;
	

			region_t *r;
			match_flags checkflags;
            value_t data_value;
			peekdata(vars->target, address, &data_value); //data_value->int64_value =    *((int64_t *)&peekbuf.cache[reqaddr - peekbuf.base]);
			memset(&checkflags, 0, sizeof(checkflags));
            int match_length;
			
			if (EXPECT(((match_length = (*g_scan_routine)(&data_value, NULL, uservalue, &checkflags, address)) > 0), false)) 
			{
                /* only set these flags for numbers */
                if ((globals.options.scan_data_type != BYTEARRAY)
                    && (globals.options.scan_data_type != STRING))
                {
                    checkflags.ineq_forwards = checkflags.ineq_reverse = 1;
                }
                old_value_and_match_info new_value = { get_u8b(&data_value), checkflags };
                writing_swath_index = add_element((&vars->matches), writing_swath_index, r->start + offset, &new_value);
                
                ++vars->num_matches;
                
                required_extra_bytes_to_record = match_length - 1;
            }
            else if (required_extra_bytes_to_record)
            {
                old_value_and_match_info new_value = { get_u8b(&data_value), zero_flag };
                writing_swath_index = add_element((&vars->matches), writing_swath_index, r->start + offset, &new_value);
                --required_extra_bytes_to_record;
            }
```

# add_element
add_element 每次add一个int8_t数据    
```
/* Returns a pointer to the swath to which the element was added - i.e. the last swath in the array after the operation */
matches_and_old_values_swath * add_element (matches_and_old_values_array **array, matches_and_old_values_swath *swath, void *remote_address, void *new_element )
{
    if (swath->number_of_bytes == 0)
    {
        assert(swath->first_byte_in_child == NULL);
        
        /* We have to overwrite this as a new swath */
        *array = allocate_enough_to_reach(*array, (void *)swath + sizeof(matches_and_old_values_swath) + sizeof(old_value_and_match_info), &swath);
        
        swath->first_byte_in_child = remote_address;
    }
    else
    {
        unsigned long local_index_excess = remote_address - remote_address_of_last_element(swath );
        unsigned long local_address_excess = local_index_excess * sizeof(old_value_and_match_info);
         
        if (local_address_excess >= sizeof(matches_and_old_values_swath) + sizeof(old_value_and_match_info))
        {
            /* It is most memory-efficient to start a new swath */
            *array = allocate_enough_to_reach(*array, local_address_beyond_last_element(swath ) + sizeof(matches_and_old_values_swath) + sizeof(old_value_and_match_info), &swath);

            swath = local_address_beyond_last_element(swath );
            swath->first_byte_in_child = remote_address;
            swath->number_of_bytes = 0;
        }
        else
        {
            /* It is most memory-efficient to write over the intervening space with null values */
            *array = allocate_enough_to_reach(*array, local_address_beyond_last_element(swath ) + local_address_excess, &swath);
            memset(local_address_beyond_last_element(swath), 0, local_address_excess);
            swath->number_of_bytes += local_index_excess - 1;
        }
    }
    
    /* Add me */
    *(old_value_and_match_info *)(local_address_beyond_last_element(swath )) = *(old_value_and_match_info *)new_element;
    ++swath->number_of_bytes;
    
    return swath;
}
```

# 比较的代码 int32

```
typedef int(*scan_routine_t)(const value_t *new_value, const value_t *old_value, const uservalue_t *user_value, match_flags *saveflag, void *address);
extern scan_routine_t g_scan_routine;
scan_routine_t get_scanroutine(scan_data_type_t dt, scan_match_type_t mt)
{
	//SCANDATATYPE, ROUTINEDATATYPENAME, SCANMATCHTYPE, ROUTINEMATCHTYPENAME
	//INTEGER8, INTEGER8, MATCHEQUALTO, EQUALTO
	//INTEGER8, INTEGER8, MATCHNOTEQUALTO, NOTEQUALTO
	if ((dt == MATCHEQUALTO) && (mt == MATCHEQUALTO))
	{
		return &scan_routine_INTEGER32_EQUALTO;
	}
}

int scan_routine_INTEGER32_EQUALTO(const value_t *new_value, const value_t *old_value, const uservalue_t *user_value, match_flags *saveflags, void *address)
{
	assert(user_value);
	int ret = 0;
	if (((new_value)->flags.s32b && (user_value)->flags.s32b) && (new_value->int32_value == user_value->int32_value))
	{
		saveflags->s32b = 1;
		ret = (32) / 8;
	}
	if (((new_value)->flags.u32b && (user_value)->flags.u32b) && (new_value->uint32_value == user_value->uint32_value))
	{
		saveflags->u32b = 1;
		ret = (32) / 8;
	}
	return ret;
}
```
