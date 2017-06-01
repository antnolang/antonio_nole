#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define LINE_LEN 256

static const char *init_mode_str[] = {
	[CFG_DEFAULT] = "default",
	[CFG_GLIDER]  = "glider",
	[CFG_RANDOM]  = "random",
};

static const struct option long_options[] =
{
	{"help",   no_argument,       0, 'h'},
	{"size_x", required_argument, 0, 'x'},
	{"size_y", required_argument, 0, 'y'},
	{"init",   required_argument, 0, 'i'},
	{0, 0, 0, 0}
};

static bool check_config(const struct config *config);
static enum cfg_init_mode str2init_mode(const char *opt);
static bool load_config(struct config *config);

int config_parse_argv(struct config *config, int argc, char *argv[])
{
	int option_index = 0;
	int c;
	bool check_options = true;
	
	// Default values
	config->show_help = false;
	config->size_x = 10;
	config->size_y = 10;
	config->init_mode = CFG_DEFAULT;
	config->cfg_file = "\0";
	
	while ((c = getopt_long(argc, argv, "hx:y:i:", long_options,
				&option_index)) != -1) {
		switch (c) {
		case 'h':
			config->show_help = true;
			break;
		case 'x':
			config->size_x = strtol(optarg, NULL, 0);
			break;
		case 'y':
			config->size_y = strtol(optarg, NULL, 0);
			break;
		case 'i':
			config->init_mode = str2init_mode(optarg);
			break;
		case '?':
			check_options = false;
			break;
		default:
			printf("\nERROR\n");
			exit(EXIT_FAILURE);
 		}
	}
	
	if (optind != argc) {
		if (optind == argc - 1) {
			config->cfg_file = argv[optind];
			optind++;
			if (!load_config(config)) {
				check_options = false;
			}
		} else {
			check_options = false;
		}
	}

	return check_options && check_config(config);
}

static bool check_config(const struct config *config)
{
	bool correct = true;

	if (config->show_help)
		return true;

	correct &= config->size_x > 0;
	correct &= config->size_y > 0;
	correct &= config->init_mode != CFG_NOT_DEF;

	return correct;
}

static enum cfg_init_mode str2init_mode(const char *opt)
{
	int i;
	for (i = 0; i < CFG_N_INIT_MODES; i++)
		if (!strcmp(init_mode_str[i], opt))
			break;

	return i == CFG_N_INIT_MODES ? CFG_NOT_DEF : i;
}

static bool load_config(struct config *config)
{
	char file_name[LINE_LEN];
	strcpy(file_name, config->cfg_file);
	
	FILE *f = fopen(file_name, "r");
	if (!f) {
		perror("Error opening config file");
		exit(EXIT_FAILURE);
	}
	
	char line1[LINE_LEN];
	fgets(line1, LINE_LEN, f);
	if (ferror(f)) {
		perror("Error reading config file");
		fclose(f);
		return false;
	}
	if (strchr(line1, '\n') != NULL) {
		*strchr(line1, '\n') = '\0';
	}
	config->size_x = strtol(line1, NULL, 0);
	
	char line2[LINE_LEN];
	fgets(line2, LINE_LEN, f);
	if (ferror(f)) {
		perror("Error reading config file");
		fclose(f);
		return false;
	}
	if (strchr(line2, '\n') != NULL) {
		*strchr(line2, '\n') = '\0';
	}
	config->size_y = strtol(line2, NULL, 0);
	
	char line3[LINE_LEN];
	fgets(line3, LINE_LEN, f);
	if (ferror(f)) {
		perror("Error reading config file");
		fclose(f);
		return false;
	}
	if (strchr(line3, '\n') != NULL) {
		*strchr(line3, '\n') = '\0';
	}
	config->init_mode = str2init_mode(line3);
	
	fclose(f);
	return true;
}

void config_print_usage(const char *arg0)
{
	printf("Usage: %s\n"
		"\t[-h|--help]\n"
		"\t[-x|--size_x <num>]\n"
		"\t[-y|--size_y <num>]\n"
		"\t[-i|--init <init_mode>]\n"
		"\t[<config_file.txt>]\n"
		, arg0);

	printf("\ninitialization modes: \n");
	
	int i;
	for (i = 0; i < CFG_N_INIT_MODES; i++)
		printf("\t%s\n", init_mode_str[i]);
		
	printf("\nData from the config file has preference over flags.\n\n");
}

void config_print(const struct config *config)
{
	printf("config {\n");
	printf("\tshow help = %s\n", config->show_help ? "TRUE" : "FALSE");
	printf("\tsize_x    = %d\n", config->size_x);
	printf("\tsize_y    = %d\n", config->size_y);
	printf("\tinit_mode = %d(%s)\n",
		config->init_mode, init_mode_str[config->init_mode]);
	printf("\tcfg_file  = %s\n", config->cfg_file);
	printf("}\n");
}
