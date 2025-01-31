#include "libft.h"
#include "parse_flag/parse_flag.h"

// Go in libft
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
//

s8 is_accepted_nmap_port_char(char c) {
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
s8 is_valid_port(char *port_str) {
	s32 port = ft_atoi(port_str);
	return (port >= 0 && port <= 65535);
}

/**
 * @brief Insert port string in opt node
 * @param opt pointer on opt node
 * @param str string to insert
 * @return TRUE if success, FALSE otherwise
 */
s8 insert_port_string(OptNode *opt, char *str) {
	U_OptValue *opt_val = NULL;;
	opt_val = opt_val_new();
	if (!opt_val) {
		return (ERROR_SET_VALUE);
	}
	return (insert_string_val(opt, opt_val, str));

}

/**
 * @brief Check if the first port is lower than the second
 * @param port1 first port
 * @param port2 second port
 * @return TRUE if the first port is lower than the second, FALSE otherwise
 */
s8 is_valid_port_range(char *port1, char *port2) {
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
char *build_trim_port_range_string(char *port1, char *port2) {
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
s8 parse_port_range(OptNode *opt, char *str) {
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
	insert_port_string(opt, trim_str);
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
s8 parse_substring_port_str(OptNode *opt, char *str) {
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
			insert_port_string(opt, "0-65535");
			return (TRUE);
		} 
		return (parse_port_range(opt, str));
	}

	/* Here we had a single port */
	if (!is_valid_port(str)) {
		ft_printf_fd(2, "Parsing port error -> %s\n", str);
		return (FALSE);
	}
	insert_port_string(opt, str);
	// insert port here
	return (TRUE);
}

s8 parse_port_string(OptNode *opt, char *str) {
	char **splited_comma = NULL;
	u32 nb_comma = 0;
	
	if ((nb_comma = count_char(str, ',')) > 0) {
		splited_comma = ft_split_trim(str, ',');
		u32 nb_sub_string = double_char_size(splited_comma);
		if (nb_sub_string != nb_comma + 1 && nb_sub_string != nb_comma) {
			ft_printf_fd(2, "Incorect number of substring after coma split got: %u -> expected: %u or %u\n", nb_sub_string, nb_comma, nb_comma +1);
			goto error_case;
		}
		for (s32 i = 0; splited_comma[i]; i++) {
			if (!parse_substring_port_str(opt, splited_comma[i])) { goto error_case; }		
		}
		free_double_char(splited_comma);
		return (TRUE);
	}
	return (parse_substring_port_str(opt, str));

	error_case:
		free_double_char(splited_comma);
		return (FALSE);
}

s8 parse_nmap_port(void *opt_ptr, void *data) {
	OptNode *opt = opt_ptr;
	char *str = data;
	s32 i = 0, str_len = 0;

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
	return (parse_port_string(opt, str));
}
