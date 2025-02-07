#ifndef NMAP_H
#define NMAP_H 1

#include "../libft.h"
#include "../parse_flag/parse_flag.h"

/* Flag enum */
typedef enum FlagEnum {
	FLAG_TYPE_SCAN = 1, // easy parse just check if char is in enum TypeScan
	FLAG_IP_ADDR = 2, // need special logic detect ip ipv4 format is ok and transform domaine name to ip addr if needed
	FLAG_IP_FILE = 4, // Need special logic, read the file and apply ip addr policy
	FLAG_NB_THREAD = 8, // easy parse digit must be between 1 and 250
	FLAG_PORT = 16, // parse done
	FLAG_HELP = 32, // no parse
} FlagEnum;

typedef enum FlagChar {
	FLAG_TYPE_SCAN_CHAR = 's',
	FLAG_IP_ADDR_CHAR = 'i',
	FLAG_IP_FILE_CHAR = 'f',
	FLAG_NB_THREAD_CHAR = 't',
	FLAG_PORT_CHAR = 'p',
	FLAG_HELP_CHAR = 'h',
} FlagChar;

#define FLAG_TYPE_SCAN_STR "scan"
#define FLAG_IP_ADDR_STR "ip"
#define FLAG_IP_FILE_STR "file"
#define FLAG_NB_THREAD_STR "speedup"
#define FLAG_PORT_STR "ports"
#define FLAG_HELP_STR "help" 


#define NMAP_HELP_STR "ft_nmap [OPTIONS]\n\
  --help Print this help screen\n\
  --ports ports to scan (eg: 1-10 or 1,2,3 or 1,5-15)\n\
  --ip ip addresses to scan in dot format\n\
  --file File name containing IP addresses to scan,\n\
  --speedup [250 max] number of parallel threads to use\n\
  --scan SYN/NULL/FIN/XMAS/ACK/UDP\n"

typedef enum TypeScan {
	SYN_SCAN = 'S',
	NULL_SCAN = 'N',
	FIN_SCAN = 'F',
	XMAS_SCAN = 'X',
	ACK_SCAN = 'A',
	UDP_SCAN = 'U',
} TypeScan;


/* Default range port */
#define DEFAULT_RANGE_PORT_STR "1-1024"

/* Max port can be scan */
#define MAX_PORT_SCAN 1024

/* Max number of threads */
#define MAX_THREAD 250

/* Function to extend port string to int port list */
s8 extend_port_string(FlagContext *c, u32 flag);

/* Function given to flag parser to parse port opt */
s8 parse_nmap_port(void *opt_ptr, void *data);


FT_INLINE s8 scan_already_present(t_list *scan_lst, char scan) {
	while (scan_lst) {
		U_OptValue *val = scan_lst->content;
		if (val->str[0] == scan) { return (TRUE); }
		scan_lst = scan_lst->next;
	}
	return (FALSE);
}

FT_INLINE s8 insert_scan_string(OptNode *opt, char scan) {
	U_OptValue *opt_val = NULL;
	opt_val = opt_val_new();
	if (!opt_val) { return (ERROR_SET_VALUE); }
	if (scan_already_present(opt->val_lst, scan)) { return (SUCCESS_SET_VALUE); }
	
	char *str = ft_calloc(2, sizeof(char));
	if (!str) { return (ERROR_SET_VALUE); }
	str[0] = scan;
	
	return (insert_string_val(opt, opt_val, str));
}

FT_INLINE s8 parse_nmap_type_scan(void *opt_ptr, void *data) {
	OptNode *otp = (OptNode *)opt_ptr;
	char *str = (char *)data;
	TypeScan type_scan = 0;
	s32 i = 0;

	while (str[i]) {
		type_scan = str[i];
		if (type_scan != SYN_SCAN && type_scan != NULL_SCAN && type_scan != FIN_SCAN && type_scan != XMAS_SCAN && type_scan != ACK_SCAN && type_scan != UDP_SCAN) {
			ft_printf_fd(2, "Error: invalid scan type\n");
			return (FALSE);
		}
		if (insert_scan_string(otp, type_scan) == ERROR_SET_VALUE) {
			return (FALSE);
		}
		i++;
	}
	return (TRUE);
}

FT_INLINE FlagContext *init_nmap_flag(char **argv) {
	FlagContext *c = flag_context_init(argv);


	// Init type scan option
	add_flag_option(c, FLAG_TYPE_SCAN_STR, FLAG_TYPE_SCAN, FLAG_TYPE_SCAN_CHAR);
	set_flag_option(c, FLAG_TYPE_SCAN, EOPT_VALUE_TYPE, CUSTOM_VALUE);
	set_flag_option(c, FLAG_TYPE_SCAN, EOPT_MULTIPLE_VAL, VALUE_APPEND);
	set_flag_option(c, FLAG_TYPE_SCAN, EOPT_PARSE_FUNC, parse_nmap_type_scan);
	set_flag_option(c, FLAG_TYPE_SCAN, EOPT_ADD_VAL_AFTER_PARSE, FALSE);

	// Init ip addr option
	add_flag_option(c, FLAG_IP_ADDR_STR, FLAG_IP_ADDR, FLAG_IP_ADDR_CHAR);
	set_flag_option(c, FLAG_IP_ADDR, EOPT_VALUE_TYPE, CHAR_VALUE);
	set_flag_option(c, FLAG_IP_ADDR, EOPT_MULTIPLE_VAL, VALUE_APPEND);
	// set_flag_option(c, FLAG_IP_ADDR, EOPT_PARSE_FUNC, parse_ip_addr);

	// Init ip file option
	add_flag_option(c, FLAG_IP_FILE_STR, FLAG_IP_FILE, FLAG_IP_FILE_CHAR);
	set_flag_option(c, FLAG_IP_FILE, EOPT_VALUE_TYPE, CHAR_VALUE);
	set_flag_option(c, FLAG_IP_FILE, EOPT_MULTIPLE_VAL, VALUE_APPEND);
	// set_flag_option(c, FLAG_IP_FILE, EOPT_PARSE_FUNC, parse_ip_file);

	// Init nb thread (speedup) option
	add_flag_option(c, FLAG_NB_THREAD_STR, FLAG_NB_THREAD, FLAG_NB_THREAD_CHAR);
	set_flag_option(c, FLAG_NB_THREAD, EOPT_VALUE_TYPE, DECIMAL_VALUE);
	set_flag_option(c, FLAG_NB_THREAD, EOPT_MAX_VAL, MAX_THREAD);
	set_flag_option(c, FLAG_NB_THREAD, EOPT_MULTIPLE_VAL, VALUE_NO_OVERRID);

	// Init port option
	add_flag_option(c, FLAG_PORT_STR, FLAG_PORT, FLAG_PORT_CHAR);
	set_flag_option(c, FLAG_PORT, EOPT_VALUE_TYPE, CUSTOM_VALUE);
	set_flag_option(c, FLAG_PORT, EOPT_MULTIPLE_VAL, VALUE_NO_OVERRID);
	// Maybe need to refact parse nmap port, avoid to store splited string in opt_node data, this cause no desired behavior for VALUE_APPEND ...
	set_flag_option(c, FLAG_PORT, EOPT_PARSE_FUNC, parse_nmap_port);
	// set_flag_option(c, FLAG_PORT, EOPT_ADD_VAL_AFTER_PARSE, FALSE);

	// Init help option
	add_flag_option(c, FLAG_HELP_STR, FLAG_HELP, FLAG_HELP_CHAR);
	set_flag_option(c, FLAG_HELP, EOPT_VALUE_TYPE, OPT_NO_VALUE);

	return (c);
}


#endif