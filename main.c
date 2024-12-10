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

void call_tester() {
	char *argv[] = {"./test", "-c", "10", "-c", "20", "-s", "string1", "-c", "30", "-s", "KOALA", "-a", "ffaa", "-a", "01234fa"};
	FlagContext * c = ft_calloc(sizeof(FlagContext), 1);
	c->prg_name = "./a.out";
	s8 error = 0;
	int argc = (sizeof(argv) / sizeof(char *)); 
	ft_printf_fd(1, "argc: %d\n", argc);
	u32 wanted_val[] = {10, 20, 30};
	char *wanted_str[] = { "string1", "KOALA"}; 
	char *wanted_hexa[] = { "ffaa", "01234fa" };


	add_flag_option(c, 'c', DIGIT_FLAG, 100, DECIMAL_VALUE, "count");
	add_flag_option(c, 's', STRING_FLAG, 100, CHAR_VALUE, "string");
	add_flag_option(c, 'a', HEXA_FLAG,  16, HEXA_VALUE, "hexa");

	/* call flag parser */
	u32 flag = parse_flag(argc, argv, c, &error);

	test_decimal_flag(c, flag, wanted_val, 3);
	test_string_flag(c, flag, wanted_str, 2, STRING_FLAG);
	test_string_flag(c, flag, wanted_hexa, 2, HEXA_FLAG);


	display_option_list(*c);


	free_flag_context(c);
	free(c);
}


int main (int argc, char **argv) {

	call_tester();
	return 0;
}