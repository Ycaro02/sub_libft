#include "nmap.h"

static s8 is_accepted_nmap_port_char(char c) {
	if (!ft_isdigit(c) && !is_space(c) && c != '-' && c != ',') {
		return (FALSE);
	}
	return (TRUE);
}

/**
 * @brief Check if the string is a valid port
 * @param port_str string to check
 * @return TRUE if valid, FALSE otherwise
 */
static s8 is_valid_port(char *port_str) {
	s32 port = ft_atoi(port_str);
	return (port >= 0 && port <= 65535);
}

/**
 * @brief Insert port string in opt node
 * @param opt pointer on opt node
 * @param str string to insert
 * @return TRUE if success, FALSE otherwise
 */
// static s8 insert_port_string(OptNode *opt, char *str) {
static s8 insert_port_string(t_list **lst, char *str) {
	ft_lstadd_back(lst, ft_lstnew(ft_strdup(str)));
	return (TRUE);
	// U_OptValue *opt_val = NULL;;
	// opt_val = opt_val_new();
	// if (!opt_val) {
	// 	return (ERROR_SET_VALUE);
	// }
	// return (insert_string_val(opt, opt_val, str));

}

/**
 * @brief Check if the first port is lower than the second
 * @param port1 first port
 * @param port2 second port
 * @return TRUE if the first port is lower than the second, FALSE otherwise
 */
static s8 is_valid_port_range(char *port1, char *port2) {
	s32 port1_int = ft_atoi(port1);
	s32 port2_int = ft_atoi(port2);

	if (port1_int > port2_int) {
		return (FALSE);
	}
	return (TRUE);
}

/**
 * @brief Build string with port range
 * @param port1 first port
 * @param port2 second port
 * @return string with port range
 */
static char *build_trim_port_range_string(char *port1, char *port2) {
	char *str = ft_strdup(port1);
	str = ft_strjoin_free(str, "-", 'f');
	str = ft_strjoin_free(str, port2, 'f');
	return (str);
}

/**
 * @brief Parse port range (all string containing a hyphen)
 * @param opt pointer on opt node
 * @param str string to parse
 */
static s8 parse_port_range(t_list **list, char *str) {
	char	**port_range = ft_split_trim(str, '-');
	u32		nb_port_string = double_char_size(port_range); 
	
	if (nb_port_string != 2) {
		ft_printf_fd(2, "Error: the split must be equal to 2 -> %u\n", nb_port_string);
		goto error_case;
	}
	for (u32 i = 0; i < nb_port_string; i++) {
		// TODO check if the first port is lower than the second to detect the range
		if (!is_valid_port(port_range[i])) {
			ft_printf_fd(2, "Parsing port error -> %s\n", port_range[i]);
			goto error_case;
		}
		if (is_valid_port_range(port_range[0], port_range[1]) == FALSE) {
			ft_printf_fd(2, "Invalid port range -> %s\n", str);
			goto error_case;
		}
	}
	char *trim_str = build_trim_port_range_string(port_range[0], port_range[1]);

	free_double_char(port_range);
	insert_port_string(list, trim_str);
	return (TRUE);

	error_case:
		free_double_char(port_range);
		return (FALSE);
}

/**
 * @brief Parse substring port string (all string separated by comma)
 * @param opt pointer on opt node
 * @param str string to parse
 */
static s8 parse_substring_port_str(t_list **list, char *str) {
	s32 nb_hyphen = 0, str_len = 0;

	str_len = ft_strlen(str);
	
	if ((nb_hyphen = count_char(str, '-')) > 1) {
		ft_printf_fd(2, "Error: nb hyphen greather than 1 -> %d\n", nb_hyphen);
		return (FALSE);
	} 

	/* Here we had a port range and need to parse it*/
	if (nb_hyphen == 1) {
		/* Special usage specify all port */
		if (str_len == 1) {
			insert_port_string(list, DEFAULT_RANGE_PORT_STR);
			return (TRUE);
		} 
		return (parse_port_range(list, str));
	}

	/* Here we had a single port */
	if (!is_valid_port(str)) {
		ft_printf_fd(2, "Parsing port error -> %s\n", str);
		return (FALSE);
	}
	insert_port_string(list, str);
	// insert port here
	return (TRUE);
}

static t_list *parse_port_string(char *str) {
	char **splited_comma = NULL;
	u32 nb_comma = 0;
	t_list *lst = NULL;


	if ((nb_comma = count_char(str, ',')) > 0) {
		splited_comma = ft_split_trim(str, ',');
		u32 nb_sub_string = double_char_size(splited_comma);
		if (nb_sub_string != nb_comma + 1 && nb_sub_string != nb_comma) {
			ft_printf_fd(2, "Incorect number of substring after coma split got: %u -> expected: %u or %u\n", nb_sub_string, nb_comma, nb_comma +1);
			goto error_case;
		}
		for (s32 i = 0; splited_comma[i]; i++) {
			if (!parse_substring_port_str(&lst, splited_comma[i])) { goto error_case; }		
		}
		free_double_char(splited_comma);
		// return (TRUE);
		return (lst);
	}
	// return (parse_substring_port_str(&lst, str));
	return (lst);

	error_case:
		// need to lst clear
		free_double_char(splited_comma);
		return (NULL);
}

// main func to parse port
s8 parse_nmap_port(void *opt_ptr, void *data) {
	OptNode *opt = opt_ptr;
	char *str = data;
	s32 i = 0, str_len = 0;

	if (!str) { goto error_case; }
	str_len = ft_strlen(str);
	if (str_len == 0) { goto error_case; }

	while (str[i]) {
		if (!is_accepted_nmap_port_char(str[i])) {
			ft_printf_fd(2, "Incorect char detected in |%s| -> %c\n", str, str[i]);
			goto error_case;
		}
		i++;
	}

	return (TRUE);

	// s8 ret = parse_port_string(opt, str);
	// return (ret);

	error_case:
		return (FALSE);
}





static s8 port_already_present(t_list *int_port_list, s32 port) {
	for (t_list *tmp = int_port_list; tmp; tmp = tmp->next) {
		s32 *tmp_port = tmp->content;
		if (*tmp_port == port) {
			return (TRUE);
		}
	}
	return (FALSE);
}

static void insert_port_digit(t_list **int_port_lst, U_OptValue *val) {
	ft_printf_fd(1, "Port string: %s\n", val->str);
	if (count_char(val->str, '-') == 0) {
		if (port_already_present(*int_port_lst, ft_atoi(val->str))) {
			return ;
		}
		s32 *tmp = malloc(sizeof(s32)); 
		*tmp = ft_atoi(val->str);
		ft_lstadd_back(int_port_lst, ft_lstnew(tmp));
	} else {
		// here we need to generate all port between the range (maybe just genere number instead of string ?)
		char **port_range = ft_split_trim(val->str, '-');
		s32 port_start = ft_atoi(port_range[0]);
		s32 port_end = ft_atoi(port_range[1]);
		for (s32 i = port_start; i <= port_end; i++) {
			if (port_already_present(*int_port_lst, i)) { continue; }
			s32 *tmp = malloc(sizeof(s32));
			*tmp = i;
			ft_lstadd_back(int_port_lst, ft_lstnew(tmp));
		}
		free_double_char(port_range);
	}
}

static s32 sort_int(void *a, void *b) {
	return (*(s32 *)a - *(s32 *)b) < 0;
}

static t_list *port_string_to_digit(t_list *port_string_lst) {
	t_list *int_port_lst = NULL;
	while (port_string_lst) {
		insert_port_digit(&int_port_lst, port_string_lst->content);
		port_string_lst = port_string_lst->next;
	}
	return (int_port_lst);
}

s8 extend_port_string(FlagContext *c, u32 flag) {
	// Maybe just store the entire string and get the value here
	t_list *opt_val = get_opt_value(c->opt_lst, flag, FLAG_PORT);

	char *brut_string = ((U_OptValue *)opt_val)->str;

	t_list *port_lst = parse_port_string(brut_string);

	// then call nmap parse port here and continue same logic 

	t_list *int_port_lst = port_string_to_digit(port_lst);

	list_sort(&int_port_lst, sort_int);
	
	ft_printf_fd(1, "Port list: ");
	for (t_list *tmp = int_port_lst; tmp; tmp = tmp->next) {
		s32 *port = tmp->content;
		ft_printf_fd(1, "%d, ", *port);
	}

	s32 nb_port = ft_lstsize(int_port_lst);

	if (nb_port > MAX_PORT_SCAN) {
		ft_printf_fd(2, "Too many port to scan\n");
		ft_lstclear(&int_port_lst, free);
		return (FALSE);
	}
	return (TRUE);
}
