# include "parse_flag.h"

# define COLOR_MAX 7

void display_flags(char *all_flag, int flags) {
    const char  *flag_colors[COLOR_MAX] = {RED,GREEN,YELLOW,PINK,CYAN,ORANGE,PURPLE};
    const char  *str_suffix = "_OPTION";

    ft_printf_fd(2, FILL_YELBLACK"Flags: [%d] : "RESET, flags);
    for (int i = 0; all_flag && all_flag[i]; ++i) {
        if ((flags >> i) & 1) {
            ft_printf_fd(2, "%s[%c%s]%s, ", flag_colors[i % COLOR_MAX], all_flag[i], str_suffix, RESET);
        }
    }
    ft_printf_fd(2, "\n");
}

FlagContext *flag_context_init(char **argv) {
	FlagContext *c = ft_calloc(sizeof(FlagContext), 1);

	if (!c) {
		ft_printf_fd(2, "malloc error flag context init\n");
		return (NULL);
	}

	c->prg_name = argv[0];
	c->error = 0;
	return (c);
}


/** 
 *	@brief Get flag value from char
 *	@param flag_c flag context
 *	@param c pointer on char to check
*/
static int get_flag_value(FlagContext *flag_c, char *c) 
{
    int i = 0;
    int flag = -1;
	char *all_flag = flag_c->opt_str;

    while (all_flag && all_flag[i]) {
        if (*c == all_flag[i]) {
            flag = (1 << i);
            break;
        }
        ++i;
    }
    return (flag);
}

/**
 *	@brief Get flag value from full name
 *	@param flag_c flag context
 *	@param full_name pointer on full name to check
*/
int flag_value_long_format(FlagContext *flag_c, char *full_name)
{
    OptNode *opt = NULL;
    int flag = -1;

    opt = search_exist_opt(flag_c->opt_lst, is_same_full_name, full_name);
	// ft_printf_fd(2, "full_name: %s\n", full_name);
    if (opt) {
		// ft_printf_fd(2, "FOUND opt->fullname: %s\n", opt->full_name);
	    flag = opt->flag_val;
    }
    return (flag);

}

/**
 *	@brief Check for add flag
 *	@param str pointer on string to check
 *	@param flag_c flag context
 *	@param flags pointer on flags to fill
 *	@param error pointer on error
 *	@param long_option long option or short option
 *	@return opt_node if found, NULL otherwise
*/
static void *check_for_flag(char *str, FlagContext *flag_c, u32 *flags, s8 *error, u8 long_option)
{
    OptNode	*opt = NULL;
    int			tmp_value = 0;
	/* Function ptr to choice between char or long format function call */
	int			(*get_flag_val_func)(FlagContext *, char *) = get_flag_value;
	s8		(*is_same_func)(void *, void *) = is_same_char_opt;
	int			j_start = 1;

	if (long_option) {
		get_flag_val_func = flag_value_long_format;
		is_same_func = is_same_full_name;
		j_start = 2;
	}

    if (!str[j_start]) {
        ft_printf_fd(2, "j_start %d"PARSE_FLAG_ERR_MSG,  j_start, flag_c->prg_name, &str[j_start],  flag_c->prg_name);
        *error = -1;
		return (NULL);
    } 
	for (int j = j_start; str[j]; ++j) {
		// ft_printf_fd(1, RED"str: %s\n"RESET, &str[j]);
		tmp_value = get_flag_val_func(flag_c, &str[j]);
		if (tmp_value == -1) {
			ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  flag_c->prg_name, &str[j],  flag_c->prg_name);
			*error = -1;
			return (NULL);
		}
		if (!flag_already_present(*flags, tmp_value)) {
			set_flag(flags, tmp_value);
		}
		opt = search_exist_opt(flag_c->opt_lst, is_same_func, &str[j]);
		if (opt) {
			break ;
		}
	}

	// TODO: need to implement logic for multiple value handling (no accepted overide or append)
	// if (opt && opt->has_value) {
	// 	if (opt->value_type == CHAR_VALUE && opt->val.str != NULL) {
	// 		// ft_printf_fd(2, "%s: check_for_flag error value already set for flag %c\n", flag_c->prg_name, opt->flag_char);
	// 		return (NULL);
	// 	}
	// }
    return (opt);
}

/** 
 * @brief Check if char is space
 * @param c char to check
 * @return 1 if space, 0 otherwise
*/
static s8 is_char_space(char c) {
    return (c == ' ' || (c >= '\t' && c <= '\r'));
}

/** 
 * @brief Find next char that is not space
 * @param str string to check
 * @return next char that is not space
*/
static char find_next_no_space(char *str) {
    int i = 0;

    while (str[i] && is_char_space(str[i])) {
        ++i;
    }
    return (str[i]);
}

/**
 * @brief Parse flag value
 * @param str string to parse
 * @return value if valid, 0 otherwise
*/

// static s8 set_flag_value(OptNode *opt, char *str, u32 max_accepted, s8 value_type) {
//     u64 tmp = 0;
// 	if (value_type == DECIMAL_VALUE) {
// 		if (str_is_digit(str)) {
// 			tmp = array_to_uint32(str);
// 			if (tmp == OUT_OF_UINT32) {
// 				tmp = 0;
// 			}
// 		}
//     	/* value * 1 if true othewise return 0 */
// 	    tmp *= (tmp <= max_accepted);
// 		opt->val.digit = (u32)tmp;
// 		return (opt->val.digit != 0);
// 	} else if (value_type == HEXA_VALUE) {
// 		if (str_is_hexa(str) && ft_strlen(str) <= opt->max_val) {
// 			opt->val.str = ft_strdup(str);
// 			return (TRUE);
// 		}
// 	} else if (value_type == CHAR_VALUE) {
// 		if (ft_strlen(str) <= opt->max_val) {
// 			opt->val.str = ft_strdup(str);
// 			return (TRUE);
// 		}
// 	}
//     return (FALSE);
// }


U_OptValue *opt_val_new() {
	U_OptValue *opt_val = ft_calloc(sizeof(U_OptValue), 1);
	return (opt_val);
}

U_OptValue *get_first_opt_value(OptNode *opt) {
	if (!opt->val_lst) {
		return (NULL);
	}
	return (opt->val_lst->content);
}

static s8 can_append_value(OptNode *opt) {
	if (opt->multiple_val == VALUE_APPEND) {
		return (TRUE);
	} else if (opt->multiple_val != VALUE_APPEND && opt->nb_stored_val == 0) {
		return (TRUE);
	}
	return (FALSE);
}

static s8 can_override_value(OptNode *opt) {
	if (opt->multiple_val == VALUE_OVERRID) {
		return (TRUE);
	}
	return (FALSE);
}

static void override_value(OptNode *opt, U_OptValue *opt_value) {
	U_OptValue *stored_val = get_first_opt_value(opt);
	if (!stored_val) {
		ft_printf_fd(2, "Error in override value, can't find first value\n");
		return ;
	}
	if (opt->value_type == DECIMAL_VALUE) {
		stored_val->digit = opt_value->digit;
	} else if (opt->value_type == CHAR_VALUE  || opt->value_type == HEXA_VALUE) {
		free(stored_val->str);
		stored_val->str = ft_strdup(opt_value->str);
	}
	// call free optvalue here
	if (opt_value->str) {
		free(opt_value->str);
	}
	free(opt_value);
} 

static s8 handle_value_add(OptNode *opt, U_OptValue *opt_val) {
	if (can_append_value(opt)) {
		ft_lstadd_back(&opt->val_lst, ft_lstnew(opt_val));
		opt->nb_stored_val += 1;
	} else if (can_override_value(opt)) {
		override_value(opt, opt_val);
	} else {
		free(opt_val);
		ft_printf_fd(2, "Can't override value return ERROR\n");
		// If we are here we can't append val (not the first or not append value is set, and we can't override it)
		return (FALSE);
	}
	return (TRUE);
}

static s8 insert_digit_val(OptNode* opt, U_OptValue *opt_val, char *str) {
    u64 value = 0;

	value = array_to_uint32(str);
	if (value == OUT_OF_UINT32) {
		ft_printf_fd(2, "Overflow uint32 in set_flag_value\n");
		free(opt_val);
		return (FALSE);
	}
	if (value <= opt->max_val) {
		opt_val->digit = (u32)value;
		return (handle_value_add(opt, opt_val));
		// if (can_append_value(opt)) {
		// 	ft_lstadd_back(&opt->val_lst, ft_lstnew(opt_val));
		// 	opt->nb_stored_val += 1;
		// } else if (can_override_value(opt)) {
		// 	override_value(opt, opt_val);
		// } else {
		// 	free(opt_val);
		// 	ft_printf_fd(2, "Can't override value return ERROR\n");
		// 	// If we are here we can't append val (not the first or not append value is set, and we can't override it)
		// 	return (FALSE);
		// }
		// return (TRUE);
	}
	free(opt_val);
	return (FALSE);
}

static s8 insert_string_val(OptNode *opt, U_OptValue *opt_val, char *str) {
	opt_val->str = ft_strdup(str);
	return (handle_value_add(opt, opt_val));
	// ft_lstadd_back(&opt->val_lst, ft_lstnew(opt_val));
	// opt->nb_stored_val += 1;
	// return (TRUE);
}

static s8 set_flag_value(OptNode *opt, char *str, s8 value_type) {
	U_OptValue *opt_val = NULL;;
	opt_val = opt_val_new();



	if (value_type == DECIMAL_VALUE) {
		if (str_is_digit(str)) {
			return (insert_digit_val(opt, opt_val, str));
		}
	} else if (value_type == HEXA_VALUE) {
		if (str_is_hexa(str) && ft_strlen(str) <= opt->max_val) {
			return (insert_string_val(opt, opt_val, str));
		}
	} else if (value_type == CHAR_VALUE) {
		if (ft_strlen(str) <= opt->max_val) {
			return (insert_string_val(opt, opt_val, str));
		}
	}
	free(opt_val);
    return (FALSE);
}

/**
 * @brief Search for value linked to flag
 * @param argv pointer on argv
 * @param i pointer on index
 * @param opt pointer on opt node to update
 * @return 1 if found, 0 otherwise
*/
int search_opt_value(char **argv, int *i, char *prg_name, OptNode *opt, u8 long_format_bool)
{
    int		char_skip = 0;
    s8	in_search = 0 , next_char = 0, ret = 0;
	/* to skip idx start at 2 to skip '-' + '?' (char option) */
	/* if long format enable, add strlen of name, keep first 2 for '--' */
	int		to_skip_idx = (long_format_bool == LONG_FORMAT) ? 2 + ft_strlen(opt->full_name) : 2;
	/* idx to avoid [*i + j] multiple time*/
	int		idx = *i;

    for (int j = 0; argv[*i + j]; ++j) {
        idx = *i + j;
		in_search = 1;
        char_skip = ((j == 0) * to_skip_idx);
        next_char = find_next_no_space(&argv[idx][char_skip]);
		if (next_char != 0) {
            ret = set_flag_value(opt, &argv[idx][char_skip], opt->value_type);
            if (ret == 0) {
                ft_printf_fd(2, PARSE_FLAG_ERR_MSG_WRONG_ARGS, prg_name, opt->flag_char, &argv[idx][char_skip], prg_name);
                return (FALSE);
            }
            argv[idx] = "";
            *i += j;
            in_search = 0;
            break;
        }
        argv[idx] = "";
    }
    if (in_search){
        ft_printf_fd(2, RESET""PARSE_FLAG_ERR_MSG_ARGS_REQ, prg_name, opt->flag_char,  prg_name);
        return (FALSE);
    }
    return (TRUE);
}

// ft_printf_fd(2, CYAN"update opt |%c|, curr val %d -> ", opt->flag_char, opt->value);
// ft_printf_fd(2, "new val %u, j = %d, first off %d\n"RESET, opt->value, j, char_skip); 

/**
 * @brief Get option value from flag context
 * @param opt_lst option list
 * @param flag flag value
 * @param to_find flag to find
 * @return allocated copy option value or NULL if not found
 */
OptNode *get_opt_node(t_list *opt_lst, u32 flag, u32 to_find)
{
	OptNode	*opt = NULL;
	// void		*ret = NULL;

	if (has_flag(flag, to_find)) {
		opt = search_exist_opt(opt_lst, is_same_flag_val_opt, (void *)&to_find);
		// if (opt && opt->value_type == DECIMAL_VALUE) {
		// 	ret = malloc(sizeof(u32));
		// 	*(u32 *)ret = opt->val.digit;
		// } else if (opt && (opt->value_type == HEXA_VALUE || opt->value_type == CHAR_VALUE)) {
		// 	ret = ft_strdup(opt->val.str);
		// }
	}
	return (opt);
}

/**
 * @brief Parse flag
 * @param argc number of argument
 * @param argv pointer on argument
 * @param flag_c pointer on flag context
 * @param error pointer on error
 * @return flags if valid, 0 otherwise and set error to -1
*/
u32 parse_flag(int argc, char **argv, FlagContext *flag_c, s8 *error)
{
    OptNode	*opt = NULL;
    u32			flags = 0;
	u8		long_format_bool = CHAR_FORMAT;

    for (int i = 1; i < argc; ++i) {
        // ft_printf_fd(1, YELLOW"Check str flag:argv[%d] %s\n"RESET,i, argv[i]);
        if (argv[i][0] == '-') {
            /* if second char is '-' check for long format instead of char */
			if (argv[i][1] && argv[i][1] == '-') {
				long_format_bool = LONG_FORMAT;
			} 

			opt = check_for_flag(argv[i], flag_c, &flags, error, long_format_bool);
            if (*error == -1) { /* if invalid flag return */
                return (FALSE);
            } else if (opt && opt->has_value && !search_opt_value(argv, &i, flag_c->prg_name, opt, long_format_bool)) {
                *error = -1;
                return (FALSE);
            }
			if (opt) {
				argv[i] = "";
			}
        }
		/* Reset long format bool */
		long_format_bool = CHAR_FORMAT;
    }

    // for (int i = 0; i < argc; ++i) {
    //     ft_printf_fd(1, YELLOW"argv[%d] %s\n"RESET,i, argv[i]);
    // }

    // display_flags(flag_c->opt_str, flags);
	return (flags);
}

