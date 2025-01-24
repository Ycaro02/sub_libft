#include "libft.h"
// #include "list/linked_list.h"
#include "parse_flag/parse_flag.h"

enum flag_enum {
	DIGIT_FLAG=1,
	STRING_FLAG=2,
	HEXA_FLAG=4,
	OCTAL_FLAG=8,
	KUSTOM_FLAG=16,
};

void verify_decimal_flag(FlagContext *c, u32 flag, u32 *wanted_val, u32 nb_wanted_val) {

	OptNode *opt = get_opt_node(c->opt_lst, flag, DIGIT_FLAG);

	u32 i = 0;

	if (!opt) {
		ft_printf_fd(1, "No opt found for decimal\n");
		return;
	}

	for (t_list *val_lst = opt->val_lst; val_lst; val_lst = val_lst->next) {
		U_OptValue *val = val_lst->content;
		if (val->digit != wanted_val[i]) {
			ft_printf_fd(2, "Error decimal %d: wanted %d, got %d\n", i, wanted_val[i], val->digit);
		}
		i++;
	}

	if (i != nb_wanted_val) {
		ft_printf_fd(2, "Error: wanted %d, got %d\n", nb_wanted_val, i);
	}
	// ft_printf_fd(2, "All test passed for '%c'\n", opt->flag_char);
}

void verify_string_flag(FlagContext *c, u32 flag, char **wanted_val, u32 nb_wanted_val, u32 wanted_flag) {
	OptNode *opt = get_opt_node(c->opt_lst, flag, wanted_flag);
	u32 i = 0;

	if (!opt) {
		ft_printf_fd(1, "No opt found for string\n");
		return;
	}


	for (t_list *val_lst = opt->val_lst; val_lst; val_lst = val_lst->next) {
		U_OptValue *val = val_lst->content;
		if (ftlib_strcmp(val->str, wanted_val[i]) != 0) {
			ft_printf_fd(2, "Error string %d: wanted %s, got %s\n", i, wanted_val[i], val->str);
		}
		i++;
	}
	if (i != nb_wanted_val) {
		ft_printf_fd(2, "Error: wanted %d, got %d\n", nb_wanted_val, i);
	}
	// ft_printf_fd(2, "All test passed for '%c'\n", opt->flag_char);

}

s8 test_parse(void *c) {
	(void)c;
	ft_printf_fd(1, "Call test_parse OK\n");
	return (TRUE);
}

/* NMAP PARSING PORT START HERE */

s32 count_char(char *str, char c) {
	s32 nb = 0, i = 0;
	while (str && str[i]) {
		if (str[i] == c) {
			nb++;
		}
		i++;
	}
	return (nb);
}

s8 is_space(char c) {
	return ((c >= '\b' && c <= '\r') || c == ' ');
}

s8 is_accepted_nmap_port_char(char c) {
	if (!ft_isdigit(c) && !is_space(c) && c != '-' && c != ',') {
		return (FALSE);
	}
	return (TRUE);
}

s8 is_valid_port(char *port_str) {
	s32 port = ft_atoi(port_str);
	return (port >= 0 && port <= 65535);
}

s8 insert_port_value(OptNode *node, char *str) {
	U_OptValue *opt_val = NULL;;
	opt_val = opt_val_new();

	return (insert_digit_val(node, opt_val, str));
}


s8 parse_substring_port_str(OptNode *node, char *str) {
	s32 nb_hyphen = 0, str_len = 0;

	str_len = ft_strlen(str);
	
	if ((nb_hyphen = count_char(str, '-')) > 1) {
		ft_printf_fd(2, "Error: nb hyphen greather than 1 -> %d\n", nb_hyphen);
		return (FALSE);
	} 

	if (nb_hyphen == 1) {
		if (str_len == 1) {
			/* Special usage specify all port */
			return (TRUE);
		} else {
			/* Here we need to split to compute the range of port to check */
			char **port_range = ft_split_trim(str, '-');
			u32 nb_port_string = double_char_size(port_range); 
			if (nb_port_string != 2) {
				free_double_char(port_range);
				ft_printf_fd(2, "Error: the split must be equal to 2 -> %u\n", nb_port_string);
				return (FALSE);
			}
			for (u32 i = 0; i < nb_port_string; i++) {
				// TODO check if the first port is lower than the second to detect the range
				if (!is_valid_port(port_range[i])) {
					ft_printf_fd(2, "Error: Parsing port -> %s\n", port_range[i]);
					return (FALSE);
				}
			}
			s32 port_start = ft_atoi(port_range[0]);
			s32 port_end = ft_atoi(port_range[1]);
			if (port_end < port_start) {
				ft_printf_fd(2, "Your port range %d-%d is backwards. Did you mean %d-%d?\n", port_start, port_end, port_end, port_start);
				return (FALSE);
			}
			// Here build list of port and add it to node_value
			return (TRUE);
		}
	}
	// here is simple port specify
	if (!is_valid_port(str)) {
		ft_printf_fd(2, "Parsing port error -> %s\n", str);
		return (FALSE);
	}
	s8 ret = insert_port_value(node, str);
	if (ret != SUCCESS_SET_VALUE) {
		return (FALSE);
	}
	return (TRUE);
}

s8 parse_nmap_port(void *optnode, void *data) {
	OptNode *node = optnode; 
	char *str = data;
	s32 i = 0, str_len = 0;
	u32 nb_comma = 0;


	if (!str) { return (FALSE); }
	str_len = ft_strlen(str);
	if (str_len == 0) { return (FALSE); }

	while (str[i]) {
		if (!is_accepted_nmap_port_char(str[i])) {
			ft_printf_fd(2, "Incorect char detected in |%s| -> %c\n", str, str[i]);
			return (FALSE);
		}
		i++;
	}

	char **splited_comma = NULL;
	if ((nb_comma = count_char(str, ',')) > 0) {
		splited_comma = ft_split_trim(str, ',');
		u32 nb_sub_string = double_char_size(splited_comma);
		if (nb_sub_string != nb_comma + 1 && nb_sub_string != nb_comma) {
			ft_printf_fd(2, "Incorect number of substring after coma split got: %u -> expected: %u or %u\n", nb_sub_string, nb_comma, nb_comma +1);
			return (FALSE);
		}
		for (s32 i = 0; splited_comma[i]; i++) {
			s8 ret = parse_substring_port_str(node, splited_comma[i]);
			if (!ret) {
				free_double_char(splited_comma);
				return (FALSE);
			}		
		}
		return (TRUE);
	}
	parse_substring_port_str(node, str);
	// no hyphen detect just parse port with the same logic than the parse port section
	return (TRUE);
}

/* NMAP PARSING PORT END HERE */

void init_flag_context(FlagContext *c, u8 value_handling) {
	u32 max_val = 100;
	u32 min_val = 2;

	u32 decimal_val = DECIMAL_VALUE;
	u32 char_val = CHAR_VALUE;
	u32 hexa_val = HEXA_VALUE;
	u32 octal_val = OCTAL_VALUE;

	add_flag_option(c, "count", DIGIT_FLAG, 'c');
	set_flag_option(c, DIGIT_FLAG, EOPT_VALUE_TYPE, decimal_val);
	set_flag_option(c, DIGIT_FLAG, EOPT_MAX_VAL, max_val);
	set_flag_option(c, DIGIT_FLAG, EOPT_MULTIPLE_VAL, value_handling);
	set_flag_option(c, DIGIT_FLAG, EOPT_MIN_VAL, min_val);

	add_flag_option(c, "string", STRING_FLAG, 's');
	set_flag_option(c, STRING_FLAG, EOPT_VALUE_TYPE, char_val);	
	set_flag_option(c, STRING_FLAG, EOPT_MAX_VAL, max_val);
	set_flag_option(c, STRING_FLAG, EOPT_MULTIPLE_VAL, value_handling);
	set_flag_option(c, STRING_FLAG, EOPT_MIN_VAL, min_val);

	add_flag_option(c, "hexa-val", HEXA_FLAG, 'a');
	set_flag_option(c, HEXA_FLAG, EOPT_VALUE_TYPE, hexa_val);	
	set_flag_option(c, HEXA_FLAG, EOPT_MAX_VAL, max_val);
	set_flag_option(c, HEXA_FLAG, EOPT_MULTIPLE_VAL, value_handling);

	add_flag_option(c, "octal", OCTAL_FLAG, 'o');
	set_flag_option(c, OCTAL_FLAG, EOPT_VALUE_TYPE, octal_val);	
	set_flag_option(c, OCTAL_FLAG, EOPT_MAX_VAL, max_val);
	set_flag_option(c, OCTAL_FLAG, EOPT_MULTIPLE_VAL, value_handling);

	add_flag_option(c, "kustom", KUSTOM_FLAG, 'k');
	set_flag_option(c, KUSTOM_FLAG, EOPT_VALUE_TYPE, CUSTOM_VALUE);
	set_flag_option(c, KUSTOM_FLAG, EOPT_MAX_VAL, 65535);
	set_flag_option(c, KUSTOM_FLAG, EOPT_MULTIPLE_VAL, value_handling);
	set_flag_option(c, KUSTOM_FLAG, EOPT_PARSE_FUNC, parse_nmap_port);
}


// void call_tester() {
int call_tester( int argc, char **argv, u32 *wanted_digit, u32 nb_wanted_digit, char **wanted_str, u32 nb_wanted_str, char **wanted_hexa, u32 nb_wanted_hexa, char **wanted_octal, u32 nb_wanted_octal, u8 value_handling) {
	FlagContext *c = flag_context_init(argv);

	init_flag_context(c, value_handling);

	/* call flag parser */
	u32 flag = parse_flag(argc, argv, c, &c->error);
	if (c->error) {
		// ft_printf_fd(2, YELLOW"Parse flag error\n"RESET);
		free_flag_context(c);
		return (0);
	}
	verify_decimal_flag(c, flag, wanted_digit, nb_wanted_digit);
	verify_string_flag(c, flag, wanted_str, nb_wanted_str, STRING_FLAG);
	verify_string_flag(c, flag, wanted_hexa, nb_wanted_hexa, HEXA_FLAG);

	if (nb_wanted_octal > 0) {
		verify_string_flag(c, flag, wanted_octal, nb_wanted_octal, OCTAL_FLAG);
	}


	// debug display all flag and val here
	// display_option_list(*c);
	// debug display all flag and val here

	free_flag_context(c);
	return (1);
}


void test1_append() {
	char *argv[] = {"./test", "--count", "10", "-c", "20", "-s", "string1", "-c", "30", "-s", "KOALA", "-a", "ffaa", "-a", "01234fa", "-c", "40", "-o" "0127"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	u32 wanted_digit[] = {10, 20, 30, 40};
	char *wanted_str[] = { "string1", "KOALA"}; 
	char *wanted_hexa[] = { "ffaa", "01234fa" };
	char *wanted_octal[] = {"0127"};

	u32 nb_digit = sizeof(wanted_digit) / sizeof(u32);
	u32 nb_str = sizeof(wanted_str) / sizeof(char *);
	u32 nb_hexa = sizeof(wanted_hexa) / sizeof(char *);
	u32 nb_octal = sizeof(wanted_octal) / sizeof(char *);

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa, wanted_octal, nb_octal, VALUE_APPEND);
	if (ret != 1) {
		ft_printf_fd(1, RED"Test error\n"RESET);
	} else {
		ft_printf_fd(1, GREEN"Test 1 append success\n"RESET);
	}
}

void test2_append() {
	char *argv[] = {"./test", "-c", "10", "-c2", "-sfoo", "-c", "3", "-s", "bar", "-adead01", "-a", "beef", "-c4"};
	int argc = (sizeof(argv) / sizeof(char *));
	u32 wanted_digit[] = {10, 2, 3, 4};
	char *wanted_str[] = {"foo", "bar"};
	char *wanted_hexa[] = {"dead01", "beef"};

	u32 nb_digit = sizeof(wanted_digit) / sizeof(u32);
	u32 nb_str = sizeof(wanted_str) / sizeof(char *);
	u32 nb_hexa = sizeof(wanted_hexa) / sizeof(char *);

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa, NULL, 0,VALUE_APPEND);
	if (ret != 1) {
		ft_printf_fd(1, RED"Test error\n"RESET);
	} else {
		ft_printf_fd(1, GREEN"Test 2 append success\n"RESET);
	}
}


void test1_override() {
	char *argv[] = {"./test", "--count", "10", "-c", "20", "-s", "string1", "-c", "30", "-s", "KOALA", "-a", "ffaa", "-a", "01234fa", "-c", "40"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	u32 wanted_digit[] = {40};
	char *wanted_str[] = {"KOALA"}; 
	char *wanted_hexa[] = {"01234fa"};

	u32 nb_digit = 1;
	u32 nb_str = 1;
	u32 nb_hexa = 1;

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa, NULL, 0, VALUE_OVERRID);
	if (ret != 1) {
		ft_printf_fd(1, RED"Test error\n"RESET);
	} else {
		ft_printf_fd(1, GREEN"Test 1 override success\n"RESET);
	}
}

void test2_override() {
	char *argv[] = {"./test", "--count", "10", "-c", "20", "-s", "string1", "-c", "30", "-s", "KOALA", "-a", "ffaa", "-add", "-a", "01234fa", "-aff", "-c40", "-stest_last"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	u32 wanted_digit[] = {40};
	char *wanted_str[] = {"test_last"}; 
	char *wanted_hexa[] = {"ff"};

	u32 nb_digit = 1;
	u32 nb_str = 1;
	u32 nb_hexa = 1;

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa,NULL, 0, VALUE_OVERRID);
	if (ret != 1) {
		ft_printf_fd(1, RED"Test error\n"RESET);
	} else {
		ft_printf_fd(1, GREEN"Test 2 override success\n"RESET);
	}
}

void test1_no_override() {
	char *argv[] = {"./test", "--count", "10", "-slol", "-aff", "-aAAb"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	u32 wanted_digit[] = {10};
	char *wanted_str[] = {"lol"}; 
	char *wanted_hexa[] = {"ff"};

	u32 nb_digit = 1;
	u32 nb_str = 1;
	u32 nb_hexa = 1;

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa, NULL, 0,VALUE_NO_OVERRID);
	if (ret == 0) {
		ft_printf_fd(1, GREEN"Test Value can't be overrid return error\n"RESET);
	} else {
		ft_printf_fd(1, RED"Test Value can't be overrid no error\n"RESET);
	}
}


void test1_invalid_flag() {
	char *argv[] = {"./test", "--count", "10", "-slol", "-aff", "-c20", "-k", "testK"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	u32 wanted_digit[] = {10};
	char *wanted_str[] = {"lol"}; 
	char *wanted_hexa[] = {"ff"};

	u32 nb_digit = 1;
	u32 nb_str = 1;
	u32 nb_hexa = 1;

	int ret = call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa,NULL, 0, VALUE_APPEND);
	if (ret == 0) {
		ft_printf_fd(1, GREEN"Test invalid flag return error\n"RESET);
	} else {
		ft_printf_fd(1, RED"Test invalid flag no error\n"RESET);
	}
}



void test_input(int argc, char **argv) {
	FlagContext *c = flag_context_init(argv);
	init_flag_context(c, VALUE_APPEND);
	u32 flag = parse_flag(argc, argv, c, &c->error);
	if (c->error) {
		ft_printf_fd(2, YELLOW"Parse flag error\n"RESET);
		free_flag_context(c);
		return ;
	}
	// display_flags(c, flag);
	display_option_list(*c);
	free_flag_context(c);
}

int main (int argc, char **argv) {

	test1_append();
	test2_append();
	test1_override();
	test2_override();
	test1_no_override();
	test_input(argc, argv);
	// test1_invalid_flag();
	return 0;
}