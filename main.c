#include "libft.h"
// #include "list/linked_list.h"
#include "parse_flag/parse_flag.h"

void test_decimal_flag(FlagContext *c, u32 flag, u32 *wanted_val, u32 nb_wanted_val) {

	OptNode *opt = get_opt_node(c->opt_lst, flag, 1);

	u32 i = 0;

	for (t_list *val_lst = opt->val_lst; val_lst; val_lst = val_lst->next) {
		U_OptValue *val = val_lst->content;
		if (opt->value_type == DECIMAL_VALUE) {
			if (val->digit != wanted_val[i]) {
				ft_printf_fd(2, "Error: wanted %d, got %d\n", wanted_val[i], val->digit);
			}
		}
		i++;
	}

	if (i != nb_wanted_val) {
		ft_printf_fd(2, "Error: wanted %d, got %d\n", nb_wanted_val, i);
	}
	ft_printf_fd(2, "All test passed for '%c'\n", opt->flag_char);
	// display_option_list(*c);
}

void call_tester() {
	char *argv[] = {"./test", "-c", "10", "-c", "20", "-c", "30", "-c", "40", "-c", "50", "-c", "60", "-c", "70", "-c", "80", "-c", "90", "-c", "100"};
	FlagContext * c = ft_calloc(sizeof(FlagContext), 1);
	s8 error = 0;

	add_flag_option(c, 'c', 1, 100, DECIMAL_VALUE, "count");
	u32 wanted_val[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	u32 flag = parse_flag(21, argv, c, &error);
	test_decimal_flag(c, flag, wanted_val, 10);

	free_flag_context(c);
	free(c);
}


int main (int argc, char **argv) {

	call_tester();
	return 1;
}