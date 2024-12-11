#include "libft.h"
// #include "list/linked_list.h"
#include "parse_flag/parse_flag.h"

enum flag_enum {
	DIGIT_FLAG=1,
	STRING_FLAG=2,
	HEXA_FLAG=4,
};

void test_decimal_flag(FlagContext *c, u32 flag, u32 *wanted_val, u32 nb_wanted_val) {

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
	ft_printf_fd(2, "All test passed for '%c'\n", opt->flag_char);
}

void test_string_flag(FlagContext *c, u32 flag, char **wanted_val, u32 nb_wanted_val, u32 wanted_flag) {
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
	ft_printf_fd(2, "All test passed for '%c'\n", opt->flag_char);

}

// void call_tester() {
void call_tester( int argc, char **argv, u32 *wanted_digit, u32 nb_wanted_digit, char **wanted_str, u32 nb_wanted_str, char **wanted_hexa, u32 nb_wanted_hexa) {
	FlagContext *c = flag_context_init(argv);

	ft_printf_fd(1, "Argv input\n");
	for (int i = 0; i < argc; i++) {
		ft_printf_fd(1, "argv[%d]: |%s|\n", i, argv[i]);
	}

	add_flag_option(c, "count", DIGIT_FLAG, 'c');
	set_flag_option(c, DIGIT_FLAG, EOPT_VALUE_TYPE, DECIMAL_VALUE);
	set_flag_option(c, DIGIT_FLAG, EOPT_MAX_VAL, 100);
	set_flag_option(c, DIGIT_FLAG, EOPT_MULTIPLE_VAL, VALUE_APPEND);

	add_flag_option(c, "string", STRING_FLAG, 's');
	set_flag_option(c, STRING_FLAG, EOPT_VALUE_TYPE, CHAR_VALUE);	
	set_flag_option(c, STRING_FLAG, EOPT_MAX_VAL, 100);
	set_flag_option(c, STRING_FLAG, EOPT_MULTIPLE_VAL, VALUE_APPEND);

	add_flag_option(c, "hexa", HEXA_FLAG, 'a');
	set_flag_option(c, HEXA_FLAG, EOPT_VALUE_TYPE, HEXA_VALUE);	
	set_flag_option(c, HEXA_FLAG, EOPT_MAX_VAL, 100);
	set_flag_option(c, HEXA_FLAG, EOPT_MULTIPLE_VAL, VALUE_APPEND);
	// set_flag_option(c, HEXA_FLAG, EOPT_MULTIPLE_VAL, VALUE_OVERRID);

	/* call flag parser */
	u32 flag = parse_flag(argc, argv, c, &c->error);
	if (c->error) {
		ft_printf_fd(2, "Parse flag error\n");
	}

	test_decimal_flag(c, flag, wanted_digit, nb_wanted_digit);
	test_string_flag(c, flag, wanted_str, nb_wanted_str, STRING_FLAG);
	test_string_flag(c, flag, wanted_hexa, nb_wanted_hexa, HEXA_FLAG);


	display_option_list(*c);


	free_flag_context(c);
	free(c);
}


void test1() {
	char *argv[] = {"./test", "-c", "10", "-c", "20", "-s", "string1", "-c", "30", "-s", "KOALA", "-a", "ffaa", "-a", "01234fa", "-c", "40"};
	int argc = (sizeof(argv) / sizeof(char *)); 
	ft_printf_fd(1, "argc: %d\n", argc);
	u32 wanted_digit[] = {10, 20, 30, 40};
	char *wanted_str[] = { "string1", "KOALA"}; 
	char *wanted_hexa[] = { "ffaa", "01234fa" };

	u32 nb_digit = sizeof(wanted_digit) / sizeof(u32);
	u32 nb_str = sizeof(wanted_str) / sizeof(char *);
	u32 nb_hexa = sizeof(wanted_hexa) / sizeof(char *);

	call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa);
}

void test2() {
	char *argv[] = {"./test", "-c", "1", "-c2", "-sfoo", "-c", "3", "-s", "bar", "-adead01", "-a", "beef", "-c4"};
	int argc = (sizeof(argv) / sizeof(char *));
	ft_printf_fd(1, "argc: %d\n", argc);
	u32 wanted_digit[] = {1, 2, 3, 4};
	char *wanted_str[] = {"foo", "bar"};
	char *wanted_hexa[] = {"dead01", "beef"};

	u32 nb_digit = sizeof(wanted_digit) / sizeof(u32);
	u32 nb_str = sizeof(wanted_str) / sizeof(char *);
	u32 nb_hexa = sizeof(wanted_hexa) / sizeof(char *);

	call_tester(argc, argv, wanted_digit, nb_digit, wanted_str, nb_str, wanted_hexa, nb_hexa);
}

int main (int argc, char **argv) {

	test1();
	test2();
	return 0;
}