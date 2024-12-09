#include "libft.h"
// #include "list/linked_list.h"
#include "parse_flag/parse_flag.h"

int main (void) {

	FlagContext * c = ft_calloc(sizeof(FlagContext), 1);

	add_flag_option(c, 'c', 1, 10, DECIMAL_VALUE, "count");
	display_option_list(*c);
	
	
	free_flag_context(c);
	free(c);
	return 1;
}